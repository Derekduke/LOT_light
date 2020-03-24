#include "board.h"
#include "uart.h"
#include "FM25VN10.h"

extern SPI_HandleTypeDef hspi2;


union LONG_BYTE tmp;
uint8_t sRx[512];
uint8_t sTx[512];

//spi2��д��������
void FMSPI_TransmitReceive(uint8_t *sTx, uint8_t *sRx,uint16_t number)
{
	SPI2_CS_0;
			HAL_SPI_TransmitReceive(&hspi2,sTx,sRx,number,100);
	SPI2_CS_1;
}

//��SPI_FM״̬�Ĵ���
uint8_t SPI_FM_ReadSR(void)   
{   
		sTx[0]=FM25_READSTATUSREG; 
    FMSPI_TransmitReceive(sTx,sRx,2);   //���Ͷ�ȡ״̬�Ĵ�������    
    return sRx[1];   
} 

//дSPI_FM״̬�Ĵ���
void SPI_FM_Write_SR(uint8_t sr)   
{ 
    sTx[0]=FM25_WRITESTATUSREG; 
		sTx[1]=sr;
    FMSPI_TransmitReceive(sTx,sRx,2); 	//����дȡ״̬�Ĵ�������,��д��sr    
}  

//SPI_FMдʹ�� 
void SPI_FM_Write_Enable(void)   
{
		sTx[0]=FM25_WRITEENABLE;    
		FMSPI_TransmitReceive(sTx,sRx,1);	//����дʹ��  
} 

//SPI_FMд��ֹ       
void SPI_FM_Write_Disable(void)   
{  
    sTx[0]=FM25_WRITEDISABLE;
		FMSPI_TransmitReceive(sTx,sRx,1);//����д��ָֹ��    
}  


//��ȡоƬID W25X16��ID:0XEF14
uint8_t SPI_FM_ReadID(uint8_t* pBuffer)
{
		sTx[0]=FM25_READID;	
		FMSPI_TransmitReceive(sTx,pBuffer,10);//���Ͷ�ȡID���� 
	  if(pBuffer[6]==0x7F && pBuffer[7]==0xC2) return 1; else return 0;
} 

	
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)

void SPI_FM_Read(uint8_t* pBuffer,uint16_t ReadAddr,uint16_t  NumByteToRead)   
{  
		tmp.i[0]=ReadAddr;	
		sTx[0]=FM25_READDATA;
		sTx[1]=tmp.b[1];
		sTx[2]=tmp.b[0];
		
		SPI2_CS_0;                           //ʹ������   	
		HAL_SPI_Transmit(&hspi2,sTx,3,100);//���Ͷ�ȡ�����24bit��ַ
		HAL_SPI_Receive(&hspi2,pBuffer,NumByteToRead,100);
    SPI2_CS_1;  	
	//ȡ��Ƭѡ     	      
}  

void SPI_FM_Write(uint8_t* pBuffer,uint16_t WriteAddr,uint16_t  NumByteToWrite)
{  
    
		SPI_FM_Write_Enable();                  //SET WEL 
	
		tmp.i[0]=WriteAddr;
		sTx[0]=FM25_WRITEDATA;
		sTx[1]=tmp.b[1];
		sTx[2]=tmp.b[0];
		

		SPI2_CS_0; 	//ʹ������   
		HAL_SPI_Transmit(&hspi2,sTx,3,100);//����д���� 17λ��ַ 
		HAL_SPI_Transmit(&hspi2,pBuffer,NumByteToWrite,100);//ѭ��д�� 	
    SPI2_CS_1; //ȡ��Ƭѡ 
	
		SPI_FM_Write_Disable();                  //SET WEL 
} 
