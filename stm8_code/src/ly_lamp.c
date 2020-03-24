//
//ly_lamp.c
//

/*
#include "stm8l15x.h"
#include "sx_board.h"

uint8_t ly_state;
extern uint8_t recv[32];
extern uint8_t recvNum;

void ly_process(void){//所有上层协议处理状态机
  switch(ly_state){
  case 0://通常情况下，接收到信息以后，再进处理
    if(recvNum>5){//说明接收到的是正常数据
      switch(recv[0]){
      case 0x80:
        break;
      }
      
    }
    break;
    
    
    
    
  }
}
*/