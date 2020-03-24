
#ifndef __SX_UART_H
#define __SX_UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sx_board.h"

/* ·�ƻظ���������Ϣ�ṹ��---------------------------------------------------*/
////(1)û�лظ�����

/////�ǹ㲥����ʽ����LF145C1����(2)
typedef struct{
  uint8_t  brightness_value;//���Ȱٷֱ�
}brightness;

/////////ʱ�����Ϣ,(2)������ʱ����Ϣ
typedef struct{
  
  uint32_t start_time;//������ʼʱ���
  uint32_t end_time;//���ƽ���ʱ���
  
}Brightness_level;

/////////����LF145C1ָ�3��
typedef struct{
  uint16_t light_vol;//�����ѹ
  uint16_t battery_vol;//��ص�ѹ
  uint8_t current_front;//��ŵ�������
  uint8_t current_value;//��ŵ���ֵ
  uint16_t led_vol;//LED��Դ���ѹ
  uint16_t temple;//�弶�¶�
}Single_LF145C1_pama;

/////////��ֵ��4,5��
typedef struct{
  uint16_t charge_current_max;//����������ֵ��10
  uint16_t discharge_current_max;//�ŵ��������ֵ��10
  uint16_t battery_value_max;//������ѹ�����ֵ
  uint16_t battery_value_min;//��ذ���ѹ����Сֵ
  uint8_t temple_max;//�弶�¶����ֵ
  uint8_t temple_flag_max;//�¶ȷ���λ���ֵ
  uint8_t temple_min;//�弶�¶���Сֵ
  uint8_t temple_flag_min;//�¶ȷ���λ��Сֵ
  uint16_t charge_AH;//���AH��
  uint16_t discharge_AH;//�ŵ�AH��
}Max_min_value;

/////////��ʷ���ݣ�6��
typedef struct{
  uint8_t related_history;//�������
  uint8_t battery_value_max_history;//������ѹ�����ֵ
  uint16_t charge_AH_history;//���AH��
  uint16_t discharge_AH_history;//�ŵ�AH��
}History_someday;

/////////����״̬��7��
typedef struct{
  uint8_t  step;//�����е�step����
  uint8_t  dayornight;//�����ҹ�ı�־��1�ǰ��죬2�Ǻ�ҹ
  uint8_t  count;//���ǳ����е�nig_ct��������ת����ҹ����ҹת������ļ���λ����ֵ��100���ڣ�
  uint32_t  time_day;//����ʱ�䣨��λ���룩
  uint32_t  time_night;//ҹ��ʱ�䣨��λ���룩
  uint32_t  charge_AS_day;//��������AS��������36000��λ����AH��
  uint32_t  discharge_AS_night;//ҹ��ų���AS��(����36000��λ����AH)
  
}State_thisday;

/////////���Ʋ��ԣ�8��
typedef struct{
        uint8_t  time1;//��һ�׶�ʱ��
        uint8_t  level1;//��ʼ����ռ��������ȵİٷֱ�
        uint8_t  speed1;//˥��������
        uint8_t  time2;//�ڶ��׶�ʱ��
        uint8_t  level2;//��ʼ����ռ��������ȵİٷֱ�
        uint8_t  speed2;//˥��������   
        uint8_t  time3;//�����׶�ʱ��
        uint8_t  level3;//��ʼ����ռ��������ȵİٷֱ�
        uint8_t  speed3;//˥��������   
        uint8_t  time4;//���Ľ׶�ʱ��
        uint8_t  level4;//��ʼ����ռ��������ȵİٷֱ�
        uint8_t  speed4;//˥��������   
        uint8_t  time5;//����׶�ʱ��
        uint8_t  level5;//��ʼ����ռ��������ȵİٷֱ�
        uint8_t  speed5;//˥��������   
}URT_auto_stratage;

/////////���Ʋ������ã�9��
//uart1Manage case9:

/////////·�Ƶ�ַ��10��
typedef struct{
  uint8_t addr_h;//���ص�ַ��λ
  uint8_t addr_l;//���ص�ַ��λ
}llf145c1_addr;

/////////��ص�ѹ��11,12��
typedef struct{
  uint8_t sunctrl_level; //��ص�ѹ�ȼ�
}sun_control;

////////��ȡ�ͻ���Ϣ��13 ,14��
typedef struct{
    uint8_t save_byte;//�����ֽ�
    uint8_t byte1;//��һ�ֽ�
    uint8_t byte2;//�ڶ��ֽ�
    uint8_t byte3;//�����ֽ�
    uint8_t byte4;//�����ֽ�
    uint8_t byte5;//�����ֽ�
}client_infor;

////////�ṹ������
typedef struct{
  uint8_t struct_type;//�ṹ������
  uint8_t* struct_p; //�ṹ����λָ��
  uint8_t struct_size; //�ṹ���С
}data_manage;


void UART1_Init(unsigned int baudrate);
void It_Get_Byte(void);
void uart1Manage(void);
void urt1RM06(void);
void changeSettings(void);
void urt1SendChar(uint8_t a);
void urt1SendStr(uint8_t *Str,uint8_t size);
////////////////////����ʹ��
void fun_for_debug(void);

/*·�Ʒ��͵�������Ϣ�ṹ��---------------------------------------------------*/
////�㲥����ʽ��������LF145C1���ȣ�1����2��
typedef struct{
  //uint8_t all; //all = 1Ϊ�ǹ㲥 �� all = 0Ϊ�㲥
  uint16_t brightness_time;
  uint8_t brightness_percent;
  
}brightness_set;

////��ȡ����LF145C1����ָ�3��
//0x03ָ��;

////��ȡ������ֵָ�4��
//0x04ָ��;

////��ȡǰһ����ֵָ�5��
//0x05ָ��;

////��ȡĳһ�����ʷ���ݣ�6��
typedef struct{
  uint8_t related_days;
}history_data;

////��ȡ����״̬��Ϣ��7��
//0x07ָ�

////���Ʋ��Զ�ȡ��8��
//0x0Cָ�

////���Ʋ������ã�9��
typedef struct{
  uint8_t  time1;//��һ�׶�ʱ��
  uint8_t  level1;//��ʼ����ռ��������ȵİٷֱ�
  uint8_t  speed1;//˥��������
  uint8_t  time2;//�ڶ��׶�ʱ��
  uint8_t  level2;//��ʼ����ռ��������ȵİٷֱ�
  uint8_t  speed2;//˥��������   
  uint8_t  time3;//�����׶�ʱ��
  uint8_t  level3;//��ʼ����ռ��������ȵİٷֱ�
  uint8_t  speed3;//˥��������   
  uint8_t  time4;//���Ľ׶�ʱ��
  uint8_t  level4;//��ʼ����ռ��������ȵİٷֱ�
  uint8_t  speed4;//˥��������   
  uint8_t  time5;//����׶�ʱ��
  uint8_t  level5;//��ʼ����ռ��������ȵİٷֱ�
  uint8_t  speed5;//˥��������   
}URT_auto_stratage_set;

////��ȡ��ַ����(10)
//0x09��0x02��0x03ָ��;

////���ù�ص�ѹ���11��
typedef struct{
  uint8_t sunctrl_level;
}sun_set;

////��ѯ��ص�ѹ���12��
//0x1Eָ�

////��ȡ�ͻ���Ϣ��13��
//0x06ָ��;

////д�ͻ���Ϣ��14��
typedef struct{
  //uint8_t save_byte;//�����ֽ�
  uint8_t byte1;//��һ�ֽ�
  uint8_t byte2;//�ڶ��ֽ�
  uint8_t byte3;//�����ֽ�
  uint8_t byte4;//�����ֽ�
  uint8_t byte5;//�����ֽ�
}get_client_infor;

//��������Ϊ��·�ư�LF145C1ͨ�ŵ�ָ��1��ָ��14�ķ��ͺ���

///////////////
//��ȡ����ָ��
void Read_LF145C1_par(uint8_t type);
//////////////
//��ѡ����
#define SINGLE_LF145C1_SEND 0x03   //��ȡ����LF145C1ָ�ָ��3��//���ڶ�ȡ
#define THISDAY_MAXMIN_SEND    0x04   //��ȡ������ֵ��ָ��4��//���ڶ�ȡ
#define LASTDAY_MAXMIN_SEND  0X05   //��ȡǰһ����ֵ��ָ��5��//���ڶ�ȡ
#define CLIENT_INFOR_SEND   0x06   //�ͻ���Ϣ��ȡ��ָ��13��
#define THISDAY_STATE_SEND  0X07   //��ȡ����״̬��Ϣ��ָ��7��//���ڶ�ȡ
#define LIGHTING_STRA_SEND  0x0C   //���Ʋ��Զ�ȡ��ָ��8�����ڶ�ȡ

/////////////////��ָ��9��
//���Ʋ�������
void Light_Stra_Setting(void);
///////////////��ָ��1��2��
//��������,all�����Ƿ�㲥
void Brightness_Setting(uint8_t percent,uint16_t time,uint8_t all);
/////////////(ָ��11)
//���ù�ص�ѹ��levelΪ�ȼ�
void SunCtrl_Setting(uint8_t level);
/////////////��ָ��12��//���ڶ�ȡ
//��ѯ��ص�ѹ
void SunCtrl_RQS(void);
//////////////��ָ��6��//���ڶ�ȡ
//��ȡ֮ǰĳһ����ʷ����
void History_RQS(uint8_t days);
///////////////��ָ��10��
//��ȡ��ַ����������Ϳ�ʹ��0x09,0x02,0x03��
void Address_RQS(uint8_t struct_type);
///////////////��ָ��14��
//д�ͻ���Ϣ
void Client_Infor_Write(uint8_t *infor);


////�ϲ�ȡ�ýṹ������
void send_data_deliver(uint8_t* buffer , uint8_t* data_size_p_t , uint8_t struct_type);
void recv_data_deliver(uint8_t* buffer , uint8_t* data_size_p_r , uint8_t struct_type);



/************************end **************************************************/
#endif