#include "board.h"
#include "uart.h"
#include "FM25VN10.h"

extern SPI_HandleTypeDef hspi2;


union LONG_BYTE tmp;
uint8_t sRx[512];
uint8_t sTx[512];

//spi2读写基本函数
void FMSPI_TransmitReceive(uint8_t *sTx, uint8_t *sRx,uint16_t number)
{
	SPI2_CS_0;
			HAL_SPI_TransmitReceive(&hspi2,sTx,sRx,number,100);
	SPI2_CS_1;
}

//读SPI_FM状态寄存器
uint8_t SPI_FM_ReadSR(void)   
{   
		sTx[0]=FM25_READSTATUSREG; 
    FMSPI_TransmitReceive(sTx,sRx,2);   //发送读取状态寄存器命令    
    return sRx[1];   
} 

//写SPI_FM状态寄存器
void SPI_FM_Write_SR(uint8_t sr)   
{ 
    sTx[0]=FM25_WRITESTATUSREG; 
		sTx[1]=sr;
    FMSPI_TransmitReceive(sTx,sRx,2); 	//发送写取状态寄存器命令,并写入sr    
}  

//SPI_FM写使能 
void SPI_FM_Write_Enable(void)   
{
		sTx[0]=FM25_WRITEENABLE;    
		FMSPI_TransmitReceive(sTx,sRx,1);	//发送写使能  
} 

//SPI_FM写禁止       
void SPI_FM_Write_Disable(void)   
{  
    sTx[0]=FM25_WRITEDISABLE;
		FMSPI_TransmitReceive(sTx,sRx,1);//发送写禁止指令    
}  


//读取芯片ID W25X16的ID:0XEF14
uint8_t SPI_FM_ReadID(uint8_t* pBuffer)
{
		sTx[0]=FM25_READID;	
		FMSPI_TransmitReceive(sTx,pBuffer,10);//发送读取ID命令 
	  if(pBuffer[6]==0x7F && pBuffer[7]==0xC2) return 1; else return 0;
} 

	
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)

void SPI_FM_Read(uint8_t* pBuffer,uint16_t ReadAddr,uint16_t  NumByteToRead)   
{  
		tmp.i[0]=ReadAddr;	
		sTx[0]=FM25_READDATA;
		sTx[1]=tmp.b[1];
		sTx[2]=tmp.b[0];
		
		SPI2_CS_0;                           //使能器件   	
		HAL_SPI_Transmit(&hspi2,sTx,3,100);//发送读取命令和24bit地址
		HAL_SPI_Receive(&hspi2,pBuffer,NumByteToRead,100);
    SPI2_CS_1;  	
	//取消片选     	      
}  

void SPI_FM_Write(uint8_t* pBuffer,uint16_t WriteAddr,uint16_t  NumByteToWrite)
{  
    
		SPI_FM_Write_Enable();                  //SET WEL 
	
		tmp.i[0]=WriteAddr;
		sTx[0]=FM25_WRITEDATA;
		sTx[1]=tmp.b[1];
		sTx[2]=tmp.b[0];
		

		SPI2_CS_0; 	//使能器件   
		HAL_SPI_Transmit(&hspi2,sTx,3,100);//发送写命令 17位地址 
		HAL_SPI_Transmit(&hspi2,pBuffer,NumByteToWrite,100);//循环写数 	
    SPI2_CS_1; //取消片选 
	
		SPI_FM_Write_Disable();                  //SET WEL 
} 
