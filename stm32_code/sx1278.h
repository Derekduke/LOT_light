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
 * \file       sx1278.h
 * \brief      SX1278 RF chip driver
 *
 * \version    2.0.B2
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __SX1278_H__
#define __SX1278_H__

#include "board.h"
typedef struct{
	bool LoRaOn;
	bool LoRaOnState;
}STRUCT_SX;

typedef enum
{
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
} tRFProcessReturnCodes;



extern STRUCT_SX sx; 

//SX1278 registers array
extern uint8_t SX1278Regs[0x71];
//brief Enables LoRa modem or FSK modem.  param [IN] opMode New operating mode 
void SX1278SetLoRaOn( bool enable );
//Gets the LoRa modem state. return LoraOn Current LoRa modem mode 
bool SX1278GetLoRaOn( void );
//Initializes the SX1278 
uint8_t SX1278Init( void );
//brief Resets the SX1278
void SX1278Reset( void );
//Sets the SX1278 operating mode.  param [IN] opMode New operating mode 
void SX1278SetOpMode( uint8_t opMode );
//Gets the SX1278 operating mode. opMode Current operating mode 
uint8_t SX1278GetOpMode( void );
//brief Reads the current Rx gain setting. rxGain Current gain setting
uint8_t SX1278ReadRxGain( void );

//Trigs and reads the current RSSI value,  rssiValue Current RSSI value in [dBm] 
double SX1278ReadRssi( void );

// Gets the Rx gain value measured while receiving the packet
//retval rxGainValue Current Rx gain value
uint8_t SX1278GetPacketRxGain( void );
//Gets the SNR value measured while receiving the packet
//retval snrValue Current SNR value in [dB]
int8_t SX1278GetPacketSnr( void );
//Gets the RSSI value measured while receiving the packet
//retval rssiValue Current RSSI value in [dBm]
double SX1278GetPacketRssi( void );
//Gets the AFC value measured while receiving the packet
//retval afcValue Current AFC value in [Hz]
uint32_t SX1278GetPacketAfc( void );
//Sets the radio in Rx mode. Waiting for a packet 
void SX1278StartRx( void );
//brief Sets the radio in Cad mode. Waiting for cad done
void SX1278StartCad( void );
//Gets a copy of the current received buffer
//param [IN]: buffer     Buffer pointer
//param [IN]: size       Buffer size
void SX1278GetRxPacket( void *buffer, uint16_t *size );
// Sets a copy of the buffer to be transmitted and starts the transmission
//param [IN]: buffer     Buffer pointer
//param [IN]: size       Buffer size
void SX1278SetTxPacket( const void *buffer, uint16_t size );
// Gets the current RFState
// retval rfState Current RF state [RF_IDLE, RF_BUSY, RF_RX_DONE, RF_RX_TIMEOUT, RF_TX_DONE, RF_TX_TIMEOUT]
uint8_t SX1278GetRFState( void );
// Sets the new state of the RF state machine
//param [IN]: state New RF state machine state 
void SX1278SetRFState( uint8_t state );
// Process the Rx and Tx state machines depending on the  SX1278 operating mode.
// retval rfState Current RF state [RF_IDLE, RF_BUSY, RF_RX_DONE, RF_RX_TIMEOUT, RF_TX_DONE, RF_TX_TIMEOUT]
uint32_t SX1278Process( void );


#endif //__SX1278_H__
