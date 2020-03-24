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

#ifndef __SX1278_HAL_H__
#define __SX1278_HAL_H__
#include "main.h"
void SX1278Write( uint8_t addr, uint8_t data );
void SX1278Read( uint8_t addr, uint8_t *data );
void SX1278WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );
void SX1278ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );
void SX1278WriteFifo( uint8_t *buffer, uint8_t size );
void SX1278ReadFifo( uint8_t *buffer, uint8_t size );
#endif //__SX1278_HAL_H__
