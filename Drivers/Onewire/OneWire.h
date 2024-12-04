/*
 * OneWire.h
 *
 *  Created on: Dec 2, 2024
 *      Author: lcastedo
 */

#ifndef ONEWIRE_ONEWIRE_H_
#define ONEWIRE_ONEWIRE_H_

#include <stdint.h>

uint8_t OW_ReadByte(void);
uint8_t OW_Reset(void);
void OW_WriteByte(uint8_t data);

#endif /* ONEWIRE_ONEWIRE_H_ */
