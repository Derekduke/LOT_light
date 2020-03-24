#ifndef __bc28v1_h
#define __bc28v1_h

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "uart.h"



typedef struct _nbiot_info_t
{
uint8_t initial_state;//初始化的状态机，走到哪一步
uint8_t CSQ;
char *CIMI;//一串卡号
char *IMEI;//模组的IMEI
uint8_t new_event;//
char *cmd;//发送给模组的命令
uint8_t cmdlen;//命令的长度
char recv[64];//从模组接收上来的命令
uint8_t recv_valid_len;//收上来的命令的有效长度
}nbiot_info;//NBIOT的配置



extern nbiot_info nb;//将这个变量变成外部变量

uint16_t StringHexToInt(unsigned char *num);
char *strnstr(const char *s1, const char *s2, size_t len);//~在s1的前pos1个字符串中查找s2，如果存在就返回s2在s1中的位置，如果没有找到，返回 NULL；这个方法返回的是被截断以后的数组
uint8_t strnstrnum(const char *s1, const char *s2, size_t len);//~功能同上，这个方法返回的是值，比下面的方法效率高,而且不会导致s1被切割
//unsigned char Str_Match(void);//字符串匹配，并且返回输出这个搜索字符串的位置数字
int StrOctToStrHex(const char *bufin, char *bufout,int len);//~将一串数字的字符串转成16进制的数字
void StrHexToIntn(const unsigned char *source, unsigned char *dest, int sourceLen); //~两位或四位十六进制字符串数字转int类型数字，可设置长度
uint16_t StrHexToInt(unsigned char *num);//~两位或四位十六进制字符串数字转int类型数字，用于NB数据数据发下来时解析到本机进行处理，注意这里的子母一定要大写
char *IntToHexStr(uint16_t Octnum,uint16_t byte_num);//~int类型数字转成十六进制的字符串，用于NB数据上报的时候进行处理
char *substring(char *s1, uint16_t len1, uint16_t len2);//~截取一段字符串，得到子串,len1不能比len2大



void nb_initial(void);//bc28的初始化程序
void nb_choose_operator(void);//更换运营商，查询到NBAND不符合设定以后可以操作的
void nb_get_CSQ(void);//发送获取CSQ的命令

void nb_send(void);//nb的发送程序
void nb_cmd_manage(char *nb_cmd);//处理NB发下来的东西


void nb_AT(void);
void nb_Get_IMEI(void);//获取IMEI的值
void nb_Get_CSQ(void);//获取CSQ信号值
void nb_Get_CIMI(void);//获取卡号
void nb_Get_Operator(void);//获取运营商
void nb_Test_Report(void);//测试上报


#endif
