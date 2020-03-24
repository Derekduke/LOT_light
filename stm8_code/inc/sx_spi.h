#ifndef __SX_SPI_H
#define __SX_SPI_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sx_board.h"

#define SPI_CS0  GPIO_WriteBit(GPIOB , GPIO_Pin_4, RESET)
#define SPI_CS1  GPIO_WriteBit(GPIOB , GPIO_Pin_4, SET)

extern uint8_t spiRecvNum;

void SPI1_Init(void);  
void SX1278Read(u8 addr, u8 *data);
void SX1278Write(u8 addr, u8 data);
void SX1278WriteFifo( uint8_t *buffer, uint8_t size );
void SX1278ReadFifo( uint8_t *buffer, uint8_t size );
/************************end **************************************************/
#endif