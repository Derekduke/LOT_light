/*BOARDC*/
#include "board.h"
// flash led
STRUCT_SYS s; 
STRUCT_EEP eep;			

//板级变量，最高级变量，只要是这个板上远行的内容，都使用这些变量
uint8_t stflash;
uint32_t atime,btime,delaytime;
uint32_t sys_hal_tick_time,li_tick_time,li_100ms_time;
//extern WWDG_HandleTypeDef hwwdg;
//系统记录时间所使用

STRU_uart urt1;
STRU_uart urt2;

uint8_t sys_ver[] = SX_MAIN_VER;
uint8_t vch[16];

//图形显示数据结构
extern IWDG_HandleTypeDef hiwdg;
extern UART_HandleTypeDef huart1;  //PC通信
extern UART_HandleTypeDef huart2;  //Nbiot
extern uint8_t Rx1Buffer[1];
extern uint8_t Rx2Buffer[1];
unsigned char picStrArry[20];
unsigned char tsnb[]={'A','T',0x0D,0x0A};

//无线接收数据结构
//run only in this c file
void li_100ms_ticks(void);
void li_start(void){
	HAL_UART_Receive_IT(&huart1,Rx1Buffer,1);//开启下一次接收中断
	HAL_Delay(1);
	HAL_UART_Receive_IT(&huart2,Rx2Buffer,1);//开启下一次接收中断
	if(s.reset_flag!= 0xA5A5A5A5){
		s.reset_flag = 0xA5A5A5A5;		
	}
	SX_RESET_1;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	NB_RESET_0;
	s.start_step=0;
	connect_init();
}

void li_ticks(void){
	//快速执行的程序放在这里
	th_uart1Manage();//放在这里，保证接收到数据之后能立即处理
	th_uart2Manage();
	SX1278LoRaProcess();
	connect_process();
	BC28_data_pack();
	broad_send_model();
	//end快速执行程序
	//connect_10m_call();
	//fresh_struct_data();
	fast_fresh();
	//slow_fresh();
	
	sys_hal_tick_time=HAL_GetTick();	//获取时间戳函数
	if(li_tick_time==sys_hal_tick_time) return;	
	//HAL_WWDG_Refresh(&hwwdg);
	s.powerdown_time++;
	li_tick_time=sys_hal_tick_time;			
	if(li_100ms_time<sys_hal_tick_time){//100ms 执行一次
		li_100ms_time=sys_hal_tick_time+100;
		li_100ms_ticks();
	}	
	s.setp0++;
	switch(s.setp0){
		case 0:	
			th_uart1Manage();				
			th_uart2Manage();
			BC28_data_pack();
			break;
		case 1:			
			break;
		case 3:
		
			break;
		default:
			s.setp0 = 255;
			break;
	}
}
void li_100ms_ticks(void){//100ms 执行一次	
	if(s.ledflash>0){LED_WORK_ON; s.ledflash--;} else LED_WORK_OFF; 
	s.setp1++;
	HAL_IWDG_Refresh(&hiwdg);
	switch(s.setp1){
		case 2: //每隔5秒，看有没有错误，如果有就响， 1秒后再检查
			break;
		case 3:
			if(s.ledflash < 1) s.ledflash = 1;
			HAL_UART_Receive_IT(&huart1,Rx1Buffer,1);//开启下一次接收中断
			HAL_UART_Receive_IT(&huart2,Rx2Buffer,1);//开启下一次接收中断
			break;
		case 6:
//			connect_10m_call();
			break;
		case 7:			
			break;
		case 9:
			NB_RESET_1;
		  if(s.start_step<100) s.start_step++;
			break;
		case 10:
			//urt2SendStr(tsnb, 4);
			s.setp1++;
			break;	
		default:			
			if(s.setp1 > 10) s.setp1 = 0;
			break;
	}
}


void Delayms(unsigned int myms){
	delaytime=HAL_GetTick();
	while(HAL_GetTick()-delaytime<myms);
}
void li_start_steps(void){
	switch(s.start_step){
		case 5:
			if(SX1278Init()) s.start_step=6;
			break;
		case 6:
			break;		
		case 7:
			SX1278LoRaStartRx();
			break;
		
		
		
	}
}

//测试FM25L16-G的函数

// SX1278通信部分----------------------------------------------
