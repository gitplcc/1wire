/*
 * ds18b20.c
 *
 *  Created on: Dec 2, 2024
 *      Author: lcastedo
 */

#include "ds18b20.h"

#include <OneWire.h>
#include <main.h>
#include <stdbool.h>

enum DS18B20_RESOL
{
	DS18B20_09BITS, DS18B20_10BITS, DS18B20_11BITS, DS18B20_12BITS, DS18B20_MAX
};
static const uint8_t DS18B20_SCMD[DS18B20_MAX] = { 0x1f, 0x3f, 0x5f, 0x7f };
static const uint32_t DS18B20_TCONV[DS18B20_MAX] = { 94, 188, 375, 750 };

static bool ds18b20_busy = false;
static uint32_t ds18b20_tsample;

enum DS18B20_STATUS DS18B20_SampleTemp(void)
{
	if (ds18b20_busy)
		return DS18B20_BUSY;

    if (!OW_Reset())
    	return DS18B20_NONE;

    ds18b20_busy = true;
    OW_WriteByte(0xCC);  // Skip ROM   (ROM-CMD)
    OW_WriteByte(0x44);  // Convert T  (F-CMD)
    ds18b20_tsample = HAL_GetTick() + DS18B20_TCONV[DS18B20_12BITS];
    return DS18B20_OK;
}

enum DS18B20_STATUS DS18B20_ReadTemp(float *reading)
{
    uint8_t raw_lsb, raw_msb;
    uint16_t raw;

    if (!ds18b20_busy)
    	return DS18B20_NOCONV;

    if (HAL_GetTick() < ds18b20_tsample)
		return DS18B20_BUSY;

    if (!OW_Reset())
    	return DS18B20_NONE;

    OW_WriteByte(0xCC);  // Skip ROM         (ROM-CMD)
    OW_WriteByte(0xBE);  // Read Scratchpad  (F-CMD)
    raw_lsb = OW_ReadByte();
    raw_msb = OW_ReadByte();
	raw = (raw_msb << 8) | raw_lsb;
    *reading = (float)raw / 16.0f;
    ds18b20_busy = false;

    return DS18B20_OK;
}
