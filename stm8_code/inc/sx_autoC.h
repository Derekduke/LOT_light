//目标实现通信的自动连接功能
#ifndef __SX_AUTOC_H
#define __SX_AUTOC_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sx_board.h"

#define MA_CALL_SON             0xC0    //主站  【1.寻子】 此时s_addr = 0xFFFF
#define MA_CONNECT              0xC1    //主站  【2.强制连接】
#define MA_RESET                0xC2    //主站  【3.强制断开】
#define MA_ANS                  0xC3    //主站  【4.主应答】
#define MA_ASK_ADDR             0xC4    //主站  【4.寻址命令】此时s_addr = 0xFFFF
#define SA_ANS_CALL             0x40    //子站  【5.应答寻子】
#define SA_ASK_DISC             0x41    //子站  【6.劝离】
#define SA_ANS                  0x42    //子站  【7.子应答】



//#define MA_CALL_SON             0xC0    //主站  【1.寻子】 此时s_addr = 0xFFFF
//#define MA_CONNECT              0xC1    //主站  【2.强制连接】
//#define MA_RESET                0xC2    //主站  【3.强制断开】
//#define MA_ANS                  0xC3    //主站  【4.主应答】
//#define MA_ASK_ADDR             0xC4    //主站  【5.询问是否连通】
//
//#define SA_ANS_CALL             0x40    //子站  【7.应答寻子】
//#define SA_ASK_DISC             0x41    //子站  【8.劝离】
//#define SA_ANS                  0x43    //子站  【9.子应答】
//#define SA_ANS_ASK              0x44    //子站  【10.子询问应答，特点是接到无需回应】


//#define MA_RQS_NOW              0xA0    //主站  【1.索要当前数据】
//#define MA_RQS_M                0xA1    //主站  【2.索要当天最值】
//#define MA_CFG_MODE             0xA2    //主站  【3.配置亮灯模式】
//#define MA_CTRL_SET             0xA3    //主站  【4.亮灯控制设置】
//#define                         0xA4    //主站  【】
//
//#define SA_                     0x20    //子站  【1】
//#define SA_                     0x21    //子站  【1】
//#define SA_                     0x23    //子站  【1】
//#define SA_                     0x24    //子站  【1】

#define SIZE_OF_STRU_EEPHEADER  4
/*
typedef struct{
        uint16_t m_addr; //主站的物理地址
        uint16_t s_addr; //子站的物理地址
        uint8_t  type;   //0xC0 位为 1
        uint16_t crc;    //前面所有数据求合，取后2字节
}STRU_auto_pack;

typedef struct{
        uint8_t  size;
        uint8_t  state;ta  //状态
        uint8_t  type;   //子站的物理地址
        uint8_t  pack_new;   //0xC0 位为 1
        uint16_t crc;    //前面所有数据求合，取后2字节
}STRU_auto_ver;
*/

//数据包格式
typedef struct{
	uint16_t m_addr; 	//主站的物理地址
	uint16_t s_addr; 	//子站的物理地址
	uint8_t  type;   	//最高位为1，则表示主站发出的内容，最高位为0，则表示子站发出的内容
	uint8_t struct_type;//对应的数据存放结构体选择位
	uint8_t  *data_p;	 	//数据区（使用char*指针指向数据缓存区（p = (uint8_t*) &package_data）
	uint16_t crc;    	//前面所有数据求合，取后2字节
}STRU_auto_pack;

//struct_type规范与朗越路灯协议中发送帧的type位相同
//如： 2的发0x01 ， 2的收0x81

//发送接收数据缓存区
typedef struct{
	uint8_t data_size; //(pack_size - 6)或数据结构体自身长度
	uint8_t data[24]; //p = package_data.data
}package_data;

//数据包标志信息记录
typedef struct{
        uint8_t  pack_size;   //数据包大小（从abf.size获取）
        uint8_t  type_r;   //接收数据包指令类型（atpr.type)
        uint8_t  type_t;   //发送数据包指令类型
        uint8_t  struct_type; //结构体类型（atpr.struct_type)
        uint8_t  data_flag; //数据传输结束标志位
}STRU_auto_ver;

//地址信息存储
typedef struct{
        uint16_t m_addr; //主站的物理地址,为0xffff 表示没有地址
        uint16_t s_addr; //子站站物理地址,
}STRU_eep_addr;

//lora收发数据缓存区
typedef struct{
  uint8_t size;
  uint8_t buff[32];
}STRU_auto_buff;

//频率选择记录
typedef struct{
  uint8_t high;
  uint8_t med;
  uint8_t low;
  uint8_t num;
  uint8_t flag;
}frequency_control;


extern package_data data_r;//接收数据缓存区
extern package_data data_t;//发送数据缓存区
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