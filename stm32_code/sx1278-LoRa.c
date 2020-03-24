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
/*!
 * \file       sx1278-LoRa.c
 * \brief      SX1278 RF chip driver mode LoRa
 *
 * \version    2.0.B2
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include "board.h"
#include <string.h>
#include <stdlib.h>


//#if defined( USE_SX1278_RADIO )

//#include "radio.h"

#include "sx1278-Hal.h"
#include "sx1278.h"

#include "sx1278-LoRaMisc.h"
#include "sx1278-LoRa.h"

#define RSSI_OFFSET_LF                              -155.0
#define RSSI_OFFSET_HF                              -150.0
#define NOISE_ABSOLUTE_ZERO                         -174.0
#define NOISE_FIGURE_LF                                4.0
#define NOISE_FIGURE_HF                                6.0

STRU_SX1278LR SX1278LR;

/*!
 * Precomputed signal bandwidth log values
 * Used to compute the Packet RSSI value.
 */
const double SignalBwLog[] =
{
    3.8927900303521316335038277369285,  // 7.8 kHz
    4.0177301567005500940384239336392,  // 10.4 kHz
    4.193820026016112828717566631653,   // 15.6 kHz
    4.31875866931372901183597627752391, // 20.8 kHz
    4.4948500216800940239313055263775,  // 31.2 kHz
    4.6197891057238405255051280399961,  // 41.6 kHz
    4.795880017344075219145044421102,   // 62.5 kHz
    5.0969100130080564143587833158265,  // 125 kHz
    5.397940008672037609572522210551,   // 250 kHz
    5.6989700043360188047862611052755   // 500 kHz
};

const double RssiOffsetLF[] =
{   // These values need to be specify in the Lab
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
};

const double RssiOffsetHF[] =
{   // These values need to be specify in the Lab
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
};

/*!
 * Frequency hopping frequencies table
 */
const int32_t HoppingFrequencies[] =
{
    916500000,
    923500000,
    906500000,
    917500000,
    917500000,
    909000000,
    903000000,
    916000000,
    912500000,
    926000000,
    925000000,
    909500000,
    913000000,
    918500000,
    918500000,
    902500000,
    911500000,
    926500000,
    902500000,
    922000000,
    924000000,
    903500000,
    913000000,
    922000000,
    926000000,
    910000000,
    920000000,
    922500000,
    911000000,
    922000000,
    909500000,
    926000000,
    922000000,
    918000000,
    925500000,
    908000000,
    917500000,
    926500000,
    908500000,
    916000000,
    905500000,
    916000000,
    903000000,
    905000000,
    915000000,
    913000000,
    907000000,
    910000000,
    926500000,
    925500000,
    911000000,
};

// Default settings
STRU_LoRaSettings LoRaSettings =
{
    435000000,        // RFFrequency
    20,               // Power
    8,                // SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                      // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    8,               	// SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    2,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    true,             // CrcOn [0: OFF, 1: ON]
    false,            // mplicitHeaderOn [0: OFF, 1: ON]
    1,                // RxSingleOn [0: Continuous, 1 Single]
    0,                // FreqHopOn [0: OFF, 1: ON]
    4,                // HopPeriod Hops every frequency hopping period symbols
    100,              // TxPacketTimeout
    100,              // RxPacketTimeout
    128,              // PayloadLength (used for implicit header mode)
};

/*!
 * SX1278 LoRa registers variable
 */

/*!
 * Local RF buffer for communication support
 */
static uint8_t RFBuffer[RF_BUFFER_SIZE];
static uint8_t TFBuffer[RF_BUFFER_SIZE];
//uint8_t RFBuffer[RF_BUFFER_SIZE];

/*!
 * RF state machine variable
 */
static uint8_t RFLRState = RFLR_STATE_IDLE;

/*!
 * Rx management support variables
 */
static uint16_t RxPacketSize = 0;
static int8_t RxPacketSnrEstimate;
static double RxPacketRssiValue;
static uint8_t RxGain = 1;
static uint32_t RxTimeoutTimer = 0;
/*!
 * PacketTimeout Stores the Rx window time value for packet reception
 */
static uint32_t PacketTimeout;

/*!
 * Tx management support variables
 */
static uint16_t TxPacketSize = 0;
extern uint8_t SX1278Regs[0x71];
/************************************************
函数名称 ： SX1278LoRaInit
功    能 ： Lora初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1278LoRaInit( void )
{
    RFLRState = RFLR_STATE_IDLE;
    SX1278Read( REG_LR_VERSION, &SX1278LR.RegVersion );//读取芯片的版本号	
    SX1278ReadBuffer( 0, SX1278Regs, 0x71); //将芯片内容寄存器值读到结构体中

    SX1278LR.RegLna = RFLR_LNA_GAIN_G1;		
    SX1278Write( REG_LR_LNA, SX1278LR.RegLna); //增益设置为最大G1
    // set the RF settings
    SX1278LoRaSetRFFrequency();																		//频率设置
    SX1278LoRaSetSpreadingFactor( LoRaSettings.SpreadingFactor ); // 扩频因子设置
    SX1278LoRaSetErrorCoding( LoRaSettings.ErrorCoding );					//循环纠错码
    SX1278LoRaSetPacketCrcOn( LoRaSettings.CrcOn );								//校验
    SX1278LoRaSetSignalBandwidth( LoRaSettings.SignalBw );				//带宽
    SX1278LoRaSetImplicitHeaderOn( LoRaSettings.ImplicitHeaderOn );//前导
    SX1278LoRaSetSymbTimeout( 0x3FF );														//超时时间
    SX1278LoRaSetPayloadLength( LoRaSettings.PayloadLength );			//最大负载长度
		//注意，无头模式（Implicit Header）时，必须提前规定好收发双方的PL
    SX1278LoRaSetLowDatarateOptimize( true );											//低速优化
		
		SX1278LoRaSetPaRamp(RFLR_PARAMP_0100_US);
		//  ↑PA Ramp的时间，如果用户LDO不能快速输出大电流（泵能力），适当增加PA Ramp时间
    //  ↑如果Ramp时间过短超过了LDO的能力时，会出现进入TX后，系统电流为发射电流，但是RF信号不出现的现象
		
		
		 SX1278Write( REG_LR_OCP,0x20|RFLR_OCP_TRIM_240_MA);//电流过载保护 Over Current Protection
		 
		if( LoRaSettings.RFFrequency > 380000000 )  //860000000
    {
        SX1278LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST ); //选择 PA_BOOST 管脚输出信号
//        SX1278LoRaSetPa20dBm( true );  //最大输出功率
        LoRaSettings.Power = 20;
        SX1278LoRaSetRFPower( LoRaSettings.Power );
    }
    else
    {
        SX1278LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_RFO );
//        SX1278LoRaSetPa20dBm( false );
        LoRaSettings.Power = 14;
        SX1278LoRaSetRFPower( LoRaSettings.Power );
    } 
			
    SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );
}


/************************************************
函数名称 ： SX1278LoraConfigCheck
功    能 ： Lora 初始化信息检查，防止配置信息出错
参    数 ： 无
返 回 值 ： bool 成功为1  失败为0
作    者 ： sun
*************************************************/

bool SX1278LoraConfigCheck(void)
{
		if((LoRaSettings.RFFrequency <137000000)||(LoRaSettings.RFFrequency >525000000))
        return false;
		if(LoRaSettings.SignalBw > 9)
			return false;
		if((LoRaSettings.SpreadingFactor < 6)||(LoRaSettings.SpreadingFactor >12))
        return false;
		if((LoRaSettings.ErrorCoding < 1)||(LoRaSettings.ErrorCoding >4))
        return false;	
		if(LoRaSettings.PayloadLength  > 128)
			return false;
		return true ;
}


/************************************************
函数名称 ： SX1278LoRaReset
功    能 ： Lora芯片重启
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1278LoRaReset( void )
{
   uint32_t startTick;
   SX_RESET_0;
    // Wait 1ms
    startTick = HAL_GetTick();
    while( (HAL_GetTick() - startTick ) < 1 ); //delay 1ms
    SX_RESET_1;
    // Wait 6ms
    startTick =  HAL_GetTick();
    while( ( HAL_GetTick() - startTick ) < 6 );
}

/************************************************
函数名称 ： SX1278LoRaSetOpMode
功    能 ： 设置Lora工作模式
参    数 ： opMode  运行模式
						
返 回 值 ： 无
作    者 ： sun
*************************************************/

void SX1278LoRaSetOpMode( uint8_t opMode )
{
    if( opMode != (SX1278LR.RegOpMode & 0x07))
    {
        SX1278LR.RegOpMode = ( SX1278LR.RegOpMode & RFLR_OPMODE_MASK ) | opMode;
				SX1278Write( REG_LR_OPMODE, SX1278LR.RegOpMode );
    }
}


/************************************************
函数名称 ： SX1278LoRaGetOpMode
功    能 ： 读取当前Lora工作模式
参    数 ： 无					
返 回 值 ： 当前工作模式
作    者 ： sun
*************************************************/
uint8_t SX1278LoRaGetOpMode( void )
{
    SX1278Read( REG_LR_OPMODE, &SX1278LR.RegOpMode );
    return SX1278LR.RegOpMode & ~RFLR_OPMODE_MASK;
}

uint8_t SX1278LoRaReadRxGain( void )
{
    SX1278Read( REG_LR_LNA, &SX1278LR.RegLna );
    return( SX1278LR.RegLna >> 5 ) & 0x07;
}

/************************************************
函数名称 ： SX1278LoRaReadRssi
功    能 ： 读取计算Lora信号强度
参    数 ： 无
						
返 回 值 ： 返回值 为信号强度
作    者 ： sun
*************************************************/
double SX1278LoRaReadRssi( void )
{
    // Reads the RSSI value
    SX1278Read( REG_LR_RSSIVALUE, &SX1278LR.RegRssiValue );

    if( LoRaSettings.RFFrequency < 860000000 )  // LF
    {
        return RssiOffsetLF[LoRaSettings.SignalBw] + ( double )SX1278LR.RegRssiValue;
    }
    else
    {
        return RssiOffsetHF[LoRaSettings.SignalBw] + ( double )SX1278LR.RegRssiValue;
    }
}
/************************************************
函数名称 ： SX1278LoRaGetPacketSnr
功    能 ： 获取Lora信噪比
参    数 ： 无
						
返 回 值 ： 返回值 信噪比
作    者 ： sun
*************************************************/
uint8_t SX1278LoRaGetPacketRxGain( void )
{
    return RxGain;
}

int8_t SX1278LoRaGetPacketSnr( void )
{
    return RxPacketSnrEstimate;
}
/************************************************
函数名称 ： SX1278LoRaGetPacketRssi
功    能 ： 读Lora信号强度
参    数 ： 无
						
返 回 值 ： 返回值 为信号强度
作    者 ： sun
*************************************************/
double SX1278LoRaGetPacketRssi( void )
{
    return RxPacketRssiValue;
}
/************************************************
函数名称 ： SX1278LoRaStartRx
功    能 ： Lora开始接收
参    数 ： 无
						
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1278LoRaStartRx( void )
{
    SX1278LoRaSetRFState( RFLR_STATE_RX_INIT );
}
/************************************************
函数名称 ： SX1278LoRaGetRxPacket
功    能 ： Lora获取接收数据
参    数 ： buffer 接收缓存器
						接收到的数据长度
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1278LoRaGetRxPacket( void *buffer, uint16_t *size )
{
		if(RxPacketSize<1){*size=0; return;}
		memcpy(( void * )buffer, ( void * )RFBuffer, ( size_t )RxPacketSize);
		*size = RxPacketSize;
		RxPacketSize = 0;		
}
/************************************************
函数名称 ： SX1278LoRaSetTxPacket
功    能 ： Lora开始发送
参    数 ： buffer 发送缓存器
						size   发送的数据长度
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1278LoRaSetTxPacket( const void *buffer, uint16_t size )
{
    if( LoRaSettings.FreqHopOn == false )
    {
        TxPacketSize = size;
    }
    else
    {
        TxPacketSize = 255;
    }
    memcpy( ( void * )TFBuffer, buffer, ( size_t )TxPacketSize );

    RFLRState = RFLR_STATE_TX_INIT;
}

/************************************************
函数名称 ： SX1278LoRaGetRFState
功    能 ： 读取工作状态
参    数 ： 无
						
返 回 值 ： 工作状态
						RFLR_STATE_IDLE,
						RFLR_STATE_RX_INIT,
						RFLR_STATE_RX_RUNNING,
						RFLR_STATE_RX_DONE,
						RFLR_STATE_RX_TIMEOUT,
						RFLR_STATE_TX_INIT,
						RFLR_STATE_TX_RUNNING,
						RFLR_STATE_TX_DONE,
						RFLR_STATE_TX_TIMEOUT,
						RFLR_STATE_CAD_INIT,
						RFLR_STATE_CAD_RUNNING
作    者 ： sun
*************************************************/

uint8_t SX1278LoRaGetRFState( void )
{
    return RFLRState;
}

/************************************************
函数名称 ： SX1278LoRaSetRFState
功    能 ： 设置工作状态
参    数 ： 工作状态
						
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1278LoRaSetRFState( uint8_t state )
{
    RFLRState = state;
}


/*!
 * \brief Process the LoRa modem Rx and Tx state machines depending on the
 *        SX1278 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY,
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
  /************************************************
函数名称 ： SX1278LoRaProcess
功    能 ： Lora运行
参    数 ： 无					
返 回 值 ： Lora的工作模式
						RF_IDLE,
						RF_BUSY,
						RF_RX_DONE,
						RF_RX_TIMEOUT,
						RF_RX_ID_ERROR,
						RF_TX_DONE,
						RF_TX_TIMEOUT,
						RF_LEN_ERROR,
						RF_CHANNEL_EMPTY,
						RF_CHANNEL_ACTIVITY_DETECTED
作    者 ： sun
*************************************************/
uint32_t SX1278LoRaProcess( void )
{
    uint32_t result = RF_BUSY;
    
    switch( RFLRState )
    {
    case RFLR_STATE_IDLE:
        break;
    case RFLR_STATE_RX_INIT:
        
        SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        SX1278LR.RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    //RFLR_IRQFLAGS_RXDONE |
                                    //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    RFLR_IRQFLAGS_CADDONE |
                                    //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                    RFLR_IRQFLAGS_CADDETECTED;
        SX1278Write( REG_LR_IRQFLAGSMASK, SX1278LR.RegIrqFlagsMask );

        if( LoRaSettings.FreqHopOn == true )
        {
            SX1278LR.RegHopPeriod = LoRaSettings.HopPeriod;

            SX1278Read( REG_LR_HOPCHANNEL, &SX1278LR.RegHopChannel );
            //SX1278LoRaSetRFFrequency( HoppingFrequencies[SX1278LR.RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
        }
        else
        {
            SX1278LR.RegHopPeriod = 255;
        }
        
        SX1278Write( REG_LR_HOPPERIOD, SX1278LR.RegHopPeriod );
                
                                    // RxDone                    RxTimeout                   FhssChangeChannel           CadDone
        SX1278LR.RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                    // CadDetected               ModeReady
        SX1278LR.RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
		
        SX1278WriteBuffer( REG_LR_DIOMAPPING1, &SX1278LR.RegDioMapping1, 2 );
    
        if( LoRaSettings.RxSingleOn == true ) // Rx single mode
        {

            SX1278LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
        }
        else // Rx continuous mode
        {
            SX1278LR.RegFifoAddrPtr = SX1278LR.RegFifoRxBaseAddr;
            SX1278Write( REG_LR_FIFOADDRPTR, SX1278LR.RegFifoAddrPtr );
            
            SX1278LoRaSetOpMode( RFLR_OPMODE_RECEIVER );
        }
        
        //memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );  //清空RFBuffer

        PacketTimeout = LoRaSettings.RxPacketTimeout;
        RxTimeoutTimer = HAL_GetTick();
        RFLRState = RFLR_STATE_RX_RUNNING;
        break;
    case RFLR_STATE_RX_RUNNING:
        
        SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
				if( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_RXDONE_MASK )
        {
            RxTimeoutTimer = HAL_GetTick();
            if( LoRaSettings.FreqHopOn == true )    //false
            {
                SX1278Read( REG_LR_HOPCHANNEL, &SX1278LR.RegHopChannel );
                //SX1278LoRaSetRFFrequency( HoppingFrequencies[SX1278LR.RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
            }
            // Clear Irq
            SX1278Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
            RFLRState = RFLR_STATE_RX_DONE;
        }
        	SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
				if( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL_MASK )// FHSS Changed Channel
        {
            RxTimeoutTimer = HAL_GetTick();
            if( LoRaSettings.FreqHopOn == true )
            {
                SX1278Read( REG_LR_HOPCHANNEL, &SX1278LR.RegHopChannel );
                //SX1278LoRaSetRFFrequency( HoppingFrequencies[SX1278LR.RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
            }
            // Clear Irq
            SX1278Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
            // Debug
            RxGain = SX1278LoRaReadRxGain( );
        }

        if( LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            if( ( HAL_GetTick() - RxTimeoutTimer ) > PacketTimeout )   //计算是否超时
            {
                RFLRState = RFLR_STATE_RX_TIMEOUT;
            }
        }
        break;
    case RFLR_STATE_RX_DONE:
        SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
        if( ( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )     //CRC校验
        {
            // Clear Irq
            SX1278Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR  );            
            // Rx single mode
            {
                RFLRState = RFLR_STATE_RX_INIT;
            }
            break;
        }				
				// If CRC off, then give up the packet.
				// This means it receives wrong header.
       
        //估计
				{
            uint8_t rxSnrEstimate;
            SX1278Read( REG_LR_PKTSNRVALUE, &rxSnrEstimate );
            if( rxSnrEstimate & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;
                RxPacketSnrEstimate = -RxPacketSnrEstimate;
            }
            else
            {
                // Divide by 4
                RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;
            }
        }
        
        // 频率小于860M
        {    
            if( RxPacketSnrEstimate < 0 )
            {
                RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + NOISE_FIGURE_LF + ( double )RxPacketSnrEstimate;
            }
            else
            {    
                SX1278Read( REG_LR_PKTRSSIVALUE, &SX1278LR.RegPktRssiValue );
                RxPacketRssiValue = RssiOffsetLF[LoRaSettings.SignalBw] + ( double )SX1278LR.RegPktRssiValue;
            }
        }
        
        // Rx single mode
        {
            SX1278LR.RegFifoAddrPtr = SX1278LR.RegFifoRxBaseAddr;       
            SX1278Write( REG_LR_FIFOADDRPTR, SX1278LR.RegFifoAddrPtr );    //读取接收数据

            if( LoRaSettings.ImplicitHeaderOn == true )
            {
                RxPacketSize = SX1278LR.RegPayloadLength;
                SX1278ReadFifo( RFBuffer, SX1278LR.RegPayloadLength );
            }
            else
            {
                SX1278Read( REG_LR_NBRXBYTES, &SX1278LR.RegNbRxBytes );
                RxPacketSize = SX1278LR.RegNbRxBytes;
                SX1278ReadFifo( RFBuffer, SX1278LR.RegNbRxBytes );									
            }
        }
        
        // Rx single mode
        {
            RFLRState = RFLR_STATE_RX_INIT;
        }
        result = RF_RX_DONE;
        break;
    case RFLR_STATE_RX_TIMEOUT:
        RFLRState = RFLR_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;
    case RFLR_STATE_TX_INIT:

        SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );

  
        {//非跳频模式
            SX1278LR.RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            SX1278LR.RegHopPeriod = 0;
        }
        SX1278Write( REG_LR_HOPPERIOD, SX1278LR.RegHopPeriod );
        SX1278Write( REG_LR_IRQFLAGSMASK, SX1278LR.RegIrqFlagsMask );

        // Initializes the payload size
        SX1278LR.RegPayloadLength = TxPacketSize;
        SX1278Write( REG_LR_PAYLOADLENGTH, SX1278LR.RegPayloadLength );
        
        SX1278LR.RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
        SX1278Write( REG_LR_FIFOTXBASEADDR, SX1278LR.RegFifoTxBaseAddr );

        SX1278LR.RegFifoAddrPtr = SX1278LR.RegFifoTxBaseAddr;
        SX1278Write( REG_LR_FIFOADDRPTR, SX1278LR.RegFifoAddrPtr );
        
        // Write payload buffer to LORA modem
        SX1278WriteFifo( TFBuffer, SX1278LR.RegPayloadLength );
		
                                        // TxDone               RxTimeout                   FhssChangeChannel          ValidHeader         
        SX1278LR.RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
                                        // PllLock              Mode Ready
        SX1278LR.RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
		
        SX1278WriteBuffer( REG_LR_DIOMAPPING1, &SX1278LR.RegDioMapping1, 2 );

        SX1278LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );

        RFLRState = RFLR_STATE_TX_RUNNING;
        break;
    case RFLR_STATE_TX_RUNNING:
				{
				 SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
				 if( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_TXDONE_MASK )
					{
							// Clear Irq
							SX1278Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );
							RFLRState = RFLR_STATE_TX_DONE;   
					}
				 SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
				}
        break;
    case RFLR_STATE_TX_DONE:
        // optimize the power consumption by switching off the transmitter as soon as the packet has been sent
        SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );
        RFLRState = RFLR_STATE_RX_INIT;
        
        break;
    case RFLR_STATE_CAD_INIT:    
        SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );
    
        SX1278LR.RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    RFLR_IRQFLAGS_RXDONE |
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    //RFLR_IRQFLAGS_CADDONE |
                                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL; // |
                                    //RFLR_IRQFLAGS_CADDETECTED;
        SX1278Write( REG_LR_IRQFLAGSMASK, SX1278LR.RegIrqFlagsMask );
           
                                    // RxDone                   RxTimeout                   FhssChangeChannel           CadDone
        SX1278LR.RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                    // CAD Detected              ModeReady
        SX1278LR.RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1278WriteBuffer( REG_LR_DIOMAPPING1, &SX1278LR.RegDioMapping1, 2 );
            
        SX1278LoRaSetOpMode( RFLR_OPMODE_CAD );
        RFLRState = RFLR_STATE_CAD_RUNNING;
        break;
    case RFLR_STATE_CAD_RUNNING:
        SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
        //		if( DIO3 == 1 ) // CAD Done interrupt
        if( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_CADDONE_MASK )
        { 
            // Clear Irq
           // SX1278Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE  );
            if( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_CADDETECTED_MASK )
            {
                // Clear Irq
                SX1278Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED  );
                // CAD detected, we have a LoRa preamble
                RFLRState = RFLR_STATE_RX_INIT;
                result = RF_CHANNEL_ACTIVITY_DETECTED;
            } 
            else
            {    
                // The device goes in Standby Mode automatically    
                RFLRState = RFLR_STATE_IDLE;
                result = RF_CHANNEL_EMPTY;
            }
        }   
        break;
    
    default:
        break;
    } 
    return result;
}

//#endif // USE_SX1278_RADIO
