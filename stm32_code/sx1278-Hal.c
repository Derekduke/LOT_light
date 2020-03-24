/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) SEMTECH S.A.
 */

#include "board.h"
#include "sx1278-Hal.h"

extern SPI_HandleTypeDef hspi1;
uint8_t sRx1[256];
uint8_t sTx1[256];


/************************************************
�������� �� SX127xӲ���㺯����SX1278��ʼ������
��    �� �� SX1278�������ų�ʼ�������
��    �� �� RESET ����ĵ�ƽ�ߵ�
�� �� ֵ �� 
��    �� �� sun
*************************************************/

void SX1278Write( uint8_t addr, uint8_t data )
{
    SX1278WriteBuffer( addr, &data, 1 );
}

void SX1278Read( uint8_t addr, uint8_t *data )
{
    SX1278ReadBuffer( addr, data, 1 );
}
/************************************************
�������� �� SX127xӲ���㺯����SX1278 SPIд�Ĵ�������ʵ��
��    �� �� SX1278 SPIд�Ĵ�������ʵ��
��    �� �� addr���Ĵ�����ַ  buffer ��д��ֵ    size ��д�����ݵĳ���
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void SX1278WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
 uint8_t ii; 
 for(ii=0;ii<size;ii++) sTx1[ii+1]=buffer[ii];
 sTx1[0]=( addr | 0x80 );
 SPI1_CS_0;
		HAL_SPI_TransmitReceive(&hspi1,sTx1,sRx1,size+1,100);		
	SPI1_CS_1;			
}

/************************************************
�������� �� SX127xӲ���㺯����SX1278 SPI���Ĵ�������ʵ��
��    �� �� SX1278 SPI���Ĵ�������ʵ��
��    �� �� addr�������Ĵ�����ַ  buffer ����ֵ    size ����ȡ���ݳ���
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void SX1278ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
	uint8_t i;
  sTx1[0]=( addr & 0x7F );
  SPI1_CS_0;
		HAL_SPI_TransmitReceive(&hspi1,sTx1,sRx1,size+1,100);		
	SPI1_CS_1;		
	for(i=0;i<size;i++){buffer[i]=sRx1[i+1];}		
}

void SX1278WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1278WriteBuffer( 0, buffer, size );
}

void SX1278ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1278ReadBuffer( 0, buffer, size );
}

