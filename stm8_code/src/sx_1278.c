#include "stm8l15x.h"
#include "sx_board.h"
#include "string.h"
U_BIT_sxb sx;
STRU_SX1278LR SX1278LR;
STRU_LoRaSettings LoRaSettings={
    0,                // lastRFFrequency
    20,               // Power
    8,                // SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                      // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    8,                // SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    2,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    1,                // CrcOn [0: OFF, 1: ON]
    0,                // mplicitHeaderOn [0: OFF, 1: ON]
    1,                // RxSingleOn [0: Continuous, 1 Single]
    0,                // FreqHopOn [0: OFF, 1: ON]
    4,                // HopPeriod Hops every frequency hopping period symbols
    100,              // TxPacketTimeout
    500,              // RxPacketTimeout
    128,              // PayloadLength (used for implicit header mode)
};




uint8_t RFBuffer[RF_BUFFER_SIZE];
STRU_LoRaSettings sv;
uint8_t temp_RFLRState;
uint8_t result=RF_BUSY;

static int8_t RxPacketSnrEstimate;
static uint8_t RFLRState = RFLR_STATE_IDLE;
static uint32_t RxTimeoutTimer = 0;
static uint32_t PacketTimeout=100;
static uint8_t RxPacketSize = 0;
static uint8_t TxPacketSize = 0;


//ģ��ĸ�λ
void SX1278Reset( void )
{
    SX_RESET_0;
    // Wait 2000=1ms    
    sys_delay(2000);
    SX_RESET_1;
    // Wait 6ms
    sys_delay(12000);		
}


//����оƬ����ģʽ
void SX1278LoRaSetOpMode( uint8_t opMode ){
  if(opMode!= (SX1278LR.RegOpMode & 0x07)){
     SX1278LR.RegOpMode = ( SX1278LR.RegOpMode & RFLR_OPMODE_MASK ) | opMode;
     SX1278Write( REG_LR_OPMODE, SX1278LR.RegOpMode );
  }
}

void SX1278LoRaFsk( uint8_t opMode )
{
    SX1278Read(REG_LR_OPMODE, &SX1278LR.RegOpMode);
    SX1278LR.RegOpMode &=0x7F;
    SX1278LR.RegOpMode |= (uint8_t)opMode;
    SX1278Write(REG_LR_OPMODE,SX1278LR.RegOpMode);
}

//�����Ҫ�ļĴ�����ֵ
void SX1278GetNeedReg(void){
  SX1278Read( REG_LR_OPMODE, &SX1278LR.RegOpMode );
  SX1278Read( REG_LR_IRQFLAGSMASK, &SX1278LR.RegIrqFlagsMask);
  SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags);
}

//����ϵͳΪLoraģʽ
void SX1278SetLoRaOn(void){ 
  SX1278Write( REG_LR_OPMODE, 0x00); //�޸�Lora��ǰ�������Ƚ���˯��ģʽ
  SX1278Write( REG_LR_OPMODE, 0x80); //�޸ĳ�Loraģʽ
  SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );
  SX1278Write( REG_LR_DIOMAPPING1, 0x00); //gpio ����ӳ��
  SX1278Write( REG_LR_DIOMAPPING2, 0x00);//gpio ����ӳ��
  SX1278GetNeedReg();//�ѱ�Ҫ�ļĴ���ֵ�����ڴ���
}


//�������ģʽ����ֵ
uint8_t SX1278LoRaGetOpMode( void )
{
    SX1278Read( REG_LR_OPMODE, &SX1278LR.RegOpMode );

    return SX1278LR.RegOpMode & ~RFLR_OPMODE_MASK;
}

//����ź�����ǿ�ȼĴ�����ֵ
uint8_t SX1278LoRaReadRxGain( void )
{
    SX1278Read( REG_LR_LNA, &SX1278LR.RegLna );
    return( SX1278LR.RegLna >> 5 ) & 0x07;
}


//ʹϵͳ�������״̬
void SX1278LoRaStartRx( void )
{
   RFLRState = RFLR_STATE_RX_INIT;  
}

//������ݰ����������ݰ�
void SX1278LoRaGetRxPacket( void *buffer, uint8_t *size )
{    
    if(RxPacketSize<1){ *size=0; return;}
    *size = RxPacketSize;   
    LoRaSettings.lastRxPacketSize=RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBuffer, ( size_t )*size );
}

//�������ݰ���������Ҫ��sizeС��64
void SX1278LoRaSetTxPacket( const void *buffer, uint8_t size )
{
    TxPacketSize = size;
    memcpy( ( void * )RFBuffer, buffer, ( size_t )TxPacketSize );

    RFLRState = RFLR_STATE_TX_INIT;
}

void SX1278LoRaSetRFFrequency(void)
{   //�����ڳ�ʼ����
    SX1278Write(REG_LR_FRFMSB,RFLR_FRFMSB_435_MHZ);
    SX1278Write(REG_LR_FRFMID,RFLR_FRFMID_435_MHZ);
    SX1278Write(REG_LR_FRFLSB,RFLR_FRFLSB_435_MHZ);            
}
void SX1278LoRaSetRFPower( uint8_t power )
{
    SX1278Write(REG_LR_PADAC,0x87);
    SX1278Write(REG_LR_PACONFIG,power);
}
void SX1278LoRaSetNbTrigPeaks( uint8_t value )
{
    uint8_t RECVER_DAT;
    SX1278Read( 0x31, &RECVER_DAT );
    RECVER_DAT = ( RECVER_DAT & 0xF8 ) | value;
    SX1278Write(0x31,RECVER_DAT);
}
void SX1278LoRaSetSpreadingFactor( uint8_t factor )
{
    uint8_t RECVER_DAT;
    if( factor > 12 ){factor = 12; }else if( factor < 6 ){factor = 6;  }
    if( factor == 6 ){SX1278LoRaSetNbTrigPeaks( 5 ); } else{SX1278LoRaSetNbTrigPeaks( 3 ); }
    
    SX1278Read( REG_LR_MODEMCONFIG2, &RECVER_DAT );  
    RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );//RFLR_MODEMCONFIG2_SF_MASK= 0x0F	
    SX1278Write(REG_LR_MODEMCONFIG2,RECVER_DAT);      
}
void SX1278LoRaSetErrorCoding( uint8_t value )
{	
    uint8_t RECVER_DAT;
    SX1278Read( REG_LR_MODEMCONFIG1,&RECVER_DAT);
    RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );//RFLR_MODEMCONFIG1_CODINGRATE_MASK=0xF1
    SX1278Write(REG_LR_MODEMCONFIG1,RECVER_DAT);   
}
void SX1278LoRaSetPacketCrcOn( _Bool enable )
{	
    uint8_t RECVER_DAT;
    SX1278Read( REG_LR_MODEMCONFIG2,&RECVER_DAT);
    RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );//RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK=0xFB
    SX1278Write( REG_LR_MODEMCONFIG2, RECVER_DAT );
}
void SX1278LoRaSetSignalBandwidth( uint8_t bw )
{
    uint8_t RECVER_DAT;
    SX1278Read( REG_LR_MODEMCONFIG1,&RECVER_DAT);
    RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );//RFLR_MODEMCONFIG1_BW_MASK=0x0F
    SX1278Write( REG_LR_MODEMCONFIG1, RECVER_DAT );
  // LoRaSettings.SignalBw = bw;
}
void SX1278LoRaSetImplicitHeaderOn( _Bool enable )
{
    uint8_t RECVER_DAT;
    SX1278Read( REG_LR_MODEMCONFIG1,&RECVER_DAT);
    RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );//RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK=0xFE
    SX1278Write( REG_LR_MODEMCONFIG1, RECVER_DAT );
}
void SX1278LoRaSetSymbTimeout( uint16_t value )
{
    uint8_t RECVER_DAT[2];
    SX1278Read( REG_LR_MODEMCONFIG2,&RECVER_DAT[0]);
    SX1278Read( REG_LR_SYMBTIMEOUTLSB,&RECVER_DAT[1] );
    RECVER_DAT[0] = ( RECVER_DAT[0] & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );//RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK=0xFC
    RECVER_DAT[1] = value & 0xFF;
    SX1278Write( REG_LR_MODEMCONFIG2, RECVER_DAT[0]);
    SX1278Write( REG_LR_SYMBTIMEOUTLSB, RECVER_DAT[1]);
}
void SX1278LoRaSetPayloadLength( uint8_t value )
{
    SX1278Write( REG_LR_PAYLOADLENGTH, value );
} 
void SX1278LoRaSetPreamLength( uint16_t value )
{
    uint8_t a[2];
    a[0]=(value&0xff00)>>8;
    a[1]=value&0xff;
    SX1278Write( REG_LR_PREAMBLEMSB, a[0] );
    SX1278Write( REG_LR_PREAMBLELSB, a[1] );
}
void SX1278LoRaSetLowDatarateOptimize( _Bool enable )
{	 
    uint8_t RECVER_DAT;
    SX1278Read( REG_LR_MODEMCONFIG3 ,&RECVER_DAT);
    RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG3_MOBILE_NODE_MASK ) | ( enable << 3 );//RFLR_MODEMCONFIG3_MOBILE_NODE_MASK=0xF7
    SX1278Write( REG_LR_MODEMCONFIG3, RECVER_DAT );
}
void SX1278LoRaSetPaRamp( uint8_t value )
{
    SX1278Read( REG_LR_PARAMP, &SX1278LR.RegPaRamp );
    SX1278LR.RegPaRamp = ( SX1278LR.RegPaRamp & RFLR_PARAMP_MASK ) | ( value & ~RFLR_PARAMP_MASK );
    SX1278Write( REG_LR_PARAMP, SX1278LR.RegPaRamp );
}



//оƬ�ĳ�ʼ��
_Bool SX1278Init( void )
{
  uint8_t TempReg;         
  sv.SignalBw=8;         //����0--9�� 7.8K --500K  ���������������50K���ϣ�����Ծ�����ȶ���Ҫ��ܸߡ�
  sv.Power=0x8f;         //b3-0,b7=1,��PA_BOOST����������ɴ�20DB Ϊ0����RFO�����������߿ɴ�14DB
  sv.ErrorCoding=2;      //��������ʣ�1--4������ȶ��ȣ����ӿ�����������Խ��Խ�ȶ�
  sv.SpreadingFactor=8;  //�������Ϊ7--12��
  sv.CrcOn=1;
  sv.ImplicitHeaderOn=0;
  sv.PayloadLength=128;
  
  SX1278Reset( );
  //SPI2 test
  SX1278Read(0x06,&TempReg);
  if(TempReg != 0x6C)  return 0;
  //��ʼ������
  SX1278LoRaSetOpMode(RFLR_OPMODE_SLEEP);                                    
  //SX1278LoRaFsk(RFLR_OPMODE_LONGRANGEMODE_ON);                                        
  SX1278LoRaSetOpMode(RFLR_OPMODE_STANDBY);                                   
 // SX1278Write( REG_LR_DIOMAPPING1,1); 
 // SX1278Write( REG_LR_DIOMAPPING2,1); 
  
  SX1278LoRaSetRFFrequency();                         //Ƶ������
  SX1278LoRaSetSpreadingFactor(sv.SpreadingFactor);   //��Ƶ��������
  SX1278LoRaSetErrorCoding(sv.ErrorCoding);           //ѭ��������		 
  SX1278LoRaSetPacketCrcOn(sv.CrcOn);	              //У��
  SX1278LoRaSetSignalBandwidth( sv.SignalBw );        //����
  SX1278LoRaSetImplicitHeaderOn(sv.ImplicitHeaderOn); //ǰ����
  SX1278LoRaSetSymbTimeout( 0x3FF );                  //��ʱʱ��
  SX1278LoRaSetPayloadLength(sv.PayloadLength);       //����س���
  SX1278LoRaSetLowDatarateOptimize(TRUE ); 	  
  
  SX1278LoRaSetPaRamp(RFLR_PARAMP_0100_US);
  
  SX1278LoRaSetRFPower(sv.Power); 		 
  SX1278Write(REG_LR_OCP,0x0B);   
  SX1278Write(REG_LR_LNA,0x20);
   
 
    
  SX1278SetLoRaOn();	 
  return 1;
}



//===============================�¼�����״̬��=================================
void SX1278LoRaProcess( void )
{
    temp_RFLRState=RFLRState;
    switch( RFLRState )
    {
    case RFLR_STATE_IDLE:
        break;
    case RFLR_STATE_RX_INIT:
        
        SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        SX1278LR.RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    RFLR_IRQFLAGS_CADDONE |
                                    RFLR_IRQFLAGS_CADDETECTED;
        SX1278Write( REG_LR_IRQFLAGSMASK, SX1278LR.RegIrqFlagsMask );
 
        SX1278Write( REG_LR_HOPPERIOD, 0xff );//�ر���Ƶ�������Ͻ�0x00�ǹرգ��д�ȷ��  
   
        // ���ý��յ����ݰ�ģʽ Rx single mode       
        SX1278LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
        //��Ҫ�ѽ��ջ�����գ�������ʱ��д����������д
        //memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );
        //ClearBuffer();
        
        PacketTimeout = LoRaSettings.RxPacketTimeout;
        RxTimeoutTimer = sys_time; //���ϵͳʱ��
        RFLRState = RFLR_STATE_RX_RUNNING;
        break;
    case RFLR_STATE_RX_RUNNING:        
        SX1278Read( REG_LR_IRQFLAGS, &SX1278LR.RegIrqFlags );
	if( SX1278LR.RegIrqFlags & RFLR_IRQFLAGS_RXDONE_MASK )
        {
            RxTimeoutTimer = sys_time;
            // Clear Irq
            SX1278Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
            RFLRState = RFLR_STATE_RX_DONE;
        }
        // Rx single mode
        {
            if( ( sys_time - RxTimeoutTimer ) > PacketTimeout )   //���ճ�ʱ���
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
	SX1278Read( REG_LR_HOPCHANNEL, &SX1278LR.RegHopChannel );
        if( !( SX1278LR.RegHopChannel & RFLR_HOPCHANNEL_PAYLOAD_CRC16_ON ) )
        {
            // Rx single mode
            {
                RFLRState = RFLR_STATE_RX_INIT;
            }
            break;
        }
        //�����Ƕ�������ز���
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
       
         // Rx single mode
        {
            SX1278LR.RegFifoAddrPtr = SX1278LR.RegFifoRxBaseAddr;       
            SX1278Write( REG_LR_FIFOADDRPTR, SX1278LR.RegFifoAddrPtr );    //?����??����?��y?Y

            if( LoRaSettings.ImplicitHeaderOn )
            {//���û���õ�
                RxPacketSize = SX1278LR.RegPayloadLength;
                SX1278ReadFifo( RFBuffer, SX1278LR.RegPayloadLength );                
            }
            else
            {//�õ�������һ��
                SX1278Read( REG_LR_NBRXBYTES, &SX1278LR.RegNbRxBytes );
                RxPacketSize = SX1278LR.RegNbRxBytes;                
                SX1278ReadFifo( RFBuffer, SX1278LR.RegNbRxBytes );  
            }
        }
        
        // Rx single mode
        RFLRState = RFLR_STATE_RX_INIT;
        result = RF_RX_DONE;
        break;
    case RFLR_STATE_RX_TIMEOUT:
        RFLRState = RFLR_STATE_RX_INIT;
        //urt1SendChar(0xbe);
        result = RF_RX_TIMEOUT;
        break;
    case RFLR_STATE_TX_INIT:

        SX1278LoRaSetOpMode( RFLR_OPMODE_STANDBY );
        {
            SX1278LR.RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            
        }
        SX1278Write( REG_LR_IRQFLAGSMASK, SX1278LR.RegIrqFlagsMask );

        // Initializes the payload size
        SX1278LR.RegPayloadLength = TxPacketSize;
        SX1278Write( REG_LR_PAYLOADLENGTH, SX1278LR.RegPayloadLength );
        
        SX1278LR.RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
        SX1278Write( REG_LR_FIFOTXBASEADDR, SX1278LR.RegFifoTxBaseAddr );

        SX1278LR.RegFifoAddrPtr = SX1278LR.RegFifoTxBaseAddr;
        SX1278Write( REG_LR_FIFOADDRPTR, SX1278LR.RegFifoAddrPtr );
        
        // Write payload buffer to LORA modem
        SX1278WriteFifo( RFBuffer, SX1278LR.RegPayloadLength );

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
        //��������Ժ�ϵͳ�������״̬�����Ҫ�������ݣ����������������ճ�ʼ��
        RFLRState = RFLR_STATE_RX_INIT;
        //result = RF_TX_DONE;
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
}


