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
�������� �� SX1278LoRaInit
��    �� �� Lora��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void SX1278LoRaInit( void )
{
    RFLRState = RFLR_STATE_IDLE;
    SX1278Read( REG_LR_VERSION, &SX1278LR.RegVersion );//��ȡоƬ�İ汾��	
    SX1278ReadBuffer( 0, SX1278Regs, 0x71); //��оƬ���ݼĴ���ֵ�����ṹ����

    SX1278LR.RegLna = RFLR_LNA_GAIN_G1;		
    SX1278Write( REG_LR_LNA, SX1278LR.RegLna); //��������Ϊ���G1
    // set the RF settings
    SX1278LoRaSetRFFrequency();																		//Ƶ������
    SX1278LoRaSetSpreadingFactor( LoRaSettings.SpreadingFactor ); // ��Ƶ��������
    SX1278LoRaSetErrorCoding( LoRaSettings.ErrorCoding );					//ѭ��������
    SX1278LoRaSetPacketCrcOn( LoRaSettings.CrcOn );								//У��
    SX1278LoRaSetSignalBandwidth( LoRaSettings.SignalBw );				//����
    SX1278LoRaSetImplicitHeaderOn( LoRaSettings.ImplicitHeaderOn );//ǰ��
    SX1278LoRaSetSymbTimeout( 0x3FF );														//��ʱʱ��
    SX1278LoRaSetPayloadLength( LoRaSettings.PayloadLength );			//����س���
		//ע�⣬��ͷģʽ��Implicit Header��ʱ��������ǰ�涨���շ�˫����PL
    SX1278LoRaSetLowDatarateOptimize( true );											//�����Ż�
		
		SX1278LoRaSetPaRamp(RFLR_PARAMP_0100_US);
		//  ��PA Ramp��ʱ�䣬����û�LDO���ܿ������������������������ʵ�����PA Rampʱ��
    //  �����Rampʱ����̳�����LDO������ʱ������ֽ���TX��ϵͳ����Ϊ�������������RF�źŲ����ֵ�����
		
		
		 SX1278Write( REG_LR_OCP,0x20|RFLR_OCP_TRIM_240_MA);//�������ر��� Over Current Protection
		 
		if( LoRaSettings.RFFrequency > 380000000 )  //860000000
    {
        SX1278LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST ); //ѡ�� PA_BOOST �ܽ�����ź�
//        SX1278LoRaSetPa20dBm( true );  //����������
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
�������� �� SX1278LoraConfigCheck
��    �� �� Lora ��ʼ����Ϣ��飬��ֹ������Ϣ����
��    �� �� ��
�� �� ֵ �� bool �ɹ�Ϊ1  ʧ��Ϊ0
��    �� �� sun
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
�������� �� SX1278LoRaReset
��    �� �� LoraоƬ����
��    �� �� ��
�� �� ֵ �� ��
��    �� �� sun
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
�������� �� SX1278LoRaSetOpMode
��    �� �� ����Lora����ģʽ
��    �� �� opMode  ����ģʽ
						
�� �� ֵ �� ��
��    �� �� sun
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
�������� �� SX1278LoRaGetOpMode
��    �� �� ��ȡ��ǰLora����ģʽ
��    �� �� ��					
�� �� ֵ �� ��ǰ����ģʽ
��    �� �� sun
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
�������� �� SX1278LoRaReadRssi
��    �� �� ��ȡ����Lora�ź�ǿ��
��    �� �� ��
						
�� �� ֵ �� ����ֵ Ϊ�ź�ǿ��
��    �� �� sun
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
�������� �� SX1278LoRaGetPacketSnr
��    �� �� ��ȡLora�����
��    �� �� ��
						
�� �� ֵ �� ����ֵ �����
��    �� �� sun
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
�������� �� SX1278LoRaGetPacketRssi
��    �� �� ��Lora�ź�ǿ��
��    �� �� ��
						
�� �� ֵ �� ����ֵ Ϊ�ź�ǿ��
��    �� �� sun
*************************************************/
double SX1278LoRaGetPacketRssi( void )
{
    return RxPacketRssiValue;
}
/************************************************
�������� �� SX1278LoRaStartRx
��    �� �� Lora��ʼ����
��    �� �� ��
						
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void SX1278LoRaStartRx( void )
{
    SX1278LoRaSetRFState( RFLR_STATE_RX_INIT );
}
/************************************************
�������� �� SX1278LoRaGetRxPacket
��    �� �� Lora��ȡ��������
��    �� �� buffer ���ջ�����
						���յ������ݳ���
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void SX1278LoRaGetRxPacket( void *buffer, uint16_t *size )
{
		if(RxPacketSize<1){*size=0; return;}
		memcpy(( void * )buffer, ( void * )RFBuffer, ( size_t )RxPacketSize);
		*size = RxPacketSize;
		RxPacketSize = 0;		
}
/************************************************
�������� �� SX1278LoRaSetTxPacket
��    �� �� Lora��ʼ����
��    �� �� buffer ���ͻ�����
						size   ���͵����ݳ���
�� �� ֵ �� ��
��    �� �� sun
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
�������� �� SX1278LoRaGetRFState
��    �� �� ��ȡ����״̬
��    �� �� ��
						
�� �� ֵ �� ����״̬
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
��    �� �� sun
*************************************************/

uint8_t SX1278LoRaGetRFState( void )
{
    return RFLRState;
}

/************************************************
�������� �� SX1278LoRaSetRFState
��    �� �� ���ù���״̬
��    �� �� ����״̬
						
�� �� ֵ �� ��
��    �� �� sun
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
�������� �� SX1278LoRaProcess
��    �� �� Lora����
��    �� �� ��					
�� �� ֵ �� Lora�Ĺ���ģʽ
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
��    �� �� sun
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
        
        //memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );  //���RFBuffer

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
            if( ( HAL_GetTick() - RxTimeoutTimer ) > PacketTimeout )   //�����Ƿ�ʱ
            {
                RFLRState = RFLR_STATE_RX_TIMEOUT;
            }
        }
        break;
    case RFLR_STATE_RX_DONE:
        SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
        if( ( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )     //CRCУ��
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
       
        //����
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
        
        // Ƶ��С��860M
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
            SX1278Write( REG_LR_FIFOADDRPTR, SX1278LR.RegFifoAddrPtr );    //��ȡ��������

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

  
        {//����Ƶģʽ
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
