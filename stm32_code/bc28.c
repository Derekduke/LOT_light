#include <stdio.h>
#include <string.h>
#include "board.h"
#include "uart.h"
#include "ctype.h"
#include "bc28.h"

//COAPͨ��ʹ�ã�hxw5/24

unsigned char  nb_operator_choose=8;//ѡ����Ӫ�̣�5�ǵ��ţ�8�ǵ���
//unsigned char  nb_platform_addr[]={"1234"};//ѡ���ƽ̨�ĶԽӵ�ַ��
uint8_t nb_initial_seq[6][30]={{"AT+CFUN=1"},{"AT+NCDP=139.159.140.34,5683"},{"AT+NNMI=1"},{"AT+CGSN=1"},{"AT+CIMI"},{"AT+NBAND?"}};//NBIOT��ʼ�����





nbiot_info nb;

char *strnstr(const char *s1, const char *s2, size_t len)//��s1��ǰpos1���ַ����в���s2��������ھͷ���s2��s1�е�λ�ã����û���ҵ������� NULL
{
    size_t l2;

    l2 = strlen(s2); 
    if (!l2)
        return (char *)s1;
    while (len >= l2) {
        len--;
        if (!memcmp(s1, s2, l2))
            return (char *)s1;
        s1++;
    }
    return NULL;
}

uint8_t strnstrnum(const char *s1, const char *s2, size_t len)//��s1��ǰpos1���ַ����в���s2��������ھͷ���s2��s1�е�λ�ã����û���ҵ������� NULL
{  
    size_t l2;
    int8_t cnt=0;
    l2 = strlen(s2); 
    if (!l2)
        return  0;
    while (len >= l2) {
        len--;
        if (!memcmp(s1, s2, l2))
					return cnt;
         //   return (char *)s1;
				cnt++;
        s1++;
    }
		s1=s1-cnt-1;
    return 255;
}



int StrOctToStrHex(const char *bufin, char *bufout, int len)
{
    int i = 0;
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for(i = 0; i < len; i++)
    {
        sprintf(bufout+i*2, "%02X", bufin[i]);
    }
    return 0;
}


void StrHexToIntn(const unsigned char *source, unsigned char *dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;
    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);
        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;
        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return ;
}
//////���������õĺ���
uint16_t StringHexToInt(unsigned char *num){//ʮ�������ַ���תʮ�������֣�����NB�������ݷ�����ʱ����
unsigned char  temp_var[2];
	temp_var[0]=num[0];
	temp_var[1]=num[1];	
	unsigned char  temp_result_var[2];
	uint16_t sum=0;
	for(int i=0;i<2;i++){
	if(temp_var[i]>=0x30&&temp_var[i]<=0x39){
		temp_result_var[i]=temp_var[i]-0x30;}
  else if (temp_var[i]>=0x41&&temp_var[i]<=0x46){
		temp_result_var[i]=temp_var[i]-0x37;}
	else{temp_result_var[i]=0;}
	}
	sum=temp_result_var[0]*16+temp_result_var[1];
  return sum;
}
/////////////////////////

uint16_t StrHexToInt(unsigned char *num){//ʮ�������ַ���תʮ�������֣�����NB�������ݷ�����ʱ����
	unsigned char  temp_var[4]={0};
	uint8_t runtime;
	runtime=strlen((char *)num);
	for(int i=0;i<runtime;i++){
	temp_var[i]=num[i];
	}
	
unsigned char  temp_result_var[4];
uint16_t sum=0;
	
	for(int i=0;i<runtime;i++){
if(temp_var[i]>=0x30&&temp_var[i]<=0x39){
  temp_result_var[i]=temp_var[i]-0x30;}
  else if (temp_var[i]>=0x41&&temp_var[i]<=0x46){
   temp_result_var[i]=temp_var[i]-0x37;}
 else{temp_result_var[i]=0;}
}
//	for(int i=runtime-1;i>=0;i--){   //�Ƚϼ�࣬����Ҫ��ѭ���ʹη�
//	sum+=temp_result_var[i]*(16^(runtime-i-1));
//	}
switch(runtime){//���ŷ������������п�һ��
	case 1:
		sum=temp_result_var[0];
		break;
	case 2:
		sum=temp_result_var[0]*16+temp_result_var[1];
		break;
	case 3:
		sum=temp_result_var[0]*256+temp_result_var[1]*16+temp_result_var[2];
		break;
	case 4:
		sum=temp_result_var[0]*4096+temp_result_var[1]*256+temp_result_var[2]*16+temp_result_var[3];
		break;
	default:
		break;
}
  return sum;
}


char *IntToHexStr(uint16_t Octnum,uint16_t byte_num){//��int����ת��ʮ�����Ƶ��ַ�������,������ָλ����1��1�ֽ�,2��2�ֽ�
  char hexstring[4];
	static char d[4];
  uint8_t result,runtime;
if(Octnum<=255&&byte_num==1){
runtime=2;
}else if(Octnum>255||byte_num==2){
runtime=4;
}
for(int i=0;i<runtime;i++){
result=Octnum%16;
Octnum=(Octnum-result)/16;
switch (result){
case 10:
hexstring[i]='A';
break;
case 11:
hexstring[i]='B';
break;
case 12:
hexstring[i]='C';
break;
case 13:
hexstring[i]='D';
break;
case 14:
hexstring[i]='E';
break;
case 15:
hexstring[i]='F';
break;
case 0:
hexstring[i]='0';
break;
case 1:
hexstring[i]='1';
break;
case 2:
hexstring[i]='2';
break;
case 3:
hexstring[i]='3';
break;
case 4:
hexstring[i]='4';
break;
case 5:
hexstring[i]='5';
break;
case 6:
hexstring[i]='6';
break;
case 7:
hexstring[i]='7';
break;
case 8:
hexstring[i]='8';
break;
case 9:
hexstring[i]='9';
break;
}
}
for(int i=0;i<runtime;i++){
	d[runtime-1-i]=hexstring[i];
}
return d;
}


char *substring(char *s1, uint16_t len1, uint16_t len2){//��ȡһ���ַ������õ��Ӵ�,��������ͨ��
	//urt1SendStr((uint8_t*)s1,strlen(s1));
	
static char  result_str[128];
	
	for(uint8_t iii=len1;iii<len2;iii++){
	//	urt1SendChar(iii);
		result_str[iii-len1]=s1[iii];
	}
	
return  result_str;
}

void nb_initial(void){//bc28�ĳ�ʼ������
	for(int ii=0;ii<3;ii++){
		HAL_Delay(400);//���ҼӸ�delay��֮���ö�ʱ��
urt2SendStr(nb_initial_seq[ii],strlen((char*)nb_initial_seq[ii]));
urt2SendChar(0x0D);
urt2SendChar(0x0A);	
	}
}

void nb_AT(void){
	uint8_t tmp_cmd[3]={"AT"};
	
	urt2SendStr(tmp_cmd,2);
urt2SendChar(0x0D);
urt2SendChar(0x0A);	
}

void nb_Get_IMEI(void){//��ȡIMEI��ֵ
	uint8_t tmp_cmd[9]={"AT+CGSN=1"};
	
	urt2SendStr(tmp_cmd,9);
urt2SendChar(0x0D);
urt2SendChar(0x0A);	
}

void nb_Get_CSQ(void){//��ȡCSQ�ź�ֵ
	uint8_t tmp_cmd[6]={"AT+CSQ"};
	
	urt2SendStr(tmp_cmd,6);
urt2SendChar(0x0D);
urt2SendChar(0x0A);	
}

void nb_Get_CIMI(void){//��ȡ����

	uint8_t tmp_cmd[7]={"AT+CIMI"};
	
	urt2SendStr(tmp_cmd,7);
urt2SendChar(0x0D);
urt2SendChar(0x0A);	
}


void nb_Get_Operator(void){//��ȡ��Ӫ��
	uint8_t tmp_cmd[9]={"AT+NBAND?"};
	urt2SendStr(tmp_cmd,9);
urt2SendChar(0x0D);
urt2SendChar(0x0A);	
}

void nb_Test_Report(void){//�����ϱ�

	uint8_t tmp_cmd[45]={"AT+NMGS=17,000100011A00010111009C00A802B9007D"};
	
	urt2SendStr(tmp_cmd,45);
urt2SendChar(0x0D);
urt2SendChar(0x0A);	
}
