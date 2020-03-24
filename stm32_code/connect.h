
/*!
 * \file       connect.h
 * \brief      SX1278 RF chip driver
 *
 * \version    2.0.B2
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "board.h"
//�ϴ���������ʽ
typedef struct{
	uint16_t at_size;
	char at_buff[50];
}mas_buff;
//BC28�·���
typedef struct{
	uint8_t bc_size;
	uint8_t bc_type;
	uint8_t bc_data[20];
}bc_pack;

//�������·�ָ��
typedef struct{
	uint8_t subnum; //Ŀ���ӻ���
	uint8_t type;
	uint8_t struct_type;
	uint8_t* master_buff; //���ݽ��ջ�����
	uint8_t state;
	uint16_t s_addr;
}master_order;

////////�ṹ������
typedef struct{
  uint8_t struct_type;//�ṹ��
  uint8_t* struct_p; //�ṹ����λָ��
  uint8_t struct_size; //�ṹ��
}data_manage;

//��վ����
typedef struct{
	uint8_t subnum;
  uint16_t s_addr;
	uint8_t state;
	data_manage* data_manage_p_get;
	data_manage* data_manage_p_set;
}sub_nature;

//��վ�����б�
typedef struct{
	uint8_t num;//��ǰ��վ��
	uint8_t num_max;//�����վ��
	uint8_t num_tmp;//�ݴ���վ���������·�������վ��Ŀ��ȴ������վ�������ƻ��Ҳ�����վ��
	sub_nature* sub_nature_p;//��վ���Թ����ָ��
}sub_list;

	//���ݰ��ĽṹΪ
	typedef struct{
		uint16_t m_addr; //��վ�������ַ
		uint16_t s_addr; //��վ�������ַ
		uint8_t  type;   //�����������źţ����У�0XC0��ǿ�����ӣ�0XC1���������ݡ�   ���У�0X40:ǿ�����ӻ�Ӧ��0X41������������Ӧ��0X42������ͨ�Ż�Ӧ��
		uint8_t  struct_type;//������������λ����STM8��·��ʹ�õ�ָ������һ�¡�����������أ�ͳһΪ0xff.
		uint8_t *data_p;//��ʼ��ʱ������ָ��STRU_auto_pack����
		uint16_t crc;    //ǰ������������ϣ�ȡ��2�ֽ�
	}STRU_auto_pack;

		typedef struct{
		uint8_t data_size; //���ݻ������Ĵ�С
		uint8_t data[24]; //���ݻ�����
	}package_data;

//	typedef struct{
//        uint8_t  size;
//        uint8_t  state;  //״̬			  
//		
//				uint8_t  num;    //
//			  uint8_t  step;   //��
//				uint8_t  nc;
//		    uint32_t time;
//		
//				
//			  uint32_t s_time;
//        uint8_t  type;   //��վ�������ַ
//        uint8_t  pack_new;   //0xC0 λΪ 1
//        uint16_t crc;    //ǰ������������ϣ�ȡ��2�ֽ�
//	}STRU_auto_ver;

		typedef struct{
        uint8_t  pack_size;//���ݰ���С����abs.size��ȡ��
				uint8_t  type_r;//���ܰ����ͣ��������У�
				uint8_t  type_t;//���Ͱ����������ͣ��������У�
				uint8_t  struct_type;//���ݵ����
				uint8_t	 struct_type_r;//���յ����������ͺ
	}STRU_auto_ver;//���߰�����Ϣ��ָ�����ͣ��������ͣ�
		

	
	typedef struct{
				uint8_t size;//�ӻ���
				uint16_t m_addr;     //���Ǳ���վ���е������ַ
        uint16_t s_addr[64]; //��վվ�����ַ,
	}STRU_eep_addr;


	//������Ϣ����,���ڻش�ȷ��
	typedef struct{
		uint16_t send_s_addr;
		uint8_t send_type;
		uint8_t send_struct_type;
		uint8_t send_flag;
		uint8_t send_chance;
		uint8_t send_time_s;
	}send_infor;
	extern send_infor send_info;
	extern uint32_t start_time;
	
	//���ͽ��ջ�����
	typedef struct{
		uint16_t size;
		uint8_t buff[32];
	}STRU_auto_buff;

	
	
	
	typedef struct{ 
		  uint8_t  brightness_percent;//���Ȱٷֱ�
		}brightness;

		/////////ʱ�����Ϣ,(2)��������Ϣ --------�ڲ���¼ʹ��
	typedef struct{  
		  uint32_t start_time;	//������ʼʱ���
		  uint32_t end_time;	//���ƽ���ʱ���
		}Brightness_level;

		/////////����LF145C1ָ��(3)---------�����Ҫ
	typedef struct{//���ߴ��Ͱ���10B��
		  uint16_t light_vol;		//�����ѹ
		  uint16_t battery_vol;		//��ص�ѹ
		  uint8_t current_front;	//��ŵ�������
		  uint8_t current_value;	//��ŵ���ֵ
		  uint16_t led_vol;			//LED��Դ���ѹ
		  uint16_t temple;			//�弶�¶�
		}Single_LF145C1_pama;

		/////////��ֵ(4,5)----------��������,һ���ǵ����,һ���������
	typedef struct{//���ߴ��Ͱ���16B��
		  uint16_t charge_current_max;		//����������ֵ
		  uint16_t discharge_current_max;	//�ŵ��������ֵ
		  uint16_t battery_value_max;		//������ѹ�����ֵ
		  uint16_t battery_value_min;		//��ذ���ѹ����Сֵ
		  uint8_t temple_max;				//�弶�¶����ֵ
		  uint8_t temple_flag_max;			//�¶ȷ���λ���ֵ
		  uint8_t temple_min;				//�弶�¶���Сֵ
		  uint8_t temple_flag_min;			//�¶ȷ���λ��Сֵ
		  uint16_t charge_AH;				//���AH��
		  uint16_t discharge_AH;			//�ŵ�AH��
		}Max_min_value;

		/////////��ʷ����(6)----------����ݲ�ʹ��,�Ǵ���оƬ�е���ʷ����
	typedef struct{
		  uint8_t related_history;				//�������
		  uint16_t battery_value_max_history;	//������ѹ�����ֵ
		  uint16_t charge_AH_history;			//���AH��
		  uint16_t discharge_AH_history;		//�ŵ�AH��
		}History_someday;

		/////////����״̬(7)---------�����ʵʱֵ
	typedef struct{//���ߴ��Ͱ�
		  uint8_t  step;						//�����е�step����
		  uint8_t  dayornight;					//��ҹ����ı�־��1�ǰ��죬2�Ǻ�ҹ
		  uint8_t  count;						//���ǳ����е�nig_ct��������ת����ҹ����ҹת������ļ���λ����ֵ��100���ڣ�
		  uint32_t  time_day;					//����ʱ�䣨��λ���룩
		  uint32_t  time_night;					//ҹ��ʱ�䣨��λ���룩
		  uint32_t  charge_AS_day;				//��������Ah��(����36000��λ����AH)
		  uint32_t  discharge_AS_night;			//ҹ��ų���Ah��(����36000��λ����AH)  
		}State_thisday;

		////////���Ʋ���(8)--------����д��������Զ�ȡ,�������ݲ��迼��
	typedef struct{//���ߴ��Ͱ���15B��
				uint8_t  time1;		//��һ�׶�ʱ��
				uint8_t  level1;	//��ʼ����ռ��������ȵİٷֱ�
				uint8_t  speed1;	//˥��������
				uint8_t  time2;		//�ڶ��׶�ʱ��
				uint8_t  level2;	//��ʼʱ��ռ������ȵİٷֱ�
				uint8_t  speed2;	//˥��������
				uint8_t  time3;		//�����׶�
				uint8_t  level3;	//��ʵ����ռ������ȵİٷֱ�
				uint8_t  speed3;	//˥��������
				uint8_t  time4;		//���Ľ׶�ʱ��
				uint8_t  level4;	//��ʼ����ռ��������ȵİٷֱ�
				uint8_t  speed4;	//˥��������
				uint8_t  time5;		//����׶�ʱ��
				uint8_t  level5;	//��ʼ����ռ��������ȵİٷֱ�
				uint8_t  speed5;	//˥������
		}URT_auto_stratage;

		/////////���Ʋ�������(9)
		//uart1Manage case9:

		/////////·�Ƶ�ַ(10)
	typedef struct{
		  uint8_t addr_h;	//���ص�ַ��λ
		  uint8_t addr_l;	//���ص�ַ��λ
		}llf145c1_addr;

		/////////��ص�ѹ(11,12)
	typedef struct{
		  uint8_t sunctrl_level; //��ص�ѹ�ȼ�
		}sun_control;

		////////��ȡ�ͻ���Ϣ(13 ,14)
	typedef struct{
			uint8_t save_byte;	//�����ֽ�
			uint8_t byte1;		//��һ�ֽ�
			uint8_t byte2;		//�ڶ��ֽ�
			uint8_t byte3;		//�����ֽ�
			uint8_t byte4;		//�����ֽ�
			uint8_t byte5;		//�����ֽ�
		}client_infor;
	
	


//����ṹ��
	typedef struct{
			uint16_t Address;
			uint32_t update_time;
			brightness bp;
		  Single_LF145C1_pama lf145c1;
			Max_min_value mmvalue_thisday;
			Max_min_value mmvalue_lastday;
			History_someday data_history;
			Brightness_level Brightness_level_Slave;
			State_thisday state;
			URT_auto_stratage stra;
			llf145c1_addr light_addr_1;
			llf145c1_addr light_addr_2;
			sun_control sun_level_set;
			sun_control sun_level_get;
			client_infor client_r;
			client_infor client_w;		
		}Node_List;
	
typedef struct{
			uint8_t* p_point[14];
			uint8_t  data_size[14];
}point_point; 
		
//�·���������Ϣ�ṹ��	
	typedef struct{ 
		  uint16_t brightness_time;//
		  uint8_t  brightness_percent;//
		}brightness_set;
		
	typedef struct{ 
		  uint8_t  related_days;//
		}history_data;	
	
	typedef struct{//
				uint8_t  time1;		//��һ�׶�ʱ��
				uint8_t  level1;	//��ʼ����ռ��������ȵİٷֱ�
				uint8_t  speed1;	//˥��������
				uint8_t  time2;		//�ڶ��׶�ʱ��
				uint8_t  level2;	//��ʼʱ��ռ������ȵİٷֱ�
				uint8_t  speed2;	//˥��������
				uint8_t  time3;		//�����׶�
				uint8_t  level3;	//��ʵ����ռ������ȵİٷֱ�
				uint8_t  speed3;	//˥��������
				uint8_t  time4;		//���Ľ׶�ʱ��
				uint8_t  level4;	//��ʼ����ռ��������ȵİٷֱ�
				uint8_t  speed4;	//˥��������
				uint8_t  time5;		//����׶�ʱ��
				uint8_t  level5;	//��ʼ����ռ��������ȵİٷֱ�
				uint8_t  speed5;	//˥������
		}URT_auto_stratage_set;
	
	typedef struct{ 
		  uint8_t  sunctrl_level;//
		}sun_set;		

	typedef struct{ 
		  uint8_t  byte1;//��һ�ֽ�
		  uint8_t  byte2;//�ڶ��ֽ�
		  uint8_t  byte3;//�����ֽ�
		  uint8_t  byte4;//�����ֽ�
		  uint8_t  byte5;//�����ֽ�
		}get_client_infor;			

//���ú󣬻��п���Ҫ��ȡԭ���Ĳ���	
typedef struct{ //��15���ֽ�
	uint8_t seg1;
	uint8_t lit1;
	uint8_t dec1;
	uint8_t seg2;
	uint8_t lit2;
	uint8_t dec2;
	uint8_t seg3;
	uint8_t lit3;
	uint8_t dec3;
	uint8_t seg4;
	uint8_t lit4;
	uint8_t dec4;
	uint8_t seg5;
	uint8_t lit5;
	uint8_t dec5;
}STRU_lv_policy;
	
	

	//�������ݣ�����¼64��
	typedef struct{		
		uint8_t  lit;		//�Ƶ�����
		uint16_t time;  //����ʱ��
		uint32_t btime; //����·�Ƶ����ȵ���ʼʱ�䣬���Ը������ֵ�����·�ƻ��������
	}STRU_lv_lit;
	
	
	//�����·�����ݣ�ÿ��5���Ӹ���һ��
	//�����������������¼64������������
	typedef struct{
		int16_t volt;			//��ص�ѹ
		int16_t ch_a;  		//������
		int16_t dch_a;  	//�ŵ����		
		int16_t ch_ah;	  //���ah��
		int16_t dch_ah;   //�ŵ�ah��
		int16_t temp;			//�¶�		
		int32_t time;     //
	}STRU_lv_now;
	
	//ÿ��15���Ӹ���һ��
	//��������ֵ���,ÿ���Ƶ���Ϣ���У�������64����
	typedef struct{
		int16_t volt_max;	//������ѹ���ֵ
		int16_t volt_min; //������ѹ��Сֵ
		int16_t ch_a_max; //���������ֵ
		int16_t dch_a_max;//�ŵ������Сֵ
		int16_t temp_max;	//�����¶����ֵ
		int16_t temp_min; //�����¶���Сֵ
		int16_t ch_ah;    //���ah��
		int16_t dch_ah;   //�ŵ�ah��
	}STRU_lv_max;
	
	//�������ֵ���
		
//������ѯ������ѡ�񣨴��1Сʱ�ϱ�һ�Σ�
typedef struct{
	uint8_t stuct_num;
	uint8_t data_type[14];
}struct_choose_fast;

//������ѯ������ѡ�񣨴��12Сʱ�ϱ�һ�Σ
typedef struct{
	uint8_t stuct_num;
	uint8_t data_type[14];
}struct_choose_slow;
//Ƶ�ʿ��

typedef struct{
	uint16_t broad_s_addr;
	uint8_t broad_type;
	uint8_t broad_struct_type;
	uint8_t broad_chance;
	uint8_t broad_flag;
}broad_set;

typedef struct{
	//�ӻ���
	uint8_t frequency_setnum;
	uint8_t frequency_getnum;
	uint8_t frequency_self;
	//�Լ���
	uint8_t high;
	uint8_t med;
	uint8_t low;
	uint8_t num;
}frequency_control;
void BC28_send(uint16_t s_addr , uint8_t type , uint8_t struct_type);
void BC28_recv(uint16_t s_addr , uint8_t type , uint8_t struct_type);
void BC28_data_pack(void);
void at_send_str(char* s1);
void at_send_data(uint8_t* val_p , uint8_t val_size);
void send_data_deliver(uint8_t* buffer , uint8_t* data_size_p_t , uint8_t struct_type);
void recv_data_deliver(uint8_t* buffer , uint8_t* data_size_p_r , uint8_t struct_type);
void connect_10m_call(void);
void fast_fresh(void);
void slow_fresh(void);
void fresh_struct_data(void);
void connect_process(void);
void connect_init(void);
void unresponse_resend(void);
void data_judge(void);
void send_reserve(uint8_t i);
void send_auto_pack(uint16_t addr, uint8_t type,uint8_t struct_type);
void SX1278LoRaSetRFFrequency_reset(uint8_t num);
void broad_send_model(void);
#endif //__SX1278_H__
