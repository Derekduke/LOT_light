
//stm32f030c8t6  64k flash  8k sram

#ifndef __board_h
#define __board_h

#include "main.h"
#include "FM25VN10.h"
#include "stdbool.h"
#include "sx1278.h"
#include "sx1278-LoRaMisc.h"
#include "sx1278-Hal.h"
#include "sx1278-LoRa.h"
#include "bc28.h"
#include "connect.h"
//��������汾�ţ�"VER: 00.1902.00" ��ʽ���ܸı�
#define SX_MAIN_VER "VER: 00.1902.00"


//��ɫ��������ܣ���PWM����
#define LED_WORK_ON  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET)
#define LED_WORK_OFF 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET)

#define SPI1_CS_1				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define SPI1_CS_0				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define SPI2_CS_1				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define SPI2_CS_0				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define SX_RESET_0			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define SX_RESET_1			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define NB_RESET_0			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET)
#define NB_RESET_1			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET)



//READ LORA STATE
#define DIOD_0_READ 			HAL_GPIO_ReadPin(DIOD0_GPIO_Port, DIOD0_Pin)   
#define DIOD_1_READ 			HAL_GPIO_ReadPin(DIOD1_GPIO_Port, DIOD1_Pin)   
#define DIOD_2_READ 			HAL_GPIO_ReadPin(DIOD2_GPIO_Port, DIOD2_Pin)   
#define DIOD_3_READ 			HAL_GPIO_ReadPin(DIOD3_GPIO_Port, DIOD3_Pin)   
#define DIOD_4_READ 			HAL_GPIO_ReadPin(DIOD4_GPIO_Port, DIOD4_Pin)   



typedef struct{
		uint32_t reset_flag;  //����ʱ�ı�־		
	  uint32_t powerdown_time;	
		uint8_t recvNew;
	  uint8_t setp0;
	  uint8_t setp1;
	  uint8_t start_step;
	  uint8_t pwStep;  	//ר�ż�¼�ص�Դ��
	  uint8_t ledflash;
	  uint8_t iwdg_new;	  
	  uint8_t err;
}STRUCT_SYS;


typedef struct{
	uint8_t t;
}STRUCT_EEP;

union LONG_BYTE{
	unsigned char b[4];
	unsigned int  i[2];	
	unsigned long L4;
};

//--------------------------------02���ڴ�����ؽṹ��--------------------------------
//���ڽ������ݻ�����
typedef struct{
		uint32_t timenow;  //��ʱ
		uint32_t timelast;
		uint8_t aTxb[64];		//���ͻ�����
		uint8_t Rxb[256];
	  uint8_t cmd[256];
		uint8_t step;
		uint8_t RxNew;
		uint8_t RxCount;			//UART5������
		uint32_t header; 				//
		uint8_t RxNum;    		//ȫ�����պ���ֽ�		
}STRU_uart;

//���պ��������:�ѽ��պ���������ͳ��������������ṹ���У� �Ա�����Ӧ��ִ��
typedef struct{
		uint8_t type;  //������
		uint8_t num;	 //������
		uint8_t crc;	 //������������128���ֽ��У� �ж��ٸ�����Ч��
		uint8_t flag;  //��־λ��������ǰ�������Ƿ���Ч		
		uint8_t Rxb[64];     //���ջ�����
}STRU_ur_data;




typedef union{
	struct{
		unsigned char b3;
		unsigned char b2;
		unsigned char b1;
		unsigned char b0;
	}b;
	struct{
		unsigned short  wh;
		unsigned short  wl;
	}w;
	unsigned long dw;
	unsigned char sd[4];
}dmpUNION;



extern STRUCT_SYS s;
extern uint32_t sys_hal_tick_time;
void li_start(void);
void li_ticks(void);
void urt1RecvCommandManage(void);
void urt1_HeaderTailSend(uint8_t *arr, uint8_t num);
void th_uart1Manage(void);
void th_uart2Manage(void);
void urt1SendStr(uint8_t *a, uint8_t num);
void urt1SendChar(uint8_t a);
void urt2SendStr(uint8_t *a, uint8_t num);
void urt2SendChar(uint8_t a);
void Delayms(unsigned int myms);
void keyManage(void);
void myinit(void);
void dispMessage(void);
void SendPack32Manage(void);


//in sx1278.c


//void RecvPack32Manage(void);

#endif


