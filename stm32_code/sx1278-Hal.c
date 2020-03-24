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
函数名称 ： SX127x硬件层函数，SX1278初始化重置
功    能 ： SX1278重启引脚初始化加输出
参    数 ： RESET 输出的电平高低
返 回 值 ： 
作    者 ： sun
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
函数名称 ： SX127x硬件层函数，SX1278 SPI写寄存器功能实现
功    能 ： SX1278 SPI写寄存器功能实现
参    数 ： addr：寄存器地址  buffer 待写入值    size ：写入数据的长度
返 回 值 ： 无
作    者 ： sun
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
函数名称 ： SX127x硬件层函数，SX1278 SPI读寄存器功能实现
功    能 ： SX1278 SPI读寄存器功能实现
参    数 ： addr：所读寄存器地址  buffer 返回值    size ：读取数据长度
返 回 值 ： 无
作    者 ： sun
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

