#ifndef __FM25VN10_h
#define __FM25VN10_h
#include "main.h"

////////////////////////////////////////////////////////////////////////////
//F-RAM F25L16-G  organized as 2048 �� 8
////////////////////////////////////////////////////////////////////////////
//ID: 7F 7F 7F 7F 7F 7F C2 24 01 
////////////////////////////////////////////////////////////////////////////
//ָ���
#define FM25_WRITEENABLE        0x06 
#define FM25_WRITEDISABLE       0x04 
#define FM25_READSTATUSREG      0x05 
#define FM25_WRITESTATUSREG     0x01 
#define FM25_READDATA           0x03 
#define FM25_FASTREADDATA       0x0B 
#define FM25_WRITEDATA          0x02 
#define FM25_POWERDOWN          0xB9 
#define FM25_READID     				0x9F 
#define FM25_READSN						  0xC3 
///////////////////////////////////////////////////////
uint8_t SPI_FM_ReadID(uint8_t* pBuffer);         //��ȡFLASH ID
uint8_t SPI_FM_ReadSR(void);         //��ȡ״̬�Ĵ��� 
void SPI_FM_Write_SR(uint8_t  sr);      //д״̬�Ĵ���
void SPI_FM_Write_Enable(void);         //дʹ�� 
void SPI_FM_Write_Disable(void);        //д����
void SPI_FM_Read(uint8_t  * pBuffer,uint16_t ReadAddr,uint16_t  NumByteToRead);   //��ȡflash
void SPI_FM_Write(uint8_t  * pBuffer,uint16_t WriteAddr,uint16_t  NumByteToWrite);//д��flash

#endif


