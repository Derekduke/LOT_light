
#ifndef __SX_UART_H
#define __SX_UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sx_board.h"

/* 路灯回复的数据信息结构体---------------------------------------------------*/
////(1)没有回复数据

/////非广播的形式设置LF145C1亮度(2)
typedef struct{
  uint8_t  brightness_value;//亮度百分比
}brightness;

/////////时间戳信息,(2)的亮灯时间信息
typedef struct{
  
  uint32_t start_time;//亮灯起始时间戳
  uint32_t end_time;//亮灯结束时间戳
  
}Brightness_level;

/////////单个LF145C1指令（3）
typedef struct{
  uint16_t light_vol;//光伏电压
  uint16_t battery_vol;//电池电压
  uint8_t current_front;//充放电流方向
  uint8_t current_value;//充放电流值
  uint16_t led_vol;//LED光源板电压
  uint16_t temple;//板级温度
}Single_LF145C1_pama;

/////////最值（4,5）
typedef struct{
  uint16_t charge_current_max;//充电流的最大值×10
  uint16_t discharge_current_max;//放电流的最大值×10
  uint16_t battery_value_max;//电池组电压的最大值
  uint16_t battery_value_min;//电池包电压的最小值
  uint8_t temple_max;//板级温度最大值
  uint8_t temple_flag_max;//温度符号位最大值
  uint8_t temple_min;//板级温度最小值
  uint8_t temple_flag_min;//温度符号位最小值
  uint16_t charge_AH;//充电AH数
  uint16_t discharge_AH;//放电AH数
}Max_min_value;

/////////历史数据（6）
typedef struct{
  uint8_t related_history;//相对天数
  uint8_t battery_value_max_history;//电池组电压的最大值
  uint16_t charge_AH_history;//充电AH数
  uint16_t discharge_AH_history;//放电AH数
}History_someday;

/////////当日状态（7）
typedef struct{
  uint8_t  step;//程序中的step变量
  uint8_t  dayornight;//白天黑夜的标志：1是白天，2是黑夜
  uint8_t  count;//就是程序中的nig_ct变量白天转到黑夜，黑夜转到白天的计数位（数值是100以内）
  uint32_t  time_day;//白天时间（单位：秒）
  uint32_t  time_night;//夜晚时间（单位：秒）
  uint32_t  charge_AS_day;//白天充入的AS数（除以36000后单位就是AH）
  uint32_t  discharge_AS_night;//夜间放出的AS数(除以36000后单位就是AH)
  
}State_thisday;

/////////亮灯策略（8）
typedef struct{
        uint8_t  time1;//第一阶段时间
        uint8_t  level1;//起始亮度占总最高亮度的百分比
        uint8_t  speed1;//衰减的速率
        uint8_t  time2;//第二阶段时间
        uint8_t  level2;//起始亮度占总最高亮度的百分比
        uint8_t  speed2;//衰减的速率   
        uint8_t  time3;//第三阶段时间
        uint8_t  level3;//起始亮度占总最高亮度的百分比
        uint8_t  speed3;//衰减的速率   
        uint8_t  time4;//第四阶段时间
        uint8_t  level4;//起始亮度占总最高亮度的百分比
        uint8_t  speed4;//衰减的速率   
        uint8_t  time5;//第五阶段时间
        uint8_t  level5;//起始亮度占总最高亮度的百分比
        uint8_t  speed5;//衰减的速率   
}URT_auto_stratage;

/////////亮灯策略设置（9）
//uart1Manage case9:

/////////路灯地址（10）
typedef struct{
  uint8_t addr_h;//返回地址高位
  uint8_t addr_l;//返回地址低位
}llf145c1_addr;

/////////光控电压（11,12）
typedef struct{
  uint8_t sunctrl_level; //光控电压等级
}sun_control;

////////读取客户信息（13 ,14）
typedef struct{
    uint8_t save_byte;//保留字节
    uint8_t byte1;//第一字节
    uint8_t byte2;//第二字节
    uint8_t byte3;//第三字节
    uint8_t byte4;//第四字节
    uint8_t byte5;//第五字节
}client_infor;

////////结构体管理表
typedef struct{
  uint8_t struct_type;//结构体名称
  uint8_t* struct_p; //结构体首位指针
  uint8_t struct_size; //结构体大小
}data_manage;


void UART1_Init(unsigned int baudrate);
void It_Get_Byte(void);
void uart1Manage(void);
void urt1RM06(void);
void changeSettings(void);
void urt1SendChar(uint8_t a);
void urt1SendStr(uint8_t *Str,uint8_t size);
////////////////////调试使用
void fun_for_debug(void);

/*路灯发送的数据信息结构体---------------------------------------------------*/
////广播的形式设置所有LF145C1亮度（1）（2）
typedef struct{
  //uint8_t all; //all = 1为非广播 ， all = 0为广播
  uint16_t brightness_time;
  uint8_t brightness_percent;
  
}brightness_set;

////读取单个LF145C1参数指令（3）
//0x03指令;

////读取当天最值指令（4）
//0x04指令;

////读取前一天最值指令（5）
//0x05指令;

////读取某一天的历史数据（6）
typedef struct{
  uint8_t related_days;
}history_data;

////读取当日状态信息（7）
//0x07指令；

////亮灯策略读取（8）
//0x0C指令；

////亮灯策略设置（9）
typedef struct{
  uint8_t  time1;//第一阶段时间
  uint8_t  level1;//起始亮度占总最高亮度的百分比
  uint8_t  speed1;//衰减的速率
  uint8_t  time2;//第二阶段时间
  uint8_t  level2;//起始亮度占总最高亮度的百分比
  uint8_t  speed2;//衰减的速率   
  uint8_t  time3;//第三阶段时间
  uint8_t  level3;//起始亮度占总最高亮度的百分比
  uint8_t  speed3;//衰减的速率   
  uint8_t  time4;//第四阶段时间
  uint8_t  level4;//起始亮度占总最高亮度的百分比
  uint8_t  speed4;//衰减的速率   
  uint8_t  time5;//第五阶段时间
  uint8_t  level5;//起始亮度占总最高亮度的百分比
  uint8_t  speed5;//衰减的速率   
}URT_auto_stratage_set;

////获取地址命令(10)
//0x09、0x02、0x03指令;

////设置光控电压命令（11）
typedef struct{
  uint8_t sunctrl_level;
}sun_set;

////查询光控电压命令（12）
//0x1E指令；

////读取客户信息（13）
//0x06指令;

////写客户信息（14）
typedef struct{
  //uint8_t save_byte;//保留字节
  uint8_t byte1;//第一字节
  uint8_t byte2;//第二字节
  uint8_t byte3;//第三字节
  uint8_t byte4;//第四字节
  uint8_t byte5;//第五字节
}get_client_infor;

//以下命令为与路灯板LF145C1通信的指令1到指令14的发送函数

///////////////
//读取参数指令
void Read_LF145C1_par(uint8_t type);
//////////////
//可选参数
#define SINGLE_LF145C1_SEND 0x03   //读取单个LF145C1指令（指令3）//周期读取
#define THISDAY_MAXMIN_SEND    0x04   //读取当天最值（指令4）//周期读取
#define LASTDAY_MAXMIN_SEND  0X05   //读取前一天最值（指令5）//周期读取
#define CLIENT_INFOR_SEND   0x06   //客户信息读取（指令13）
#define THISDAY_STATE_SEND  0X07   //读取当日状态信息（指令7）//周期读取
#define LIGHTING_STRA_SEND  0x0C   //亮灯策略读取（指令8）周期读取

/////////////////（指令9）
//亮灯策略设置
void Light_Stra_Setting(void);
///////////////（指令1，2）
//设置亮度,all代表是否广播
void Brightness_Setting(uint8_t percent,uint16_t time,uint8_t all);
/////////////(指令11)
//设置光控电压，level为等级
void SunCtrl_Setting(uint8_t level);
/////////////（指令12）//周期读取
//查询光控电压
void SunCtrl_RQS(void);
//////////////（指令6）//周期读取
//读取之前某一天历史数据
void History_RQS(uint8_t days);
///////////////（指令10）
//获取地址命令，明明类型可使用0x09,0x02,0x03：
void Address_RQS(uint8_t struct_type);
///////////////（指令14）
//写客户信息
void Client_Infor_Write(uint8_t *infor);


////上层取用结构体数据
void send_data_deliver(uint8_t* buffer , uint8_t* data_size_p_t , uint8_t struct_type);
void recv_data_deliver(uint8_t* buffer , uint8_t* data_size_p_r , uint8_t struct_type);



/************************end **************************************************/
#endif