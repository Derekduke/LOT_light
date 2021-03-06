
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
//上传服务器格式
typedef struct{
	uint16_t at_size;
	char at_buff[50];
}mas_buff;
//BC28下发包
typedef struct{
	uint8_t bc_size;
	uint8_t bc_type;
	uint8_t bc_data[20];
}bc_pack;

//服务器下发指令
typedef struct{
	uint8_t subnum; //目标子机号
	uint8_t type;
	uint8_t struct_type;
	uint8_t* master_buff; //数据接收缓存区
	uint8_t state;
	uint16_t s_addr;
}master_order;

////////结构体管理表
typedef struct{
  uint8_t struct_type;//结构体
  uint8_t* struct_p; //结构体首位指针
  uint8_t struct_size; //结构体
}data_manage;

//子站属性
typedef struct{
	uint8_t subnum;
  uint16_t s_addr;
	uint8_t state;
	data_manage* data_manage_p_get;
	data_manage* data_manage_p_set;
}sub_nature;

//子站管理列表
typedef struct{
	uint8_t num;//当前子站数
	uint8_t num_max;//最大子站数
	uint8_t num_tmp;//暂存子站数（比如下发增减子站数目，却可能子站长度限制或找不到子站）
	sub_nature* sub_nature_p;//子站属性管理表指针
}sub_list;

	//数据包的结构为
	typedef struct{
		uint16_t m_addr; //主站的物理地址
		uint16_t s_addr; //子站的物理地址
		uint8_t  type;   //区分上下行信号：下行：0XC0：强制连接，0XC1：请求数据。   上行：0X40:强制连接回应，0X41：名花有主回应，0X42：数据通信回应。
		uint8_t  struct_type;//区分数据类型位：与STM8与路灯使用的指令类型一致。若是连接相关，统一为0xff.
		uint8_t *data_p;//初始化时候让其指向STRU_auto_pack类型
		uint16_t crc;    //前面所有数据求合，取后2字节
	}STRU_auto_pack;

		typedef struct{
		uint8_t data_size; //数据缓存区的大小
		uint8_t data[24]; //数据缓存区
	}package_data;

//	typedef struct{
//        uint8_t  size;
//        uint8_t  state;  //状态			  
//		
//				uint8_t  num;    //
//			  uint8_t  step;   //步
//				uint8_t  nc;
//		    uint32_t time;
//		
//				
//			  uint32_t s_time;
//        uint8_t  type;   //子站的物理地址
//        uint8_t  pack_new;   //0xC0 位为 1
//        uint16_t crc;    //前面所有数据求合，取后2字节
//	}STRU_auto_ver;

		typedef struct{
        uint8_t  pack_size;//数据包大小（从abs.size获取）
				uint8_t  type_r;//接受包类型（上行下行）
				uint8_t  type_t;//发送包的数据类型（上行下行）
				uint8_t  struct_type;//数据的类�
				uint8_t	 struct_type_r;//接收到的数据类型�
	}STRU_auto_ver;//无线包的信息（指令类型，数据类型）
		

	
	typedef struct{
				uint8_t size;//子机数
				uint16_t m_addr;     //这是本主站固有的物理地址
        uint16_t s_addr[64]; //子站站物理地址,
	}STRU_eep_addr;


	//发送信息缓存,用于回传确认
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
	
	//发送接收缓存区
	typedef struct{
		uint16_t size;
		uint8_t buff[32];
	}STRU_auto_buff;

	
	
	
	typedef struct{ 
		  uint8_t  brightness_percent;//亮度百分比
		}brightness;

		/////////时间戳信息,(2)的亮灯信息 --------内部记录使用
	typedef struct{  
		  uint32_t start_time;	//亮灯起始时间戳
		  uint32_t end_time;	//亮灯结束时间戳
		}Brightness_level;

		/////////单个LF145C1指令(3)---------这个需要
	typedef struct{//无线传送包【10B】
		  uint16_t light_vol;		//光伏电压
		  uint16_t battery_vol;		//电池电压
		  uint8_t current_front;	//充放电流方向
		  uint8_t current_value;	//充放电流值
		  uint16_t led_vol;			//LED光源板电压
		  uint16_t temple;			//板级温度
		}Single_LF145C1_pama;

		/////////最值(4,5)----------包括两种,一种是当天的,一种是昨天的
	typedef struct{//无线传送包【16B】
		  uint16_t charge_current_max;		//充电流的最大值
		  uint16_t discharge_current_max;	//放电流的最大值
		  uint16_t battery_value_max;		//电池组电压的最大值
		  uint16_t battery_value_min;		//电池包电压的最小值
		  uint8_t temple_max;				//板级温度最大值
		  uint8_t temple_flag_max;			//温度符号位最大值
		  uint8_t temple_min;				//板级温度最小值
		  uint8_t temple_flag_min;			//温度符号位最小值
		  uint16_t charge_AH;				//充电AH数
		  uint16_t discharge_AH;			//放电AH数
		}Max_min_value;

		/////////历史数据(6)----------这个暂不使用,是存在芯片中的历史数据
	typedef struct{
		  uint8_t related_history;				//相对天数
		  uint16_t battery_value_max_history;	//电池组电压的最大值
		  uint16_t charge_AH_history;			//充电AH数
		  uint16_t discharge_AH_history;		//放电AH数
		}History_someday;

		/////////当日状态(7)---------当天的实时值
	typedef struct{//无线传送包
		  uint8_t  step;						//程序中的step变量
		  uint8_t  dayornight;					//黑夜白天的标志：1是白天，2是黑夜
		  uint8_t  count;						//就是程序中的nig_ct变量白天转到黑夜，黑夜转到白天的计数位（数值是100以内）
		  uint32_t  time_day;					//白天时间（单位：秒）
		  uint32_t  time_night;					//夜晚时间（单位：秒）
		  uint32_t  charge_AS_day;				//白天充入的Ah数(除以36000后单位就是AH)
		  uint32_t  discharge_AS_night;			//夜晚放出的Ah数(除以36000后单位就是AH)  
		}State_thisday;

		////////亮灯策略(8)--------存在写策略与策略读取,具体内容不需考虑
	typedef struct{//无线传送包【15B】
				uint8_t  time1;		//第一阶段时间
				uint8_t  level1;	//起始亮度占总最高亮度的百分比
				uint8_t  speed1;	//衰减的速率
				uint8_t  time2;		//第二阶段时间
				uint8_t  level2;	//起始时间占最高亮度的百分比
				uint8_t  speed2;	//衰减的速率
				uint8_t  time3;		//第三阶段
				uint8_t  level3;	//其实亮度占最高亮度的百分比
				uint8_t  speed3;	//衰减的速率
				uint8_t  time4;		//第四阶段时间
				uint8_t  level4;	//起始亮度占总最高亮度的百分比
				uint8_t  speed4;	//衰减的速率
				uint8_t  time5;		//第五阶段时间
				uint8_t  level5;	//起始亮度占总最高亮度的百分比
				uint8_t  speed5;	//衰减速率
		}URT_auto_stratage;

		/////////亮灯策略设置(9)
		//uart1Manage case9:

		/////////路灯地址(10)
	typedef struct{
		  uint8_t addr_h;	//返回地址高位
		  uint8_t addr_l;	//返回地址低位
		}llf145c1_addr;

		/////////光控电压(11,12)
	typedef struct{
		  uint8_t sunctrl_level; //光控电压等级
		}sun_control;

		////////读取客户信息(13 ,14)
	typedef struct{
			uint8_t save_byte;	//保留字节
			uint8_t byte1;		//第一字节
			uint8_t byte2;		//第二字节
			uint8_t byte3;		//第三字节
			uint8_t byte4;		//第四字节
			uint8_t byte5;		//第五字节
		}client_infor;
	
	


//链表结构体
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
		
//下发的数据信息结构体	
	typedef struct{ 
		  uint16_t brightness_time;//
		  uint8_t  brightness_percent;//
		}brightness_set;
		
	typedef struct{ 
		  uint8_t  related_days;//
		}history_data;	
	
	typedef struct{//
				uint8_t  time1;		//第一阶段时间
				uint8_t  level1;	//起始亮度占总最高亮度的百分比
				uint8_t  speed1;	//衰减的速率
				uint8_t  time2;		//第二阶段时间
				uint8_t  level2;	//起始时间占最高亮度的百分比
				uint8_t  speed2;	//衰减的速率
				uint8_t  time3;		//第三阶段
				uint8_t  level3;	//其实亮度占最高亮度的百分比
				uint8_t  speed3;	//衰减的速率
				uint8_t  time4;		//第四阶段时间
				uint8_t  level4;	//起始亮度占总最高亮度的百分比
				uint8_t  speed4;	//衰减的速率
				uint8_t  time5;		//第五阶段时间
				uint8_t  level5;	//起始亮度占总最高亮度的百分比
				uint8_t  speed5;	//衰减速率
		}URT_auto_stratage_set;
	
	typedef struct{ 
		  uint8_t  sunctrl_level;//
		}sun_set;		

	typedef struct{ 
		  uint8_t  byte1;//第一字节
		  uint8_t  byte2;//第二字节
		  uint8_t  byte3;//第三字节
		  uint8_t  byte4;//第四字节
		  uint8_t  byte5;//第五字节
		}get_client_infor;			

//设置后，还有可能要读取原来的策略	
typedef struct{ //共15个字节
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
	
	

	//亮度数据，共记录64个
	typedef struct{		
		uint8_t  lit;		//灯的亮度
		uint16_t time;  //亮灯时间
		uint32_t btime; //设置路灯的亮度的起始时间，可以根据这个值计算出路灯还能亮多久
	}STRU_lv_lit;
	
	
	//当天的路灯数据，每隔5分钟更新一次
	//当天数据情况，共记录64个这样的数据
	typedef struct{
		int16_t volt;			//电池电压
		int16_t ch_a;  		//充电电流
		int16_t dch_a;  	//放电电流		
		int16_t ch_ah;	  //充电ah数
		int16_t dch_ah;   //放电ah数
		int16_t temp;			//温度		
		int32_t time;     //
	}STRU_lv_now;
	
	//每隔15分钟更新一次
	//当天的最大值情况,每个灯的信息都有，共设置64个；
	typedef struct{
		int16_t volt_max;	//电池组电压最大值
		int16_t volt_min; //电池组电压最小值
		int16_t ch_a_max; //充电电流最大值
		int16_t dch_a_max;//放电电流最小值
		int16_t temp_max;	//板上温度最大值
		int16_t temp_min; //板上温度最小值
		int16_t ch_ah;    //充电ah数
		int16_t dch_ah;   //放电ah数
	}STRU_lv_max;
	
	//昨天最大值情况
		
//快速轮询的数据选择（大概1小时上报一次）
typedef struct{
	uint8_t stuct_num;
	uint8_t data_type[14];
}struct_choose_fast;

//慢速轮询的数据选择（大概12小时上报一次�
typedef struct{
	uint8_t stuct_num;
	uint8_t data_type[14];
}struct_choose_slow;
//频率控�

typedef struct{
	uint16_t broad_s_addr;
	uint8_t broad_type;
	uint8_t broad_struct_type;
	uint8_t broad_chance;
	uint8_t broad_flag;
}broad_set;

typedef struct{
	//子机的
	uint8_t frequency_setnum;
	uint8_t frequency_getnum;
	uint8_t frequency_self;
	//自己的
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
