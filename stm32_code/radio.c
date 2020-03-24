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
 * \file       radio.c
 * \brief      Generic radio driver ( radio abstraction )
 *
 * \version    2.0.B2
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Gregory Cristian on Apr 25 2013
 */

#include "radio.h"

#if defined( USE_SX1232_RADIO )
    #include "sx1232.h"
#elif defined( USE_SX1272_RADIO )
    #include "sx1272.h"
	#include "sx1272-LoRaMisc.h"
	#include "sx1272-LoRa.h"
	#include "sx1272-Hal.h"
#elif defined( USE_SX1276_RADIO )
    #include "sx1276.h"
	#include "sx1276-LoRaMisc.h"
	#include "sx1276-LoRa.h"
	#include "sx1276-Hal.h"
#else
//	#error "Missing define: USE_XXXXXX_RADIO (ie. USE_SX1272_RADIO)"
#endif


tRadioDriver RadioDriver;


tRadioDriver* RadioDriverInit( void )
{
#if defined( USE_SX1232_RADIO )

    RadioDriver.Init = SX1232Init;
    RadioDriver.Reset = SX1232Reset;
    RadioDriver.StartRx = SX1232StartRx;
    RadioDriver.GetRxPacket = SX1232GetRxPacket;
    RadioDriver.SetTxPacket = SX1232SetTxPacket;
    RadioDriver.Process = SX1232Process;

#elif defined( USE_SX1272_RADIO )

    RadioDriver.Init = SX1272Init;
    RadioDriver.Reset = SX1272Reset;
	RadioDriver.StartCad = SX1272StartCad;
    RadioDriver.StartRx = SX1272StartRx;
    RadioDriver.GetRxPacket = SX1272GetRxPacket;
    RadioDriver.SetTxPacket = SX1272SetTxPacket;
    RadioDriver.Process = SX1272Process;

	RadioDriver.LoRaSetRFFrequency = SX1272LoRaSetRFFrequency;
	RadioDriver.LoRaGetRFFrequency = SX1272LoRaGetRFFrequency;
	RadioDriver.LoRaSetRFPower = SX1272LoRaSetRFPower;
	RadioDriver.LoRaGetRFPower = SX1272LoRaGetRFPower;
	RadioDriver.LoRaSetSignalBandwidth = SX1272LoRaSetSignalBandwidth;
	RadioDriver.LoRaGetSignalBandwidth = SX1272LoRaGetSignalBandwidth;
	RadioDriver.LoRaSetSpreadingFactor = SX1272LoRaSetSpreadingFactor;
	RadioDriver.LoRaGetSpreadingFactor = SX1272LoRaGetSpreadingFactor;
	RadioDriver.LoRaSetErrorCoding = SX1272LoRaSetErrorCoding;
	RadioDriver.LoRaGetErrorCoding = SX1272LoRaGetErrorCoding;
	RadioDriver.LoRaSetPacketCrcOn = SX1272LoRaSetPacketCrcOn;
	RadioDriver.LoRaGetPacketCrcOn = SX1272LoRaGetPacketCrcOn;
	RadioDriver.LoRaSetImplicitHeaderOn = SX1272LoRaSetImplicitHeaderOn;
	RadioDriver.LoRaGetImplicitHeaderOn = SX1272LoRaGetImplicitHeaderOn;
	RadioDriver.LoRaSetRxSingleOn = SX1272LoRaSetRxSingleOn;
	RadioDriver.LoRaGetRxSingleOn = SX1272LoRaGetRxSingleOn;
	RadioDriver.LoRaSetFreqHopOn = SX1272LoRaSetFreqHopOn;
	RadioDriver.LoRaGetFreqHopOn = SX1272LoRaGetFreqHopOn;
	RadioDriver.LoRaSetHopPeriod = SX1272LoRaSetHopPeriod;
	RadioDriver.LoRaGetHopPeriod = SX1272LoRaGetHopPeriod;
	RadioDriver.LoRaSetTxPacketTimeout = SX1272LoRaSetTxPacketTimeout;
	RadioDriver.LoRaGetTxPacketTimeout = SX1272LoRaGetTxPacketTimeout;
	RadioDriver.LoRaSetRxPacketTimeout = SX1272LoRaSetRxPacketTimeout;
	RadioDriver.LoRaGetRxPacketTimeout = SX1272LoRaGetRxPacketTimeout;
	RadioDriver.LoRaSetPayloadLength = SX1272LoRaSetPayloadLength;
	RadioDriver.LoRaGetPayloadLength = SX1272LoRaGetPayloadLength;
	RadioDriver.LoRaSetPa20dBm = SX1272LoRaSetPa20dBm;
	RadioDriver.LoRaGetPa20dBm = SX1272LoRaGetPa20dBm;
	RadioDriver.LoRaSetPaRamp = SX1272LoRaSetPaRamp;
	RadioDriver.LoRaGetPaRamp = SX1272LoRaGetPaRamp;
	RadioDriver.LoRaSetSymbTimeout = SX1272LoRaSetSymbTimeout;
	RadioDriver.LoRaGetSymbTimeout = SX1272LoRaGetSymbTimeout;
	RadioDriver.LoRaSetLowDatarateOptimize = SX1272LoRaSetLowDatarateOptimize;
	RadioDriver.LoRaGetLowDatarateOptimize = SX1272LoRaGetLowDatarateOptimize;
	RadioDriver.LoRaGetPreambleLength = SX1272LoRaGetPreambleLength;
	RadioDriver.LoRaSetPreambleLength = SX1272LoRaSetPreambleLength;
	RadioDriver.LoRaSetNbTrigPeaks = SX1272LoRaSetNbTrigPeaks;

	RadioDriver.LoRaSetOpMode = SX1272LoRaSetOpMode;
	RadioDriver.Write = SX1272Write;

#elif defined( USE_SX1276_RADIO )

    RadioDriver.Init = SX1276Init;
    RadioDriver.Reset = SX1276Reset;
		RadioDriver.StartCad = SX1276StartCad;
    RadioDriver.StartRx = SX1276StartRx;
    RadioDriver.GetRxPacket = SX1276GetRxPacket;
    RadioDriver.SetTxPacket = SX1276SetTxPacket;
    RadioDriver.Process = SX1276Process;

	RadioDriver.LoRaSetRFFrequency = SX1276LoRaSetRFFrequency;
	RadioDriver.LoRaGetRFFrequency = SX1276LoRaGetRFFrequency;
	RadioDriver.LoRaSetRFPower = SX1276LoRaSetRFPower;
	RadioDriver.LoRaGetRFPower = SX1276LoRaGetRFPower;
	RadioDriver.LoRaSetSignalBandwidth = SX1276LoRaSetSignalBandwidth;
	RadioDriver.LoRaGetSignalBandwidth = SX1276LoRaGetSignalBandwidth;
	RadioDriver.LoRaSetSpreadingFactor = SX1276LoRaSetSpreadingFactor;
	RadioDriver.LoRaGetSpreadingFactor = SX1276LoRaGetSpreadingFactor;
	RadioDriver.LoRaSetErrorCoding = SX1276LoRaSetErrorCoding;
	RadioDriver.LoRaGetErrorCoding = SX1276LoRaGetErrorCoding;
	RadioDriver.LoRaSetPacketCrcOn = SX1276LoRaSetPacketCrcOn;
	RadioDriver.LoRaGetPacketCrcOn = SX1276LoRaGetPacketCrcOn;
	RadioDriver.LoRaSetImplicitHeaderOn = SX1276LoRaSetImplicitHeaderOn;
	RadioDriver.LoRaGetImplicitHeaderOn = SX1276LoRaGetImplicitHeaderOn;
	RadioDriver.LoRaSetRxSingleOn = SX1276LoRaSetRxSingleOn;
	RadioDriver.LoRaGetRxSingleOn = SX1276LoRaGetRxSingleOn;
	RadioDriver.LoRaSetFreqHopOn = SX1276LoRaSetFreqHopOn;
	RadioDriver.LoRaGetFreqHopOn = SX1276LoRaGetFreqHopOn;
	RadioDriver.LoRaSetHopPeriod = SX1276LoRaSetHopPeriod;
	RadioDriver.LoRaGetHopPeriod = SX1276LoRaGetHopPeriod;
	RadioDriver.LoRaSetTxPacketTimeout = SX1276LoRaSetTxPacketTimeout;
	RadioDriver.LoRaGetTxPacketTimeout = SX1276LoRaGetTxPacketTimeout;
	RadioDriver.LoRaSetRxPacketTimeout = SX1276LoRaSetRxPacketTimeout;
	RadioDriver.LoRaGetRxPacketTimeout = SX1276LoRaGetRxPacketTimeout;
	RadioDriver.LoRaSetPayloadLength = SX1276LoRaSetPayloadLength;
	RadioDriver.LoRaGetPayloadLength = SX1276LoRaGetPayloadLength;
	RadioDriver.LoRaSetPa20dBm = SX1276LoRaSetPa20dBm;
	RadioDriver.LoRaGetPa20dBm = SX1276LoRaGetPa20dBm;
	RadioDriver.LoRaSetPaRamp = SX1276LoRaSetPaRamp;
	RadioDriver.LoRaGetPaRamp = SX1276LoRaGetPaRamp;
	RadioDriver.LoRaSetSymbTimeout = SX1276LoRaSetSymbTimeout;
	RadioDriver.LoRaGetSymbTimeout = SX1276LoRaGetSymbTimeout;
	RadioDriver.LoRaSetLowDatarateOptimize = SX1276LoRaSetLowDatarateOptimize;
	RadioDriver.LoRaGetLowDatarateOptimize = SX1276LoRaGetLowDatarateOptimize;
	RadioDriver.LoRaGetPreambleLength = SX1276LoRaGetPreambleLength;
	RadioDriver.LoRaSetPreambleLength = SX1276LoRaSetPreambleLength;
	RadioDriver.LoRaSetNbTrigPeaks = SX1276LoRaSetNbTrigPeaks;

	RadioDriver.LoRaSetOpMode = SX1276LoRaSetOpMode;
	RadioDriver.LoRaGetOpMode = SX1276LoRaGetOpMode;
	RadioDriver.Read = SX1276Read;
	RadioDriver.Write = SX1276Write;
#else
//	#error "Missing define: USE_XXXXXX_RADIO (ie. USE_SX1272_RADIO)"
#endif

    return &RadioDriver;
}
