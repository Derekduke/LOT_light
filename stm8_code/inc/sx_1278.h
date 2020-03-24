
#ifndef __SX_1278_H
#define __SX_1278_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sx_board.h"

#define SX_RESET_0  GPIO_WriteBit(GPIOD , GPIO_Pin_4, RESET)
#define SX_RESET_1  GPIO_WriteBit(GPIOD , GPIO_Pin_4, SET)

#define RF_BUFFER_SIZE  64
#define TX_PACKET_SIZE  64


/*!
 * SX1278 Internal registers Address
 */
#define REG_LR_FIFO                                 0x00
// Common settings
#define REG_LR_OPMODE                               0x01
#define REG_LR_BANDSETTING                          0x04
#define REG_LR_FRFMSB                               0x06
#define REG_LR_FRFMID                               0x07
#define REG_LR_FRFLSB                               0x08
// Tx settings
#define REG_LR_PACONFIG                             0x09
#define REG_LR_PARAMP                               0x0A
#define REG_LR_OCP                                  0x0B
// Rx settings
#define REG_LR_LNA                                  0x0C
// LoRa registers
#define REG_LR_FIFOADDRPTR                          0x0D
#define REG_LR_FIFOTXBASEADDR                       0x0E
#define REG_LR_FIFORXBASEADDR                       0x0F
#define REG_LR_FIFORXCURRENTADDR                    0x10
#define REG_LR_IRQFLAGSMASK                         0x11
#define REG_LR_IRQFLAGS                             0x12
#define REG_LR_NBRXBYTES                            0x13
#define REG_LR_RXHEADERCNTVALUEMSB                  0x14
#define REG_LR_RXHEADERCNTVALUELSB                  0x15
#define REG_LR_RXPACKETCNTVALUEMSB                  0x16
#define REG_LR_RXPACKETCNTVALUELSB                  0x17
#define REG_LR_MODEMSTAT                            0x18
#define REG_LR_PKTSNRVALUE                          0x19
#define REG_LR_PKTRSSIVALUE                         0x1A
#define REG_LR_RSSIVALUE                            0x1B
#define REG_LR_HOPCHANNEL                           0x1C
#define REG_LR_MODEMCONFIG1                         0x1D
#define REG_LR_MODEMCONFIG2                         0x1E
#define REG_LR_SYMBTIMEOUTLSB                       0x1F
#define REG_LR_PREAMBLEMSB                          0x20
#define REG_LR_PREAMBLELSB                          0x21
#define REG_LR_PAYLOADLENGTH                        0x22
#define REG_LR_PAYLOADMAXLENGTH                     0x23
#define REG_LR_HOPPERIOD                            0x24
#define REG_LR_FIFORXBYTEADDR                       0x25
#define REG_LR_MODEMCONFIG3                         0x26
// end of documented register in datasheet
// I/O settings
#define REG_LR_DIOMAPPING1                          0x40
#define REG_LR_DIOMAPPING2                          0x41
// Version
#define REG_LR_VERSION                              0x42
// Additional settings
#define REG_LR_PLLHOP                               0x44
#define REG_LR_TCXO                                 0x4B
#define REG_LR_PADAC                                0x4D
#define REG_LR_FORMERTEMP                           0x5B
#define REG_LR_BITRATEFRAC                          0x5D
#define REG_LR_AGCREF                               0x61
#define REG_LR_AGCTHRESH1                           0x62
#define REG_LR_AGCTHRESH2                           0x63
#define REG_LR_AGCTHRESH3                           0x64

/*!
 * SX1278 LoRa bit control definition
 */


/*!
 * SX1278 LoRa bit control definition
 */

/*!
 * RegFifo
 */

/*!
 * RegOpMode
 */
#define RFLR_OPMODE_LONGRANGEMODE_MASK              0x7F
#define RFLR_OPMODE_LONGRANGEMODE_OFF               0x00 // Default
#define RFLR_OPMODE_LONGRANGEMODE_ON                0x80

#define RFLR_OPMODE_ACCESSSHAREDREG_MASK            0xBF
#define RFLR_OPMODE_ACCESSSHAREDREG_ENABLE          0x40
#define RFLR_OPMODE_ACCESSSHAREDREG_DISABLE         0x00 // Default

#define RFLR_OPMODE_FREQMODE_ACCESS_MASK            0xF7
#define RFLR_OPMODE_FREQMODE_ACCESS_LF              0x08 // Default
#define RFLR_OPMODE_FREQMODE_ACCESS_HF              0x00

#define RFLR_OPMODE_MASK                            0xF8
#define RFLR_OPMODE_SLEEP                           0x00
#define RFLR_OPMODE_STANDBY                         0x01 // Default
#define RFLR_OPMODE_SYNTHESIZER_TX                  0x02
#define RFLR_OPMODE_TRANSMITTER                     0x03
#define RFLR_OPMODE_SYNTHESIZER_RX                  0x04
#define RFLR_OPMODE_RECEIVER                        0x05
// LoRa specific modes
#define RFLR_OPMODE_RECEIVER_SINGLE                 0x06
#define RFLR_OPMODE_CAD                             0x07

/*!
 * RegBandSetting
 */
#define RFLR_BANDSETTING_MASK                       0x3F
#define RFLR_BANDSETTING_AUTO                       0x00 // Default
#define RFLR_BANDSETTING_DIV_BY_1                   0x40
#define RFLR_BANDSETTING_DIV_BY_2                   0x80
#define RFLR_BANDSETTING_DIV_BY_6                   0xC0

/*!
 * RegFrf (MHz)
   SX1278 频率只能在137MHz-525MHz 本次应用模块在 415-525MHz
   计算方式， 应用的频率 = (32*10^6)*0X(MSB-MID-LSB)/(2^19)
   python: print('{0:X}'.format(int((435/32)*(2**19))))
 */   
#define RFLR_FRFMSB_434_MHZ                         0x6C // Default
#define RFLR_FRFMID_434_MHZ                         0x80 // Default
#define RFLR_FRFLSB_434_MHZ                         0x00 // Default

#define RFLR_FRFMSB_435_MHZ                         0x6C // Default
#define RFLR_FRFMID_435_MHZ                         0xC0 // Default
#define RFLR_FRFLSB_435_MHZ                         0x00 // Default



/*!
 * RegPaConfig
 */
#define RFLR_PACONFIG_PASELECT_MASK                 0x7F
#define RFLR_PACONFIG_PASELECT_PABOOST              0x80
#define RFLR_PACONFIG_PASELECT_RFO                  0x00 // Default

#define RFLR_PACONFIG_MAX_POWER_MASK                0x8F
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_ON            0x40
#define RFLR_PACONFIG_OUTPUTPOWER_MASK              0xF0

/*!
 * RegPaRamp
 */
#define RFLR_PARAMP_TXBANDFORCE_MASK                0xEF
#define RFLR_PARAMP_TXBANDFORCE_BAND_SEL            0x10
#define RFLR_PARAMP_TXBANDFORCE_AUTO                0x00 // Default

#define RFLR_PARAMP_MASK                            0xF0
#define RFLR_PARAMP_3400_US                         0x00
#define RFLR_PARAMP_2000_US                         0x01
#define RFLR_PARAMP_1000_US                         0x02
#define RFLR_PARAMP_0500_US                         0x03
#define RFLR_PARAMP_0250_US                         0x04
#define RFLR_PARAMP_0125_US                         0x05
#define RFLR_PARAMP_0100_US                         0x06
#define RFLR_PARAMP_0062_US                         0x07
#define RFLR_PARAMP_0050_US                         0x08
#define RFLR_PARAMP_0040_US                         0x09 // Default
#define RFLR_PARAMP_0031_US                         0x0A
#define RFLR_PARAMP_0025_US                         0x0B
#define RFLR_PARAMP_0020_US                         0x0C
#define RFLR_PARAMP_0015_US                         0x0D
#define RFLR_PARAMP_0012_US                         0x0E
#define RFLR_PARAMP_0010_US                         0x0F

/*!
 * RegOcp
 */
#define RFLR_OCP_MASK                               0xDF
#define RFLR_OCP_ON                                 0x20 // Default
#define RFLR_OCP_OFF                                0x00

#define RFLR_OCP_TRIM_MASK                          0xE0
#define RFLR_OCP_TRIM_045_MA                        0x00
#define RFLR_OCP_TRIM_050_MA                        0x01
#define RFLR_OCP_TRIM_055_MA                        0x02
#define RFLR_OCP_TRIM_060_MA                        0x03
#define RFLR_OCP_TRIM_065_MA                        0x04
#define RFLR_OCP_TRIM_070_MA                        0x05
#define RFLR_OCP_TRIM_075_MA                        0x06
#define RFLR_OCP_TRIM_080_MA                        0x07
#define RFLR_OCP_TRIM_085_MA                        0x08
#define RFLR_OCP_TRIM_090_MA                        0x09
#define RFLR_OCP_TRIM_095_MA                        0x0A
#define RFLR_OCP_TRIM_100_MA                        0x0B  // Default
#define RFLR_OCP_TRIM_105_MA                        0x0C
#define RFLR_OCP_TRIM_110_MA                        0x0D
#define RFLR_OCP_TRIM_115_MA                        0x0E
#define RFLR_OCP_TRIM_120_MA                        0x0F
#define RFLR_OCP_TRIM_130_MA                        0x10
#define RFLR_OCP_TRIM_140_MA                        0x11
#define RFLR_OCP_TRIM_150_MA                        0x12
#define RFLR_OCP_TRIM_160_MA                        0x13
#define RFLR_OCP_TRIM_170_MA                        0x14
#define RFLR_OCP_TRIM_180_MA                        0x15
#define RFLR_OCP_TRIM_190_MA                        0x16
#define RFLR_OCP_TRIM_200_MA                        0x17
#define RFLR_OCP_TRIM_210_MA                        0x18
#define RFLR_OCP_TRIM_220_MA                        0x19
#define RFLR_OCP_TRIM_230_MA                        0x1A
#define RFLR_OCP_TRIM_240_MA                        0x1B

/*!
 * RegLna
 */
#define RFLR_LNA_GAIN_MASK                          0x1F
#define RFLR_LNA_GAIN_G1                            0x20 // Default
#define RFLR_LNA_GAIN_G2                            0x40
#define RFLR_LNA_GAIN_G3                            0x60
#define RFLR_LNA_GAIN_G4                            0x80
#define RFLR_LNA_GAIN_G5                            0xA0
#define RFLR_LNA_GAIN_G6                            0xC0

#define RFLR_LNA_BOOST_LF_MASK                      0xE7
#define RFLR_LNA_BOOST_LF_DEFAULT                   0x00 // Default
#define RFLR_LNA_BOOST_LF_GAIN                      0x08
#define RFLR_LNA_BOOST_LF_IP3                       0x10
#define RFLR_LNA_BOOST_LF_BOOST                     0x18

#define RFLR_LNA_RXBANDFORCE_MASK                   0xFB
#define RFLR_LNA_RXBANDFORCE_BAND_SEL               0x04
#define RFLR_LNA_RXBANDFORCE_AUTO                   0x00 // Default

#define RFLR_LNA_BOOST_HF_MASK                      0xFC
#define RFLR_LNA_BOOST_HF_OFF                       0x00 // Default
#define RFLR_LNA_BOOST_HF_ON                        0x03

/*!
 * RegFifoAddrPtr
 */
#define RFLR_FIFOADDRPTR                            0x00 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFOTXBASEADDR                         0x80 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFORXBASEADDR                         0x00 // Default

/*!
 * RegFifoRxCurrentAddr (Read Only)
 */

/*!
 * RegIrqFlagsMask
 */
#define RFLR_IRQFLAGS_RXTIMEOUT_MASK                0x80
#define RFLR_IRQFLAGS_RXDONE_MASK                   0x40
#define RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK          0x20
#define RFLR_IRQFLAGS_VALIDHEADER_MASK              0x10
#define RFLR_IRQFLAGS_TXDONE_MASK                   0x08
#define RFLR_IRQFLAGS_CADDONE_MASK                  0x04
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL_MASK       0x02
#define RFLR_IRQFLAGS_CADDETECTED_MASK              0x01


/*!
 * RFLR_MODECONFIG_Mask
 */
#define RFLR_MODEMCONFIG2_SF_MASK                   0x0F
#define RFLR_MODEMCONFIG1_CODINGRATE_MASK           0xF1
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK         0xFB
#define RFLR_MODEMCONFIG1_BW_MASK                   0x0F
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK       0xFE
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK       0xFC
#define RFLR_MODEMCONFIG3_MOBILE_NODE_MASK          0xF7



/*!
 * RegIrqFlags
 */
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80
#define RFLR_IRQFLAGS_RXDONE                        0x40
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10
#define RFLR_IRQFLAGS_TXDONE                        0x08
#define RFLR_IRQFLAGS_CADDONE                       0x04
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02
#define RFLR_IRQFLAGS_CADDETECTED                   0x01

/*!
 * RF state machine
 */

#define     RF_IDLE                             0
#define     RF_BUSY                             1
#define     RF_RX_DONE                          2
#define     RF_RX_TIMEOUT                       3
#define     RF_RX_ID_ERROR                      4
#define     RF_TX_DONE                          5
#define     RF_TX_TIMEOUT                       6
#define     RF_LEN_ERROR                        7
#define     RF_CHANNEL_EMPTY                    8
#define     RF_CHANNEL_ACTIVITY_DETECTED        9

//LoRa
//#define      RFLR_STATE_IDLE                    0
//#define      RFLR_STATE_RX_INIT                 1
//#define      RFLR_STATE_RX_RUNNING              2
//#define      RFLR_STATE_RX_DONE                 3
//#define      RFLR_STATE_RX_TIMEOUT              4
//#define      RFLR_STATE_TX_INIT                 5
//#define      RFLR_STATE_TX_RUNNING              6
//#define      RFLR_STATE_TX_DONE                 7
//#define      RFLR_STATE_TX_TIMEOUT              8
//#define      RFLR_STATE_CAD_INIT                9
//#define      RFLR_STATE_CAD_RUNNING             10
//

   
typedef struct{                          //RFFrequency 没有定义，原因是计算复杂，可在函数中查询计算方式 
    uint8_t  lastRxPacketSize;           //表示上一次接收的数据长度  
    uint8_t  Power;
    uint8_t  SignalBw;                   // LORA [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                                        // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    uint8_t SpreadingFactor;            // LORA [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    uint8_t ErrorCoding;                // LORA [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    _Bool CrcOn;                         // [0: OFF, 1: ON]
    _Bool ImplicitHeaderOn;              // [0: OFF, 1: ON]
    _Bool RxSingleOn;                    // [0: Continuous, 1 Single]
    _Bool FreqHopOn;                     // [0: OFF, 1: ON]
    uint8_t HopPeriod;                  // Hops every frequency hopping period symbols
    uint32_t TxPacketTimeout;
    uint32_t RxPacketTimeout;
    uint8_t PayloadLength;
}STRU_LoRaSettings;





typedef union{  
  _Bool LoRaOn:       1;
  _Bool LoRaOnStete:  1;
  _Bool bit2:         1;
  _Bool bit3:         1;
  _Bool bit4:1;
  _Bool bit5:1;
  _Bool bit6:1;
  _Bool bit7:1; 
}U_BIT_sxb;

typedef struct{
    uint8_t RegOpMode;                              // 0x01
    uint8_t RegPaRamp;                              // 0x0A
    uint8_t RegLna;                                 // 0x0C
    uint8_t RegFifoAddrPtr;                         // 0x0D
    uint8_t RegFifoTxBaseAddr;                      // 0x0E
    uint8_t RegFifoRxBaseAddr;                      // 0x0F
    uint8_t RegFifoRxCurrentAddr;                   // 0x10
    uint8_t RegIrqFlagsMask;                        // 0x11
    uint8_t RegIrqFlags;                            // 0x12
    uint8_t RegNbRxBytes;                           // 0x13
    uint8_t RegHopChannel;                          // 0x1C
    uint8_t RegModemConfig1;                        // 0x1D
    uint8_t RegModemConfig2;                        // 0x1E
    uint8_t RegPayloadLength;                       // 0x22    
}STRU_SX1278LR;

typedef enum
{   RFLR_STATE_IDLE,
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
}tRFLRStates;

extern STRU_LoRaSettings LoRaSettings;
extern uint8_t temp_RFLRState;
void SX1278Reset( void );
_Bool SX1278Init( void );
void SX1278LoRaSetOpMode( uint8_t opMode );
void SX1278GetNeedReg(void);
void SX1278SetLoRaOn(void);
uint8_t SX1278LoRaGetOpMode( void );
uint8_t SX1278LoRaReadRxGain( void );
void SX1278LoRaStartRx( void );
void SX1278LoRaSetTxPacket( const void *buffer, uint8_t size );
void SX1278LoRaGetRxPacket( void *buffer, uint8_t *size );
void SX1278LoRaSetErrorCoding( uint8_t value );
void SX1278LoRaSetSignalBandwidth( uint8_t bw );
void SX1278LoRaSetRFPower( uint8_t power );
void SX1278LoRaSetNbTrigPeaks( uint8_t value );
void SX1278LoRaSetSpreadingFactor( uint8_t factor );


void SX1278LoRaProcess( void );



#endif
/************************end **************************************************/
