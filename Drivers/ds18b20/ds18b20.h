/*
 * ds18b20.h
 *
 *  Created on: Dec 2, 2024
 *      Author: lcastedo
 */

#ifndef DS18B20_DS18B20_H_
#define DS18B20_DS18B20_H_


enum DS18B20_STATUS
{
	DS18B20_OK = 0, DS18B20_BUSY, DS18B20_NONE, DS18B20_NOCONV
};

enum DS18B20_STATUS DS18B20_SampleTemp(void);
enum DS18B20_STATUS DS18B20_ReadTemp(float *reading);

#endif /* DS18B20_DS18B20_H_ */
