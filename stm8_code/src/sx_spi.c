#include "stm8l15x.h"
#include "sx_board.h"

uint8_t spiRecvNum;
/******************************************************************************
**鍑芥暟鍚嶇О锛歷oid UART1_Init(unsigned int baudrate)
**鍔熻兘鎻忚堪锛氬垵濮嬪寲USART妯″潡
**鍏ュ彛鍙傛暟锛歶nsigned int baudrate  -> 璁剧疆涓插彛娉㈢壒鐜?**杈撳嚭锛氭棤
******************************************************************************/
void SPI1_Init(void)
{
  
   SPI_DeInit(SPI1);
 //SPI_CLOCK:PB5, SPI_MOSI: PB6, SPI_MISO: PB7	
  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);	
  GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);	
  //
  GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);

  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);

  SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_4, 
           SPI_Mode_Master,SPI_CPOL_High, SPI_CPHA_2Edge,
           SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft, 0x07);
  
  //SPI_ITConfig(SPI1, SPI_IT_RXNE, ENABLE); //SPI 接收中断使能
  SPI_Cmd(SPI1, ENABLE);	/*  */
  
  /*  */
  GPIO_Init(GPIOB , GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
  SPI_CS1;		/*  */
}



 /*******************************************************************************
****函数名称:
****函数功能:发送字节
****版本:V1.0
****日期:14-2-2014
****入口参数:无
****出口参数:SPI接收到的数据
****说明:
********************************************************************************/

unsigned char SPIReadWriteByte(unsigned char byte)
{
    u16 nCount = 0;
  
     while (SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) == RESET)
    {
        nCount++;
        if(nCount > 390)
        {
            return 0;
        }
    }
    nCount = 0;
  
    SPI_SendData(SPI1,byte);
    while (SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE) == RESET)
    {
        nCount++;
        if(nCount > 390)
        {
            return 0;
        }
    }
    return SPI_ReceiveData(SPI1);	 
}
/**********************************************************
**Name:     SPICmd8bit
**Function: SPI Write one byte
**Input:    WrPara
**Output:   none
**note:     use for burst mode
**********************************************************/
void SPICmd8bit(u8 WrPara)
{
    //u8 retry=0;	
    SPI_CS0; 
    SPIReadWriteByte(WrPara);
}
/**********************************************************
**Name:     SPIRead8bit
**Function: SPI Read one byte
**Input:    None
**Output:   result byte
**Note:     use for burst mode
**********************************************************/
u8 SPIRead8bit(void)
{
    u8 temp=0;
    SPI_CS0; 
    temp = SPIReadWriteByte(0);	
    return temp;	
}
    
/**********************************************************
**Name:     SPIRead
**Function: SPI Read CMD
**Input:    adr -> address for read
**Output:   None
**********************************************************/
void SX1278Read(u8 addr, u8 *data)
{    
    SPI_CS0; 
    SPIReadWriteByte(addr); 
    data[0] = SPIReadWriteByte(0);
    while( SPI_GetFlagStatus(SPI1,SPI_FLAG_BSY) == SET);
    SPI_CS1;    
}

/**********************************************************
**Name:     SPIWrite
**Function: SPI Write CMD
**Input:    WrPara -> address & data
**Output:   None
**********************************************************/
void SX1278Write(u8 addr, u8 data)
{           
    SPICmd8bit(addr|0x80);       
    SPICmd8bit(data);  
    while( SPI_GetFlagStatus(SPI1,SPI_FLAG_BSY) == SET);   //?D??SPIê?·??|
     SPI_CS1;       
}
/**********************************************************
**Name:     SPIBurstRead
**Function: SPI burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many bytes for read
**Output:   None
**********************************************************/
void SX1278ReadFifo( uint8_t *buffer, uint8_t size )
{
    u8 i;
    if(size<=1)                                            //length must more than one
        return;
    else
    {
        SPICmd8bit(0); 
        for(i=0;i<size;i++)
        buffer[i] = SPIRead8bit();
        while(SPI_GetFlagStatus(SPI1,SPI_FLAG_BSY) == SET);
         SPI_CS1;       
    }
}
/**********************************************************
**Name:     SPIBurstWrite
**Function: SPI burst write mode
**Input:    adr-----address for write
**          ptr-----data buffer point for write
**          length--how many bytes for write
**Output:   none
**********************************************************/
void SX1278WriteFifo( uint8_t *buffer, uint8_t size )
{ 
    u8 i;
    if(size<=1)                                            //length must more than one
        return;
    else  
    {      
        SPICmd8bit(0x80);
        for(i=0;i<size;i++)
            SPICmd8bit(buffer[i]);
        while( SPI_GetFlagStatus(SPI1,SPI_FLAG_BSY) == SET);
         SPI_CS1;  
    }
}

