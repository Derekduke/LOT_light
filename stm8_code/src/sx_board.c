#include "stm8l15x.h"
#include "sx_board.h"
#include "sx_autoC.h"

U_BIT_bod bd;
u32 sys_time;
u8  sys_step;
u8  led_num;
u8  sys_slow_step;
u8  sys_start_step;
u8  spiNew;
uint8_t lora_recv[32]; //无线接收到的数据放在这里
uint8_t recvNum;  //接收到的字节数放在这里
static uint8_t start = 0;

extern STRU_auto_buff test;
extern frequency_control frequency_val;
void board_init(void){
   GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
}
/*
  每1ms执行一次操作
*/
void sys_delay(u16 num){
  u16 ii;
  for(ii=0;ii<num;ii++);
}
void sys_wait(_Bool num){
  u16 ii;
  for(ii=0;ii<10000;ii++){
    if(num) ii=0xf000;
  }  
}
void sx_quick_run(void){
   SX1278LoRaProcess();
   auto_pack_process();
   uart1Manage();
   if(atv.type_t != 0x42) return;
   if(atv.data_flag == 0) SX1278LoRaSetTxPacket((void*)test.buff , 32);
   else{
   send_data_deliver((uint8_t*)data_t.data , &data_t.data_size , atv.struct_type);
   send_auto_pack(atad.m_addr, atv.type_t);
   atv.type_t = 0;
   }
}
void sx_ticks_run(void){
  switch(sys_step){
  case 0:
    break;
  case 1:
    break;
  case 8:
    if(led_num & 0x10) LED_ON; else LED_OF; 
    
    led_num++;
    sys_slow_step++;    
    sys_step++;
    break;
  case 16:

    break; 
  case 17:
     //SX1278LoRaGetRxPacket((void *)recv, &recvNum);     
     //urt1SendStr(r,LoRaSettings.lastRxPacketSize);
     sys_step=0;
    break;
  }
}

//
//void time_send(uint8_t time ,uint8_t type){
//    start = start + 1;
//    if(start == time){
//      Read_LF145C1_par(type);
//      start=0;
//    }
//    sys_slow_step++;
//}


void time_send(void){

//    if(start == time){
//      Read_LF145C1_par(type);
//      start=0;
//    }
  start = start + 1;
    switch(start){
    
    case 1:
      Read_LF145C1_par(SINGLE_LF145C1_SEND);//(3)
      break;
      
    case 2:
      Read_LF145C1_par(THISDAY_MAXMIN_SEND);//(4)
      break;
      
    case 3:
      Read_LF145C1_par(LASTDAY_MAXMIN_SEND);//(5)
      break;
            
    case 4:
      Read_LF145C1_par(THISDAY_STATE_SEND); //(7)
      break;
            
    case 5:
      Read_LF145C1_par(LIGHTING_STRA_SEND); //(8)
      break;
            
    case 6:
      SunCtrl_RQS();//(12)
      break;
            
    case 7:
      History_RQS(0);//(6)
      break;
      
    case 8:
      Read_LF145C1_par(CLIENT_INFOR_SEND); //(13)
      start = 0;
      break;
    }
    sys_slow_step++;
}

void sx_slow_run(void){
  switch(sys_slow_step){
  case 0:
    //SX1278Write(0x01, 0x00);
    //SX1278Write(0x01, 0x81);
    //USART_SendData8(USART1, 0x39);
    if(frequency_val.flag){
      SX1278LoRaSetRFFrequency_reset(frequency_val.num);
      frequency_val.flag = 0;
    }
    sys_slow_step++;
    break;
  case 1:
    //SX1278Read(0x01, &temp);
    // USART_SendData8(USART1, temp);
     //SX1278Reset();
    // USART_SendData8(USART1, 0xaa);   
     sys_slow_step++;
      sys_start_step++; sys_start_step &=0x7f;
    break;
  case 3:
    //urt1SendChar(temp_RFLRState);
    sx_start();
    sys_slow_step++;
    break;
  case 4:
//    for(temp=0;temp<16;temp++) t[temp]=0x30+temp;
//    SX1278LoRaSetTxPacket(t,16);
//    Read_LF145C1_par(THISDAY_MAXMIN_SEND);
//    sys_slow_step++;
//      time_send(2,SINGLE_LF145C1_SEND);
    time_send();
    //urt1SendChar(sizeof(State_thisday)); //结构体大小测试
    break;
    
  }
}
//开机以后，自动进入到启动状态  
void sx_start(void){
  switch(sys_start_step){   
  case 1: 
       //auto_pack_init();
       SX1278Init();
    break;    
  case 2:    
      SX1278LoRaStartRx();    
    break;  
  case 3:    
      SX1278Init();
    break;
  }  
}