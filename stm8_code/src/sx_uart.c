#include "stm8l15x.h"
#include "sx_board.h"
#include "stdlib.h"
#include "sx_autoC.h"

/*****************************************************************************/
//uint8_t client_infor[5]={11,12,13,14,15};//���û�����д�������
uint8_t urtRxComm[64];
uint8_t urt_rx_buff[64];
uint8_t urt_tx_buff[64];
uint8_t urtStep;
uint8_t urtRxCount;
uint8_t urtRxNum;
uint32_t urtTimeNow;
uint8_t lora_send[32];

//����·�Ʒ��������ݽṹ��
brightness bp;//���Ȱٷֱ�(2)
Single_LF145C1_pama  lf145c1;//����LF145C1�����ṹ��(3)
Max_min_value mmvalue_thisday,mmvalue_lastday;//��ֵ�ṹ��(4),(5)
History_someday data_history;//��ʷ����(6)
State_thisday state;//����״̬(7)
URT_auto_stratage stra;//�������Ʋ��Խṹ�壨8��
llf145c1_addr light_addr_1 = {0x19 , 0x6C};
llf145c1_addr light_addr_2 = {0x19 , 0x6C};//��·�������ַ��10��
//Brightness_level bright_time;//����ʱ�䣬��ص�ѹ�ȼ�
sun_control sun_level_set , sun_level_get;//��ص�ѹ�ȼ�(11)(12)
client_infor client_r , client_w;//��/д�ͻ���Ϣ(13)(14)

///////////////2019/7/2 duke
//�������ݴ����͵Ľṹ������
static const data_manage struct_id_get[20] = {
  {0x01 , (uint8_t*)&bp , sizeof(bp)}, //2
  {0x03 , (uint8_t*)&lf145c1 , sizeof(lf145c1)}, //3
  {0x04 , (uint8_t*)&mmvalue_thisday , sizeof(mmvalue_thisday)}, //4
  {0x05 , (uint8_t*)&mmvalue_lastday , sizeof(mmvalue_lastday)}, //5
  {0x0D , (uint8_t*)&data_history, sizeof(data_history)}, //6
  {0x07 , (uint8_t*)&state , sizeof(state)}, //7
  {0x0C , (uint8_t*)&stra , sizeof(stra)}, //8
  {0x09 , (uint8_t*)&light_addr_1 , sizeof(light_addr_1)}, //10_1
  {0x02 , (uint8_t*)&light_addr_2 , sizeof(light_addr_2)}, //10_2
  {0x0E , (uint8_t*)&sun_level_set , sizeof(sun_level_set)}, //11
  {0x1E , (uint8_t*)&sun_level_get , sizeof(sun_level_get)}, //12
  {0x06 , (uint8_t*)&client_r , sizeof(client_r)}, //13
  {0x08 , (uint8_t*)&client_w , sizeof(client_w)}, //14
};

//׼����·�Ʒ������ݵĽṹ��
brightness_set bright_all , bright_single;// �㲥/�ǹ㲥��ʽ���ƵƵ����ȣ�1����2��
history_data history_days;//��ȡĳһ�����ʷ������6��
URT_auto_stratage_set stra_set;//���Ʋ������ã�9��
sun_set sun_vol_set; //��ѯ��ص�ѹ���11��
get_client_infor client_set;//д�ͻ���Ϣ ��14��

static const data_manage struct_id_set[20] = {
  {0x11 , (uint8_t*)&bright_all , 3},//1
  {0x01 , (uint8_t*)&bright_single , 3},//2
  {0x0D , (uint8_t*)&history_days , 1},//6
  {0x0B , (uint8_t*)&stra_set , 15},//9
  {0x0E , (uint8_t*)&sun_vol_set , 1},//11
  {0x08 , (uint8_t*)&client_set , 5},//14
};

//�ṹ��������ϲ����ݺ�ִ����Ӧ�Ĺ���
void struct_function(uint8_t struct_type){
  switch(struct_type){
  case 0x11:
    Brightness_Setting(bright_all.brightness_percent , bright_all.brightness_time , 0);
    atv.data_flag = 1;
    break;
    
  case 0x01:
    Brightness_Setting(bright_single.brightness_percent , bright_single.brightness_time , 1);
    break;
    
  case 0x0B:
    Light_Stra_Setting();
    break;
    
  case 0x0E:
    SunCtrl_Setting(sun_vol_set.sunctrl_level);
    break;
    
  case 0x08:
    Client_Infor_Write((uint8_t*)&client_set);
    atv.data_flag = 1;
    break;
  }
}

/////////////2019/7/2 duke
//�ϲ�ȡ�ýṹ������tx
void send_data_deliver(uint8_t* buffer , uint8_t* data_size_p_t , uint8_t struct_type){
  uint8_t ii;
  uint8_t jj;
  uint8_t* data_p;
     
  for(ii=0 ; ii<20 ; ii++){
    if(struct_id_get[ii].struct_type == struct_type){
      data_p = struct_id_get[ii].struct_p;
      *data_size_p_t = struct_id_get[ii].struct_size;
      for(jj=0 ; jj<*data_size_p_t ; jj++){
        buffer[jj] = data_p[jj];
      }
      break;
    }
  }      
}
//�ϲ㴫�����ݵ��ṹ��rx
void recv_data_deliver(uint8_t* buffer , uint8_t* data_size_p_r , uint8_t struct_type){
  uint8_t ii;
  uint8_t jj;
  uint8_t* data_p;
  if(*data_size_p_r > 0){//��������������Ч��������λ
    for(ii=0 ; ii<20 ; ii++){
      if(struct_id_set[ii].struct_type == struct_type){
        data_p = struct_id_set[ii].struct_p;
        for(jj=0 ; jj<*data_size_p_r ; jj++){
          data_p[jj] = buffer[jj];
        }
        *data_size_p_r = 0;
        struct_function(struct_type); //ִ�ж�Ӧ�Ĺ��ܺ���
        break;
      }
      
    }
    
  }
  else{ //��������������Ч����û������λ
    switch(struct_type){  
      case 0x09:
      //Address_RQS(struct_type);
      break;
    
    case 0x02:
      //Address_RQS(struct_type);
      break;;
    }
    atv.data_flag = 1;//�����������õ�ȡֵָ��
  }
}

void UART1_Init(unsigned int baudrate)
{ 
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE);
  USART_Init(USART1,                //USART1
            baudrate,               //������
            USART_WordLength_8b,    //���ݳ���
            USART_StopBits_1,       //
            USART_Parity_No,        //
            USART_Mode_TxRX);  //
  
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART1 , ENABLE);
}

//
//void urt1SendStr(uint8_t *a, uint8_t num){
//  
//	HAL_UART_Transmit_IT(&huart1,(uint8_t*)a, num);
//}	

void urt1SendChar(uint8_t a){
    USART1->DR = a;
    sys_delay(100);
}

void urt1SendStr(uint8_t *Str,uint8_t size) 
{       
     uint8_t ii;
        for(ii=0;ii<size;)//��Ϊ����
        {
            USART1->DR = Str[ii];     //�������� 
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));//�ȴ��������
            ii++;//��һ������
        }
}
/////ԭ�жϽ��պ���
/*
void It_Get_Byte(void){//�����ж���ִ��
    uint8_t num;
    num = USART1->DR;
    urtTimeNow=sys_time; ;
    if((sys_time-urtTimeNow)>5000) urtStep=0; //һ��Ҫ�������λ���ϣ���Ȼ���������
    //USART_SendData8(USART1, num);
    switch(urtStep){
            case 0:               
                if(num==0xA5){                    
                    urtRxCount=0;
                    urt_rx_buff[urtRxCount]=num;					
                    urtStep=1;
                }else{urtStep=0;}
                break;
            case 1:                
                if(num==0x5A){                      
                     urtRxCount++;
                     urt_rx_buff[urtRxCount]=num;
                     urtStep=2;							
                }else{ urtStep=0;}
                break;
            case 2:
                urtRxCount++;
                urt_rx_buff[urtRxCount]=num;
                urtRxNum=num+3; //
                urtStep=3;                     
                break;
            case 3:				
                urtRxCount++;
                urtRxCount &= 0x3f;
                urt_rx_buff[urtRxCount]=num;
                if(num==0xEE && urtRxCount==urtRxNum){                                                      
                  bd.rxNew=1;
                  urtStep=0;
                }
                break;
            default:  urtStep=0; break;
     }  
}
*/

//////////////////////�Ա�д�жϽ��պ���////////////

void It_Get_Byte(void){//�����ж���ִ��
    uint8_t num;
    num = USART1->DR;
    urtTimeNow=sys_time; ;
    if((sys_time-urtTimeNow)>5000) urtStep=0; //һ��Ҫ�������λ���ϣ���Ȼ���������
    //USART_SendData8(USART1, num);
    switch(urtStep){
            case 0:               
                if(num==0xA5){                    
                    urtRxCount=0;
                    urt_rx_buff[urtRxCount]=num;					
                    urtStep=1;
                }else{urtStep=0;}
                break;
            case 1:                
                urtRxCount++;
                urt_rx_buff[urtRxCount]=num;
                urtRxNum=num; //
                urtStep=2;                     
                break;             
            case 2:				
                urtRxCount++;
                urtRxCount &= 0x3f;
                urt_rx_buff[urtRxCount]=num;
//                if(num==0x0D && (urtRxCount==urtRxNum-1)){   //
                if(num==0x0D ){   
                  bd.rxNew=1;
                  urtStep=0;
                }
                break;
            default:  urtStep=0; break;
     }  
}




////////ԭ���մ�����
/*
void uart1Manage(void)
{//���մ�����
    uint8_t ii;
    
    if(bd.rxNew) bd.rxNew=0; else return;
    
    for(ii=0;ii<64;ii++)urtRxComm[ii]=urt_rx_buff[ii];      
    switch(urtRxComm[3]){
            case 0x00:                 
                    urt1SendChar(rand());
                    break;
            case 0x01:
              urt1SendChar(1);
                    break;
            case 0x02:
              //eep_wr_test();
              urt1SendChar(2);
                    break;
            case 0x03:
              //eep_re_test();
              //urt1SendChar(3);
                    break;
            case 0x04:
              urt1SendChar(4);
                    break;
            case 0x05:
              urt1SendChar(5);
                    break;
            case 0x06:
                    urt1RM06();
                    break;
            case 0x07:                    
                    break;
    }	
}
*/
///////�Ա�д���ܴ�����

void uart1Manage(void)
{//���մ�����
    uint8_t ii;
    //uint8_t pack_size=urt_rx_buff[1];
   // uint8_t sum=0;
    if(bd.rxNew) bd.rxNew=0; else return;
    
//    for(ii=1;ii<=(pack_size-3);ii++)
//    {
//        sum = urt_rx_buff[ii] + sum;
//    } 
//    if(sum!=urt_rx_buff[pack_size-2]) return;
   
    for(ii=0;ii<64;ii++)urtRxComm[ii]=urt_rx_buff[ii];      
    switch(urtRxComm[4]){
            case 0x44:                 
                    ////����ʹ��
                    //fun_for_debug();
                    
                    break;   
            case 0x81:                 
                    ////�ǹ㲥��ʽ�������ȷ���֡��ָ��2��
                    bp.brightness_value=urtRxComm[5];
                    //if(bp.brightness_value != data_r[1];
                    atv.data_flag = 1;
                    break;
            case 0x83:
              ///////////��ȡ����LF145C1����ָ���֡��ָ��3��
                    lf145c1.light_vol=(urtRxComm[6]<<8)|urtRxComm[5];
                    lf145c1.battery_vol=(urtRxComm[8]<<8)|urtRxComm[7];
                    lf145c1.current_front=urtRxComm[9];
                    lf145c1.current_value=urtRxComm[10];
                    lf145c1.led_vol=(urtRxComm[12]<<8)|urtRxComm[11];
                    lf145c1.temple=(urtRxComm[14]<<8)|urtRxComm[13];
                    break;
            case 0x84:
              ///////////��ȡ������ֵָ���֡��ָ��4��
                    mmvalue_thisday.charge_current_max=(urtRxComm[6]<<8)|urtRxComm[5];
                    mmvalue_thisday.discharge_current_max=(urtRxComm[8]<<8)|urtRxComm[7];
                    mmvalue_thisday.battery_value_max=(urtRxComm[10]<<8)|urtRxComm[9];
                    mmvalue_thisday.battery_value_min=(urtRxComm[12])|urtRxComm[11];
                    mmvalue_thisday.temple_max=urtRxComm[13];
                    mmvalue_thisday.temple_flag_max=urtRxComm[14];
                    mmvalue_thisday.temple_min=urtRxComm[15];
                    mmvalue_thisday.temple_flag_min=urtRxComm[16];
                    mmvalue_thisday.charge_AH=(urtRxComm[18]<<8)|urtRxComm[17];
                    mmvalue_thisday.discharge_AH=(urtRxComm[20]<<8)|urtRxComm[19];
                    break;
            case 0x85:
              ///////////��ȡǰһ����ֵָ���֡��ָ��5��
                    mmvalue_lastday.charge_current_max=(urtRxComm[6])|urtRxComm[5];
                    mmvalue_lastday.discharge_current_max=(urtRxComm[8]<<8)|urtRxComm[7];
                    mmvalue_lastday.battery_value_max=(urtRxComm[10]<<8)|urtRxComm[9];
                    mmvalue_lastday.battery_value_min=(urtRxComm[12]<<8)|urtRxComm[11];
                    mmvalue_lastday.temple_max=urtRxComm[13];
                    mmvalue_lastday.temple_flag_max=urtRxComm[14];
                    mmvalue_lastday.temple_min=urtRxComm[15];
                    mmvalue_lastday.temple_flag_min=urtRxComm[16];
                    mmvalue_lastday.charge_AH=(urtRxComm[18]<<8)|urtRxComm[17];
                    mmvalue_lastday.discharge_AH=(urtRxComm[20]<<8)|urtRxComm[19]; 
                    break; 
            case 0x8D:
              ///////////��ȡĳһ�����ʷ���ݷ���֡��ָ��6��
                    data_history.related_history=urtRxComm[5];
                    data_history.battery_value_max_history=urtRxComm[6];
                    data_history.charge_AH_history=(urtRxComm[8]<<8)|urtRxComm[7];
                    data_history.discharge_AH_history=(urtRxComm[10]<<8)|urtRxComm[9];  
                    break; 
            case 0x87:
              ///////////��ȡ����״̬��Ϣָ���֡��ָ��7��
                    state.step=urtRxComm[5];
                    state.dayornight=urtRxComm[6];
                    state.count=urtRxComm[7];
                    state.time_day=((u32)urtRxComm[11]<<24)|((u32)urtRxComm[10]<<16)|(urtRxComm[9]<<8)|(urtRxComm[8]);
                    state.time_night=((u32)urtRxComm[15]<<24)|((u32)urtRxComm[14]<<16)|(urtRxComm[13]<<8)|(urtRxComm[12]);
                    state.charge_AS_day=((u32)urtRxComm[19]<<24)|((u32)urtRxComm[18]<<16)|(urtRxComm[17]<<8)|(urtRxComm[16]);
                    state.discharge_AS_night=((u32)urtRxComm[23]<<24)|((u32)urtRxComm[22]<<16)|(urtRxComm[21]<<8)|(urtRxComm[20]);
                    break; 
            case 0x8C:
              ///////////���Ʋ��Զ�ȡָ���֡��ָ��8��
                    stra.time1=urtRxComm[5];
                    stra.level1=urtRxComm[6];
                    stra.speed1=urtRxComm[7];
                    stra.time2=urtRxComm[8];
                    stra.level2=urtRxComm[9];
                    stra.speed2=urtRxComm[10];
                    stra.time3=urtRxComm[11];
                    stra.level3=urtRxComm[12];
                    stra.speed3=urtRxComm[13];
                    stra.time4=urtRxComm[14];
                    stra.level4=urtRxComm[15];
                    stra.speed4=urtRxComm[16];
                    stra.time5=urtRxComm[17];
                    stra.level5=urtRxComm[18];
                    stra.speed5=urtRxComm[19];
                    break; 
            case 0x8B:
              ///////////���Ʋ�������ָ���֡��ָ��9��
                    
                    atv.data_flag = 1;
                    break; 
            case 0x89:
              ///////////��ȡ��ַ�����֡��ָ��10��
                    //light_addr_1.addr_h = urtRxComm[5];
                    //light_addr_1.addr_l = urtRxComm[6];
                    //atv.data_flag = 1;
                    break; 
            case 0x8E:
              ///////////���ù�ص�ѹ����������ָ���֡��ָ��11��
                    sun_level_set.sunctrl_level=urtRxComm[5];
                    atv.data_flag = 1;
                    break; 
            case 0x9E:
              ///////////��ѯ��ص�ѹ����������ָ���֡��ָ��12��
                    sun_level_get.sunctrl_level=urtRxComm[5];
                    break; 
            case 0x86:
              ///////////��ȡ�ͻ���Ϣָ���֡��ָ��13��
                    client_r.save_byte = urtRxComm[5];
                    client_r.byte1 = urtRxComm[6];
                    client_r.byte2 = urtRxComm[7];
                    client_r.byte3 = urtRxComm[8];
                    client_r.byte4 = urtRxComm[9];
                    client_r.byte5 = urtRxComm[10];
                    atv.data_flag = 1;
                    break; 
            case 0x88:
              ///////////д�ͻ���Ϣָ���֡��ָ��14��
                    client_w.save_byte = urtRxComm[5];
                    client_w.byte1 = urtRxComm[6];
                    client_w.byte2 = urtRxComm[7];
                    client_w.byte3 = urtRxComm[8];
                    client_w.byte4 = urtRxComm[9];
                    client_w.byte5 = urtRxComm[10];
                    atv.data_flag = 1;
                    
                    //atv.data_flag = 1;
                    
                    break; 
            default:
                    break;
    }	
}

////////////////////����ʹ��
/*
void fun_for_debug(void)
{
  switch(urtRxComm[5]){
          case 0x01:
              ////�㲥��ʽ�������ȣ�ָ��1��
            Brightness_Setting(50,100,1);
                  break; 
          case 0x02:
              //////�ǹ㲥��ʽ�������ȣ�ָ��2��
            Brightness_Setting(50,100,0);
                  break; 
          case 0x03:
              ///��ȡ����LF145C1������ָ��3��
            Read_LF145C1_par(SINGLE_LF145C1_SEND);
                  break; 
          case 0x04:
              ///��ȡ������ֵ��ָ��4��
            Read_LF145C1_par(THISDAY_MAXMIN_SEND);
                  break; 
          case 0x05:
              ///��ȡǰһ����ֵ��ָ��5��
            Read_LF145C1_par(LASTDAY_MAXMIN_SEND);
                  break; 
          case 0x06:
              ///��ȡĳһ�����ʷ���ݣ�ָ��6��
            History_RQS(2);
                  break; 
          case 0x07:
              ///��ȡ����״̬��Ϣ��ָ��7��
            Read_LF145C1_par(THISDAY_STATE_SEND);
                  break; 
          case 0x08:
              ///���Ʋ��Զ�ȡ��ָ��8��
            Read_LF145C1_par(LIGHTING_STRA_SEND);
                  break; 
          case 0x09:
              ///���Ʋ������ã�ָ��9��
            Light_Stra_Setting();
                  break; 
          case 0x0A:
              ///��ȡ��ַ��ָ��10��
            Address_RQS();
                  break; 
          case 0x0B:
              ///���ù�ص�ѹ������������ָ��11��
            SunCtrl_Setting(55);
                  break; 
          case 0x0C:
              ///��ѯ��ص�ѹ������������ָ��12��
            SunCtrl_RQS();
                  break; 
          case 0x0D:
              ///��ȡ�ͻ���Ϣ��ָ��13��
            Read_LF145C1_par(CLIENT_INFOR_SEND);
                  break; 
          case 0x0E:
              ///д�ͻ���Ϣ��ָ��14��
            //Client_Infor_Write();
                  break; 
          case 0x0F: 
            //////д
                  break;
          case 0x10: 
            //////��
                  break;
                            
          default:
                  break;
    
  }
  

}

*/
void urt1RM06(void){
	static uint8_t temp;
	uint8_t t[16];
	uint8_t r[64];
	uint8_t size;
	switch(urtRxComm[4]){
            case 0x00: //read		                   
                  for(temp=0;temp<16;temp++){				
                          SX1278Read(urtRxComm[5]+temp, &t[temp]);			
                  }
                  urt1SendStr(t,16);		
                    break;
            case 0x01: //write
                    SX1278Write(urtRxComm[5],urtRxComm[6]);			
                    urt1SendChar(0xaa);	
                    break;
            case 0x03: //buffer read
                    for(temp=0;temp<16;temp++) t[temp]=0x30+temp;
                    urt1SendStr(t,16);
                    break;
            case 0x04: //buffer write
                    
                    break;
            case 0x05: //buffer write					
                   if(SX1278Init()) urt1SendChar(0xa1);
                   else urt1SendChar(0xa0);
                    break;
            case 0x06: //                   
                   urt1SendChar(0xaa);
                    break;
            case 0x07: //
                   changeSettings();
                    break;
            case 0x08: //���߷������ݰ�
                   t[0]=0x12; t[1]=0x34; t[2]=0x56; t[3]=0x78; t[4]=0xC0; t[5]=0x00; t[6]=0x00;
                    SX1278LoRaSetTxPacket(t,7);
                    break;
            case 0x09: //RXSTART
                    SX1278LoRaStartRx();
                    SX1278Read(0x01, &temp);
                    urt1SendChar(temp);
                    break;
            case 0x11: //���߽������ݰ�		
                    SX1278LoRaGetRxPacket((void *)r, &size);
                    if(size<1) urt1SendChar(0xff);
                    else urt1SendStr(r,size);
                    break;
	}
}
 

void changeSettings(void){
  	static uint8_t temp1;
        switch(urtRxComm[5]){
            case 0x00: //change SpreadingFactor		
                    SX1278LoRaSetSpreadingFactor(urtRxComm[6]);
                    urt1SendChar(0x1E);
                    SX1278Read(0x1E, &temp1);
                    urt1SendChar(temp1);
                    break;
            case 0x01: //change singlBw
                    SX1278LoRaSetSignalBandwidth(urtRxComm[6]);
                    urt1SendChar(0x1D);
                    SX1278Read(0x1D, &temp1);
                    urt1SendChar(temp1);
                    break;
            case 0x02: //change ErrorCoding
                    SX1278LoRaSetErrorCoding(urtRxComm[6]);
                    urt1SendChar(0x1D);
                    SX1278Read(0x1D, &temp1);
                    urt1SendChar(temp1);
                    break;
            case 0x03: //change RFFrequency
                    SX1278Write(REG_LR_FRFMSB,urtRxComm[6]);
                    SX1278Write(REG_LR_FRFMID,urtRxComm[7]);
                    SX1278Write(REG_LR_FRFLSB,urtRxComm[8]);   
                    urt1SendChar(0x06);
                    SX1278Read(0x06, &temp1);
                    urt1SendChar(temp1);

                    break;
            case 0x04: //change power
                    SX1278LoRaSetRFPower(urtRxComm[6]);
                    urt1SendChar(0x09);
                    SX1278Read(0x09, &temp1);
                    urt1SendChar(temp1);
                    break;
            case 0x05: //buffer write					
                    break;
            case 0x06: //
                    break;
            case 0x07: //
                    break;
            case 0x08: //
                    break;
            case 0x10: //RXSTART
                    break;
            case 0x11: //			
                    break;
	}
}

















///////////////
//��·�ư�ͨ�����ݰ��������
//����ʱ�䣺2019.6.28


///////////////
//��������,all�����Ƿ�㲥(1 , 2)
void Brightness_Setting(uint8_t percent,uint16_t time,uint8_t all)
{
  uint8_t ii;
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x0A;
  if(all){
      urt_tx_buff[2]=0x19;
      urt_tx_buff[3]=0x6C; 
  }
  else{
      urt_tx_buff[2]=0xff;
      urt_tx_buff[3]=0xff; 
      atv.data_flag = 1;
  }
  urt_tx_buff[4]=0x01;
  urt_tx_buff[5]=percent;
  urt_tx_buff[6]=time;
  urt_tx_buff[7]=0xff&(time>>8);  
  urt_tx_buff[8]=0;
  for(ii=1;ii<8;ii++) urt_tx_buff[8]+=urt_tx_buff[ii];
  urt_tx_buff[9]=0x5A;
  urt1SendStr(urt_tx_buff,10);  
}

///////////////
//��ȡ����ָ��(3 , 4 , 5 , 7 , 8)
void Read_LF145C1_par(uint8_t type)
{
  uint8_t ii;
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x07;
  urt_tx_buff[2]=0x19;
  urt_tx_buff[3]=0x6C;
  urt_tx_buff[4]=type;
  urt_tx_buff[5]=0;
  for(ii=1;ii<5;ii++) urt_tx_buff[5]+=urt_tx_buff[ii];
  urt_tx_buff[6]=0x5A;
  urt1SendStr(urt_tx_buff,7);
}

//////////////
//��ȡ֮ǰĳһ����ʷ����(6)
void History_RQS(uint8_t days)
{
  uint8_t ii;
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x08;
  urt_tx_buff[2]=0x19;
  urt_tx_buff[3]=0x6C;
  urt_tx_buff[4]=0x0D;
  urt_tx_buff[5]=days;
  urt_tx_buff[6]=0;
  for(ii=1;ii<6;ii++) urt_tx_buff[6]+=urt_tx_buff[ii];
  urt_tx_buff[7]=0x5A;
  urt1SendStr(urt_tx_buff,8);

}

///////////////
//��ȡ��ַ����(10)
void Address_RQS(uint8_t struct_type)
{
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x09;
  urt_tx_buff[2]=0xff;
  urt_tx_buff[3]=0xff;
  urt_tx_buff[4]=0x09;
  urt_tx_buff[7]=0;
  
  switch(struct_type){
  case 0x09:
  urt_tx_buff[5]=0xAD;
  urt_tx_buff[6]=0xDA;
  break;
  
  case 0x02:
  urt_tx_buff[5]=0xAC;
  urt_tx_buff[6]=0xCA;
  break;
  //case 0x03:
  //urt_tx_buff[5]=0xAB;
  //urt_tx_buff[6]=0xBA;
  }
  uint8_t ii;
  for(ii=1;ii<7;ii++) urt_tx_buff[7]+=urt_tx_buff[ii];
  urt_tx_buff[8]=0x5A;
  urt1SendStr(urt_tx_buff,9);
}

/////////////////
//���Ʋ�������(9)
void Light_Stra_Setting()
{
  uint8_t ii;
  uint8_t *p;
  p=(uint8_t *)&stra_set;
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x17;
  urt_tx_buff[2]=0x19;
  urt_tx_buff[3]=0x6C;
  urt_tx_buff[4]=0x0B;
  for(ii=0;ii<15;ii++) urt_tx_buff[ii+5]=p[ii]; 
  urt_tx_buff[20]=0;
  for(ii=5;ii<20;ii++) urt_tx_buff[20]+=urt_tx_buff[ii];
  urt_tx_buff[21]=0;
  for(ii=1;ii<21;ii++) urt_tx_buff[21]+=urt_tx_buff[ii];
  urt_tx_buff[22]=0x5A;
  urt1SendStr(urt_tx_buff,23);
}

/////////////
//���ù�ص�ѹ��levelΪ�ȼ�(11)
void SunCtrl_Setting(uint8_t level)
{
  uint8_t ii;
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x08;
  urt_tx_buff[2]=0x19;
  urt_tx_buff[3]=0x6C;
  urt_tx_buff[4]=0x0E;
  urt_tx_buff[5]=level;
  urt_tx_buff[6]=0;
  for(ii=1;ii<6;ii++) urt_tx_buff[6]+=urt_tx_buff[ii];
  urt_tx_buff[7]=0x5A;
  urt1SendStr(urt_tx_buff,8);

}

/////////////
//��ѯ��ص�ѹ(12)
void SunCtrl_RQS(void)
{
  uint8_t ii;
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x08;
  urt_tx_buff[2]=0x19;
  urt_tx_buff[3]=0x6C;
  urt_tx_buff[4]=0x1E;
  urt_tx_buff[5]=0x00;
  urt_tx_buff[6]=0;
  for(ii=1;ii<6;ii++) urt_tx_buff[6]+=urt_tx_buff[ii];
  urt_tx_buff[7]=0x5A;
  urt1SendStr(urt_tx_buff,8);

}

///////////////
//д�ͻ���Ϣ(14)
void Client_Infor_Write(uint8_t *infor)
{
  uint8_t ii;
  urt_tx_buff[0]=0xA5;
  urt_tx_buff[1]=0x0D;
  urt_tx_buff[2]=0x19;
  urt_tx_buff[3]=0x6C;
  urt_tx_buff[4]=0x08;
  urt_tx_buff[5]=0x5A;
  for(ii=6;ii<11;ii++) urt_tx_buff[ii]=infor[ii-6]; 
  urt_tx_buff[11]=0;
  for(ii=1;ii<11;ii++) urt_tx_buff[11]+=urt_tx_buff[ii];
  urt_tx_buff[12]=0x5A;
  urt1SendStr(urt_tx_buff,13);
}

