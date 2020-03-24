/*BOARDC*/
#include "board.h"
// flash led
STRUCT_SYS s; 
STRUCT_EEP eep;			

//�弶��������߼�������ֻҪ���������Զ�е����ݣ���ʹ����Щ����
uint8_t stflash;
uint32_t atime,btime,delaytime;
uint32_t sys_hal_tick_time,li_tick_time,li_100ms_time;
//extern WWDG_HandleTypeDef hwwdg;
//ϵͳ��¼ʱ����ʹ��

STRU_uart urt1;
STRU_uart urt2;

uint8_t sys_ver[] = SX_MAIN_VER;
uint8_t vch[16];

//ͼ����ʾ���ݽṹ
extern IWDG_HandleTypeDef hiwdg;
extern UART_HandleTypeDef huart1;  //PCͨ��
extern UART_HandleTypeDef huart2;  //Nbiot
extern uint8_t Rx1Buffer[1];
extern uint8_t Rx2Buffer[1];
unsigned char picStrArry[20];
unsigned char tsnb[]={'A','T',0x0D,0x0A};

//���߽������ݽṹ
//run only in this c file
void li_100ms_ticks(void);
void li_start(void){
	HAL_UART_Receive_IT(&huart1,Rx1Buffer,1);//������һ�ν����ж�
	HAL_Delay(1);
	HAL_UART_Receive_IT(&huart2,Rx2Buffer,1);//������һ�ν����ж�
	if(s.reset_flag!= 0xA5A5A5A5){
		s.reset_flag = 0xA5A5A5A5;		
	}
	SX_RESET_1;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	NB_RESET_0;
	s.start_step=0;
	connect_init();
}

void li_ticks(void){
	//����ִ�еĳ����������
	th_uart1Manage();//���������֤���յ�����֮������������
	th_uart2Manage();
	SX1278LoRaProcess();
	connect_process();
	BC28_data_pack();
	broad_send_model();
	//end����ִ�г���
	//connect_10m_call();
	//fresh_struct_data();
	fast_fresh();
	//slow_fresh();
	
	sys_hal_tick_time=HAL_GetTick();	//��ȡʱ�������
	if(li_tick_time==sys_hal_tick_time) return;	
	//HAL_WWDG_Refresh(&hwwdg);
	s.powerdown_time++;
	li_tick_time=sys_hal_tick_time;			
	if(li_100ms_time<sys_hal_tick_time){//100ms ִ��һ��
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
void li_100ms_ticks(void){//100ms ִ��һ��	
	if(s.ledflash>0){LED_WORK_ON; s.ledflash--;} else LED_WORK_OFF; 
	s.setp1++;
	HAL_IWDG_Refresh(&hiwdg);
	switch(s.setp1){
		case 2: //ÿ��5�룬����û�д�������о��죬 1����ټ��
			break;
		case 3:
			if(s.ledflash < 1) s.ledflash = 1;
			HAL_UART_Receive_IT(&huart1,Rx1Buffer,1);//������һ�ν����ж�
			HAL_UART_Receive_IT(&huart2,Rx2Buffer,1);//������һ�ν����ж�
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

//����FM25L16-G�ĺ���

// SX1278ͨ�Ų���----------------------------------------------
