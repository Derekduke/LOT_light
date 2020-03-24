#ifndef __SX_BOARD_H
#define __SX_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sx_uart.h"
#include "sx_1278.h"
#include "sx_spi.h"
#include "ly_lamp.h"
#include "sx_autoC.h"




#define LED_ON          GPIO_ResetBits(GPIOB, GPIO_Pin_2)
#define LED_OF          GPIO_SetBits(GPIOB, GPIO_Pin_2)
#define LED_TOGGLE      GPIO_ToggleBits(GPIOB, GPIO_Pin_2)

typedef union{  
  _Bool rxNew:     1;
  _Bool txRunning: 1;
  _Bool bit2:    1;
  _Bool bit3:      1;
  _Bool bit4:   1;
  _Bool bit5:   1;
  _Bool bit6:   1;
  _Bool bit7:   1; 
}U_BIT_bod;

extern u8 spiNew;




extern U_BIT_bod bd;
extern u32 sys_time;
extern u8  sys_step;
extern u8  sys_start_step;


void sys_delay(u16 num);
void sys_wait(_Bool num);
void board_init(void);
void sx_quick_run(void);
void sx_ticks_run(void);
void sx_slow_run(void);
void sx_start(void);
#endif
/************************end **************************************************/
