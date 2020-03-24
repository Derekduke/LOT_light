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

//#include "radio.h"
#include "sx1278.h"
#include "sx1278-Hal.h"
#include "sx1278-LoRa.h"

/*!
 * SX1278 registers variable
 */
 
STRUCT_SX sx;
uint8_t SX1278Regs[0x71];
//static bool LoRaOn = false;
//static bool LoRaOnState = false;

uint8_t SX1278Init( void )
{
	uint8_t TempReg;
	//判断参数是否可用
	if(false == SX1278LoraConfigCheck())
	{		//这是仅检测配置信息对不对,并没有配置任何信息
			//return SX127x_PARAMETER_INVALID;
	}
	SX1278Reset( ); //仅复位
	//SPI测试
  SX1278Read(0x06,&TempReg);
  if(TempReg != 0x6C)	return 1;	 
	 
	//系统配置为LoRa 模式
    sx.LoRaOn = true;	 
    SX1278SetLoRaOn( sx.LoRaOn );	 
    SX1278LoRaInit( );
	return 0;
}



void SX1278Reset( void )
{
    uint32_t startTick;
    SX_RESET_0;
    // Wait 1ms
    startTick =HAL_GetTick();
    while( ( HAL_GetTick() - startTick ) < 3 );  //1
    SX_RESET_1;
    // Wait 6ms
    startTick = HAL_GetTick();
    while(( HAL_GetTick() - startTick ) <6 );
		
}

void SX1278SetLoRaOn( bool enable )
{
    if( sx.LoRaOnState == enable ) return;
    sx.LoRaOnState = enable;
    sx.LoRaOn = enable;
    if( sx.LoRaOn == true )
    {
				SX1278LoRaSetOpMode( RFLR_OPMODE_SLEEP );
				
        SX1278LR.RegOpMode = ( SX1278LR.RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
        SX1278Write( REG_LR_OPMODE, SX1278LR.RegOpMode );

        SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );
                                        // RxDone               RxTimeout                   FhssChangeChannel           CadDone
        SX1278LR.RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                        // CadDetected          ModeReady
        SX1278LR.RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1278WriteBuffer( REG_LR_DIOMAPPING1, &SX1278LR.RegDioMapping1, 2 );
        SX1278ReadBuffer( REG_LR_OPMODE, SX1278Regs+1, 0x6F );
    }

}


void SX1278SetOpMode( uint8_t opMode )
{
   SX1278LoRaSetOpMode( opMode );
}

uint8_t SX1278GetOpMode( void )
{
    return SX1278LoRaGetOpMode( ); 
}

double SX1278ReadRssi( void )
{
    return SX1278LoRaReadRssi( );
}

uint8_t SX1278ReadRxGain( void )
{
    return SX1278LoRaReadRxGain( );
}

uint8_t SX1278GetPacketRxGain( void )
{
    return SX1278LoRaGetPacketRxGain(  );
}

int8_t SX1278GetPacketSnr( void )
{
    return SX1278LoRaGetPacketSnr(  );
}

double SX1278GetPacketRssi( void )
{
    return SX1278LoRaGetPacketRssi( );
}


void SX1278StartRx( void )
{
   SX1278LoRaSetRFState( RFLR_STATE_RX_INIT );
}

void SX1278StartCad( void )
{
   SX1278LoRaSetRFState( RFLR_STATE_CAD_INIT );
}

void SX1278GetRxPacket( void *buffer, uint16_t *size )
{
   SX1278LoRaGetRxPacket( buffer, size ); 
}

void SX1278SetTxPacket( const void *buffer, uint16_t size )
{
   SX1278LoRaSetTxPacket( buffer, size );
}

uint8_t SX1278GetRFState( void )
{
    return SX1278LoRaGetRFState( );
}

void SX1278SetRFState( uint8_t state )
{
    SX1278LoRaSetRFState( state );
}

uint32_t SX1278Process( void )
{
    return SX1278LoRaProcess( );
}


