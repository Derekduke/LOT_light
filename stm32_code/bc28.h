#ifndef __bc28v1_h
#define __bc28v1_h

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "uart.h"



typedef struct _nbiot_info_t
{
uint8_t initial_state;//��ʼ����״̬�����ߵ���һ��
uint8_t CSQ;
char *CIMI;//һ������
char *IMEI;//ģ���IMEI
uint8_t new_event;//
char *cmd;//���͸�ģ�������
uint8_t cmdlen;//����ĳ���
char recv[64];//��ģ���������������
uint8_t recv_valid_len;//���������������Ч����
}nbiot_info;//NBIOT������



extern nbiot_info nb;//�������������ⲿ����

uint16_t StringHexToInt(unsigned char *num);
char *strnstr(const char *s1, const char *s2, size_t len);//~��s1��ǰpos1���ַ����в���s2��������ھͷ���s2��s1�е�λ�ã����û���ҵ������� NULL������������ص��Ǳ��ض��Ժ������
uint8_t strnstrnum(const char *s1, const char *s2, size_t len);//~����ͬ�ϣ�����������ص���ֵ��������ķ���Ч�ʸ�,���Ҳ��ᵼ��s1���и�
//unsigned char Str_Match(void);//�ַ���ƥ�䣬���ҷ��������������ַ�����λ������
int StrOctToStrHex(const char *bufin, char *bufout,int len);//~��һ�����ֵ��ַ���ת��16���Ƶ�����
void StrHexToIntn(const unsigned char *source, unsigned char *dest, int sourceLen); //~��λ����λʮ�������ַ�������תint�������֣������ó���
uint16_t StrHexToInt(unsigned char *num);//~��λ����λʮ�������ַ�������תint�������֣�����NB�������ݷ�����ʱ�������������д���ע���������ĸһ��Ҫ��д
char *IntToHexStr(uint16_t Octnum,uint16_t byte_num);//~int��������ת��ʮ�����Ƶ��ַ���������NB�����ϱ���ʱ����д���
char *substring(char *s1, uint16_t len1, uint16_t len2);//~��ȡһ���ַ������õ��Ӵ�,len1���ܱ�len2��



void nb_initial(void);//bc28�ĳ�ʼ������
void nb_choose_operator(void);//������Ӫ�̣���ѯ��NBAND�������趨�Ժ���Բ�����
void nb_get_CSQ(void);//���ͻ�ȡCSQ������

void nb_send(void);//nb�ķ��ͳ���
void nb_cmd_manage(char *nb_cmd);//����NB�������Ķ���


void nb_AT(void);
void nb_Get_IMEI(void);//��ȡIMEI��ֵ
void nb_Get_CSQ(void);//��ȡCSQ�ź�ֵ
void nb_Get_CIMI(void);//��ȡ����
void nb_Get_Operator(void);//��ȡ��Ӫ��
void nb_Test_Report(void);//�����ϱ�


#endif
