//Ŀ��ʵ��ͨ�ŵ��Զ����ӹ���
#ifndef __SX_AUTOC_H
#define __SX_AUTOC_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sx_board.h"

#define MA_CALL_SON             0xC0    //��վ  ��1.Ѱ�ӡ� ��ʱs_addr = 0xFFFF
#define MA_CONNECT              0xC1    //��վ  ��2.ǿ�����ӡ�
#define MA_RESET                0xC2    //��վ  ��3.ǿ�ƶϿ���
#define MA_ANS                  0xC3    //��վ  ��4.��Ӧ��
#define MA_ASK_ADDR             0xC4    //��վ  ��4.Ѱַ�����ʱs_addr = 0xFFFF
#define SA_ANS_CALL             0x40    //��վ  ��5.Ӧ��Ѱ�ӡ�
#define SA_ASK_DISC             0x41    //��վ  ��6.Ȱ�롿
#define SA_ANS                  0x42    //��վ  ��7.��Ӧ��



//#define MA_CALL_SON             0xC0    //��վ  ��1.Ѱ�ӡ� ��ʱs_addr = 0xFFFF
//#define MA_CONNECT              0xC1    //��վ  ��2.ǿ�����ӡ�
//#define MA_RESET                0xC2    //��վ  ��3.ǿ�ƶϿ���
//#define MA_ANS                  0xC3    //��վ  ��4.��Ӧ��
//#define MA_ASK_ADDR             0xC4    //��վ  ��5.ѯ���Ƿ���ͨ��
//
//#define SA_ANS_CALL             0x40    //��վ  ��7.Ӧ��Ѱ�ӡ�
//#define SA_ASK_DISC             0x41    //��վ  ��8.Ȱ�롿
//#define SA_ANS                  0x43    //��վ  ��9.��Ӧ��
//#define SA_ANS_ASK              0x44    //��վ  ��10.��ѯ��Ӧ���ص��ǽӵ������Ӧ��


//#define MA_RQS_NOW              0xA0    //��վ  ��1.��Ҫ��ǰ���ݡ�
//#define MA_RQS_M                0xA1    //��վ  ��2.��Ҫ������ֵ��
//#define MA_CFG_MODE             0xA2    //��վ  ��3.��������ģʽ��
//#define MA_CTRL_SET             0xA3    //��վ  ��4.���ƿ������á�
//#define                         0xA4    //��վ  ����
//
//#define SA_                     0x20    //��վ  ��1��
//#define SA_                     0x21    //��վ  ��1��
//#define SA_                     0x23    //��վ  ��1��
//#define SA_                     0x24    //��վ  ��1��

#define SIZE_OF_STRU_EEPHEADER  4
/*
typedef struct{
        uint16_t m_addr; //��վ�������ַ
        uint16_t s_addr; //��վ�������ַ
        uint8_t  type;   //0xC0 λΪ 1
        uint16_t crc;    //ǰ������������ϣ�ȡ��2�ֽ�
}STRU_auto_pack;

typedef struct{
        uint8_t  size;
        uint8_t  state;ta  //״̬
        uint8_t  type;   //��վ�������ַ
        uint8_t  pack_new;   //0xC0 λΪ 1
        uint16_t crc;    //ǰ������������ϣ�ȡ��2�ֽ�
}STRU_auto_ver;
*/

//���ݰ���ʽ
typedef struct{
	uint16_t m_addr; 	//��վ�������ַ
	uint16_t s_addr; 	//��վ�������ַ
	uint8_t  type;   	//���λΪ1�����ʾ��վ���������ݣ����λΪ0�����ʾ��վ����������
	uint8_t struct_type;//��Ӧ�����ݴ�Žṹ��ѡ��λ
	uint8_t  *data_p;	 	//��������ʹ��char*ָ��ָ�����ݻ�������p = (uint8_t*) &package_data��
	uint16_t crc;    	//ǰ������������ϣ�ȡ��2�ֽ�
}STRU_auto_pack;

//struct_type�淶����Խ·��Э���з���֡��typeλ��ͬ
//�磺 2�ķ�0x01 �� 2����0x81

//���ͽ������ݻ�����
typedef struct{
	uint8_t data_size; //(pack_size - 6)�����ݽṹ��������
	uint8_t data[24]; //p = package_data.data
}package_data;

//���ݰ���־��Ϣ��¼
typedef struct{
        uint8_t  pack_size;   //���ݰ���С����abf.size��ȡ��
        uint8_t  type_r;   //�������ݰ�ָ�����ͣ�atpr.type)
        uint8_t  type_t;   //�������ݰ�ָ������
        uint8_t  struct_type; //�ṹ�����ͣ�atpr.struct_type)
        uint8_t  data_flag; //���ݴ��������־λ
}STRU_auto_ver;

//��ַ��Ϣ�洢
typedef struct{
        uint16_t m_addr; //��վ�������ַ,Ϊ0xffff ��ʾû�е�ַ
        uint16_t s_addr; //��վվ�����ַ,
}STRU_eep_addr;

//lora�շ����ݻ�����
typedef struct{
  uint8_t size;
  uint8_t buff[32];
}STRU_auto_buff;

//Ƶ��ѡ���¼
typedef struct{
  uint8_t high;
  uint8_t med;
  uint8_t low;
  uint8_t num;
  uint8_t flag;
}frequency_control;


extern package_data data_r;//�������ݻ�����
extern package_data data_t;//�������ݻ�����
extern STRU_eep_addr  atad;
extern STRU_auto_buff abf;
extern STRU_auto_ver atv;
void auto_pack_process(void);
void send_auto_pack(uint16_t addr, uint8_t type_t);
void auto_pack_init(void);

void eep_wr_test(void);
void eep_re_test(void);

void SX1278LoRaSetRFFrequency_reset(uint8_t num);
/************************end **************************************************/
#endif