#include "stm8l15x.h"
#include "sx_board.h"
#include "stdlib.h"
#include "sx_uart.h"

extern uint8_t lora_recv[32];
//extern uint8_t lora_send[32];
uint8_t lora_recv_size;
STRU_auto_pack atpr, atpt;      //�����,���հ��뷢�Ͱ�
STRU_auto_ver  atv;             //������ز�������
STRU_eep_addr  atad, atadt;     //�����ַ����ʱ��ַ
STRU_auto_buff abf;//lora��buff������
package_data data_r;//�������ݻ�����
package_data data_t;//�������ݻ�����
STRU_auto_buff test;
frequency_control frequency_val;
/**********************����EEPROM�Ķ�д��������*******************************/
// 0x1000-0x13ff, 1page=1block=128bytes  sum=8pages =8blocks
//


//--������д�뵽EEPROM��

//void eeprom_write(uint8_t *p)
//{
//  uint8_t ii;
//  FLASH_Unlock(FLASH_MemType_Data);//����
//  //FLASH_EraseBlock(1,FLASH_MemType_Data);//����
//  for(ii=0;ii<SIZE_OF_STRU_EEPHEADER;ii++){
//    while(!FLASH_GetFlagStatus(FLASH_FLAG_EOP));
//    FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+ii,p[ii]);
//    sys_delay(480);
//  }
//  while(!FLASH_GetFlagStatus(FLASH_FLAG_EOP));
//  FLASH_Lock(FLASH_MemType_Data);  
//}
//
////--�����ݴ�EEPROM�ж�����
//void eeprom_read(uint8_t *p){
//  uint8_t ii;
//  for(ii=0;ii<SIZE_OF_STRU_EEPHEADER;ii++){
//   p[ii]=FLASH_ReadByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+ii);
//  }
//}


/*
void eep_wr_test(void){
  atad.m_addr=0x1122;
  atad.s_addr=0x3344;  
  eeprom_write((uint8_t *)&atad);  
}

void eep_re_test(void){
 // eeprom_write((uint8_t *)&atadt);  
  urt1SendStr((uint8_t *)&atadt, 4);
}
*/

void auto_pack_init(void){  
  atad.m_addr=0xffff;
  atad.s_addr=0x4567;  
}

//���ݰ�����
void send_auto_pack(uint16_t addr, uint8_t type_t){
  uint8_t* p;
  p = (uint8_t*)&atpt;
  uint8_t i;
  uint8_t j;
  
  //װ��atpt���Ͱ�����atpt���Ƶ�abf.buff
  switch(atv.type_t){
    case 0x40://���Ͳ������ݵ�atpt��,ǿ�����ӻ�Ӧ;
      atpt.m_addr = atad.m_addr;//װ�����ӵ�ַ��
      atpt.s_addr = atad.s_addr;
      atpt.type = atv.type_t;//��Ӧǿ������ָ��;
      atpt.struct_type = 0xff;//struct_type��Ϊ0xff;
      atpt.crc = 0x00;//װ��crc��
      //װ�ط��ͻ�����
      abf.size = 8;
      for(i=0 ; i<6 ; i++){
        abf.buff[i] = p[i];
      }
      abf.buff[6] = 0x00;
      abf.buff[7] = 0x00;
      SX1278LoRaSetTxPacket((void*)abf.buff , abf.size);
      urt1SendStr(abf.buff , abf.size);
      break;
      
    case 0x41://���Ͳ������ݵ�atpt��������������Ӧ;
      atpt.m_addr = atpr.m_addr;//װ�����ӵ�ַ��
      atpt.s_addr = atad.s_addr;
      atpt.type = atv.type_t;//��Ӧ��������ָ��;
      atpt.struct_type = 0xff;//struct_type��Ϊ0xff;
      atpt.crc = 0x00;//װ��crc��
      
      abf.size = 8;
      for(i=0 ; i<6 ; i++){
        abf.buff[i] = p[i];
      }
      urt1SendChar(abf.buff[4]);
      abf.buff[6] = 0x00;
      abf.buff[7] = 0x00;
      SX1278LoRaSetTxPacket((void*)abf.buff , abf.size);
      urt1SendStr(abf.buff , abf.size);
      break;
      
    case 0x43: //ǿ�ƶϿ�
      atpt.m_addr = atpr.m_addr;//װ�����ӵ�ַ��
      atpt.s_addr = atad.s_addr;
      atpt.type = atv.type_t;//��Ӧ��������ָ��;
      atpt.struct_type = 0xff;//struct_type��Ϊ0xff;
      atpt.crc = 0x00;//װ��crc��
      
      abf.size = 8;
      for(i=0 ; i<6 ; i++){
        abf.buff[i] = p[i];
      }
      urt1SendChar(abf.buff[4]);
      abf.buff[6] = 0x00;
      abf.buff[7] = 0x00;
      SX1278LoRaSetTxPacket((void*)abf.buff , abf.size);
      urt1SendStr(abf.buff , abf.size);
      break;
    
    case 0x44://Ƶ�����÷���
      atpt.m_addr = atpr.m_addr;//װ�����ӵ�ַ��
      atpt.s_addr = atad.s_addr;
      atpt.type = atv.type_t;//��Ӧ��������ָ��;
      atpt.struct_type = 0xff;//struct_type��Ϊ0xff;
      atpt.crc = 0x00;//װ��crc��
      
      abf.size = 9;
      for(i=0 ; i<6 ; i++){
        abf.buff[i] = p[i];
      }
      urt1SendChar(abf.buff[4]);
      abf.buff[6] = frequency_val.num;
      abf.buff[7] = 0x00;
      abf.buff[8] = 0x00;
      
      SX1278LoRaSetTxPacket((void*)abf.buff , abf.size);
      urt1SendStr(abf.buff , abf.size);
      break;
      
    case 0x42://���ʹ����ݵ�atpt��������ͨ�Ż�Ӧ
      if(atv.struct_type != 0x11){
      //if(atv.data_flag){
      atpt.m_addr = atad.m_addr;//װ�����ӵ�ַ��
      atpt.s_addr = atad.s_addr;
      atpt.type = atv.type_t;//��Ӧ����ͨ��ָ��;
      atpt.struct_type = atv.struct_type;//struct_type��Ϊatv.struct_type;
      atpt.data_p = (uint8_t*) &data_t; //ָ��ָ���ͻ�����
      atpt.crc = 0x00;//װ��crc��

      abf.size = data_t.data_size + 8;
      for(i=0 ; i<6 ; i++){
        abf.buff[i] = p[i];
      }
      
      ////װ������λ��buff
      for(j=0 ; j<data_t.data_size ; j++){
        abf.buff[j+6] = atpt.data_p[j+1];
      }
      data_t.data_size = 0;
      
      abf.buff[abf.size-2] = 0x00;
      abf.buff[abf.size-1] = 0x00;
          SX1278LoRaSetTxPacket((void*)abf.buff , abf.size);
      urt1SendStr(abf.buff , abf.size);
      break;
      }
      else{
        atpt.m_addr = 0xffff;//װ�����ӵ�ַ��
        atpt.s_addr = 0xffff;
        atpt.type = atv.type_t;//��Ӧǿ������ָ��;
        atpt.struct_type = 0xff;//struct_type��Ϊ0xff;
        atpt.crc = 0x00;//װ��crc��
        //װ�ط��ͻ�����
        abf.size = 8;
        for(i=0 ; i<6 ; i++){
          abf.buff[i] = p[i];
        }
        abf.buff[6] = 0x00;
        abf.buff[7] = 0x00;
        SX1278LoRaSetTxPacket((void*)abf.buff , abf.size);
        urt1SendStr(abf.buff , abf.size);
        break;
      }
      //else{
      //  SX1278LoRaSetTxPacket((void*)test.buff , 32);
      //} 
  }
  abf.size = 0;
  atv.data_flag = 0;
}

uint8_t strength_data[4];
//���ݰ�����
void auto_pack_process(void){
  uint8_t *p;
  uint8_t i;
  abf.size = 0;
  SX1278LoRaGetRxPacket((void *)abf.buff, &abf.size);
  //SX1278LoRaGetRxPacket((void *)abf.buff, &abf.size);
  //urt1SendChar(abf.size);
  if(abf.size <= 0) return; 
  //urt1SendStr(abf.buff , abf.size);
  
  //SX1278Read(0x1A, strength_data);
  //urt1SendStr(strength_data , 4);
  
 //atpr����buff�е�����
  p = (uint8_t*)&atpr;
  atpr.data_p = (uint8_t*)&data_r;
  data_r.data_size = abf.size - 8;
  p[0] = abf.buff[0];
  p[1] = abf.buff[1];
  p[2] = abf.buff[2];
  p[3] = abf.buff[3];
  p[4] = abf.buff[4];
  p[5] = abf.buff[5];
  for(i=0 ; i<data_r.data_size ; i++){
    atpr.data_p[i+1] = abf.buff[6+i];
  }
  //p[abf.size-1] = 0x00;
  
  
  //atv��atpr��ȡ��Ҫ����Ϣ
  atv.pack_size = abf.size;
  //urt1SendChar(atv.pack_size);
  //while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
  atv.type_r = atpr.type;
  //urt1SendChar(atv.type_r);
  //while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
  atv.struct_type = atpr.struct_type;
  //urt1SendChar(atv.struct_type);
  //while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
  
  //����atpr.type
  abf.size = 0;
  
  switch(atv.type_r){
  case 0xC0://ǿ������
    if( atad.s_addr == atpr.s_addr){
      atad.m_addr = atpr.m_addr;
      atv.type_t = 0x40;
      send_auto_pack(atad.m_addr, atv.type_t);
    }
    break;
    
  case 0xC1://����Ҫ��ͨ��
    if(atad.s_addr == atpr.s_addr){
      if(atad.m_addr != atpr.m_addr){ //��������
        atv.type_t = 0x41;
        send_auto_pack(atpr.m_addr, atv.type_t);
      }
      else{ //��������
        recv_data_deliver((uint8_t*)data_r.data , &data_r.data_size , atv.struct_type);
        atv.type_t = 0x42;
      }
    }
    if(atpr.s_addr == 0xffff){
      recv_data_deliver((uint8_t*)data_r.data , &data_r.data_size , atv.struct_type);
      atv.type_t = 0x42;
    }
    break;
    
  case 0xC2://ǿ�ƶϿ�
    if(atad.s_addr == atpr.s_addr){
      atad.m_addr = 0xffff;
      atv.type_t = 0x43;
      send_auto_pack(atpr.m_addr, atv.type_t);
    }
    break;
    
  case 0xC4://Ƶ������
    if((atpr.s_addr == 0xffff)&&(atad.m_addr == atpr.m_addr)){
      atv.type_t = 0x44;
      frequency_val.num = atpr.data_p[1];
      frequency_val.flag = 1;
      //SX1278LoRaSetRFFrequency_reset(frequency_val.num);
      send_auto_pack(atpr.m_addr, atv.type_t);
      //send_auto_pack(atpr.m_addr, atv.type_t);
    }
    break;
    
  }
}

//////////////////////////////////////////////Ƶ���޸Ļ���///////////////////////////////////////////////////////////////////////////
//����numΪ1��ʱ�򣬶�Ӧfrequency_list[0]
const uint32_t frequency_list[116] = {
0x67C000  , 
0x680000  , 
0x684000  , 
0x688000  , 
0x68C000  , 
0x690000  , 
0x694000  , 
0x698000  ,
0x69C000  ,
0x6A0000  ,
0x6A4000  ,
0x6A8000  ,
0x6AC000  ,
0x6B0000  ,
0x6B4000  ,
0x6B8000  ,
0x6BC000  ,
0x6C0000  ,
0x6C4000  ,
0x6C8000  ,
0x6CC000  ,
0x6D0000  ,
0x6D4000  ,
0x6D8000  ,
0x6DC000  ,
0x6E0000  ,
0x6E4000  ,
0x6E8000  ,
0x6EC000  ,
0x6F0000  ,
0x6F4000  ,
0x6F8000  ,
0x6FC000  ,
0x700000  ,
0x704000  ,
0x708000  ,
0x70C000  ,
0x710000  ,
0x714000  ,
0x718000  ,
0x71C000  ,
0x720000  ,
0x724000  ,
0x728000  ,
0x72C000  ,
0x730000  ,
0x734000  ,
0x738000  ,
0x73C000  ,
0x740000  ,
0x744000  ,
0x748000  ,
0x74C000  ,
0x750000  ,
0x754000  ,
0x758000  ,
0x75C000  ,
0x760000  ,
0x764000  ,
0x768000  ,
0x76C000  ,
0x770000  ,
0x774000  ,
0x778000  ,
0x77C000  ,
0x780000  ,
0x784000  ,
0x788000  ,
0x78C000  ,
0x790000  ,
0x794000  ,
0x798000  ,
0x79C000  ,
0x7A0000  ,
0x7A4000  ,
0x7A8000  ,
0x7AC000  ,
0x7B0000  ,
0x7B4000  ,
0x7B8000  ,
0x7BC000  ,
0x7C0000  ,
0x7C4000  ,
0x7C8000  ,
0x7CC000  ,
0x7D0000  ,
0x7D4000  ,
0x7D8000  ,
0x7DC000  ,
0x7E0000  ,
0x7E4000  ,
0x7E8000  ,
0x7EC000  ,
0x7F0000  ,
0x7F4000  ,
0x7F8000  ,
0x7FC000  ,
0x800000  ,
0x804000  ,
0x808000  ,
0x80C000  ,
0x810000  ,
0x814000  ,
0x818000  ,
0x81C000  ,
0x820000  ,
0x824000  ,
0x828000  ,
0x82C000  ,
0x830000  };


void SX1278LoRaSetRFFrequency_reset(uint8_t num)
{ //������ӦƵ����ֵ
    uint32_t frequency_target =  frequency_list[num-1];
  //��������λ�ý������޸�
    frequency_val.low = (uint8_t)(frequency_target & 0x000000ff);
    frequency_val.med = (uint8_t)((frequency_target & 0x0000ff00)>>8);
    frequency_val.high = (uint8_t)((frequency_target & 0x00ff0000)>>16);
    SX1278Write(REG_LR_FRFMSB,frequency_val.high);
    SX1278Write(REG_LR_FRFMID,frequency_val.med);
    SX1278Write(REG_LR_FRFLSB,frequency_val.low);            
}

/*
void send_auto_pack(uint16_t addr, uint8_t type)
{
  atpt.m_addr=addr;
  atpt.s_addr=atad.s_addr;
  atpt.type=type;
  atpt.crc=0x0000;
  SX1278LoRaSetTxPacket(&atpt,7);
}

//�Զ����մ������ݰ�״̬��
void auto_pack_process(void){
  uint8_t ii; 
  uint8_t *p;
  //������յ������ݰ�
  
  SX1278LoRaGetRxPacket((void *)abf.buff, &abf.size);
  if(abf.size>0){ 
    //urt1SendChar(0xBB);
      p=(uint8_t *)&atpr;
      if((abf.buff[4] & 0xC0)==0xC0){//���������ģ����ڽ����Զ����ӵ����ݰ���
         for(ii=0;ii<7;ii++) p[ii]=abf.buff[ii];
         atv.type=atpr.type;
      }
      //urt1SendStr(abf.buff,abf.size); 
  }
  //
  switch(atv.type){
  case 0xC0://���յ���վѰ������
    if(atad.m_addr==0xffff || atad.m_addr==0x0000){//��վ����������Լ���
        atadt.m_addr=atpr.m_addr;           //��������ʱ���
        send_auto_pack(atpr.m_addr, 0x40);  //Ȼ�󷢳�Ӧ��Ѱ��
    }
    urt1SendChar(0xC0);
    atv.type=0x00;
    break;
  case 0xC1://���յ���վǿ������ָ��
    if(atad.s_addr==atpr.s_addr){//ֻ��֪����վ�ĵ�ַ������ǿ�Ƹı���
      atad.m_addr=atpr.m_addr;
      //eeprom_write((uint8_t *)&atad);//��atad,������EEPROM��
    }
    urt1SendChar(0xc1);
    atv.type=0x00;
    break;
  case 0xC2://���յ���վ�Ͽ�ָ��
     if(atad.s_addr==atpr.s_addr){//ֻ��֪����վ�ĵ�ַ������ǿ�Ƹı���
        atadt.m_addr=0xffff;           //��������ʱ���      
        //eeprom_write((uint8_t *)&atad);//��atad,������EEPROM��
    }
    urt1SendChar(0xc2);
    atv.type=0x00;
    break;    
  
  case 0xC3://�ӵ���վӦ��ָ��
    if(atad.m_addr==0xffff || atad.m_addr==0x0000){
      if(atadt.m_addr==atpr.m_addr){
        atad.m_addr=atadt.m_addr;   atadt.m_addr=0xffff;
        //eeprom_write((uint8_t *)&atad);//��atad,������EEPROM��
      }
    } 
    urt1SendChar(0xc3);
    atv.type=0x00;
    break; 
    
    case 0xC4://�ӵ���վӦ��ָ��
      if((atadt.m_addr==atpr.m_addr) && (atadt.s_addr==atpr.s_addr)){        
        send_auto_pack(atpr.m_addr, 0x44);  //Ȼ�󷢳�Ӧ��Ѱ��
      }
    urt1SendChar(0xc4);  
    atv.type=0x00;
    break; 
  }
}
*/