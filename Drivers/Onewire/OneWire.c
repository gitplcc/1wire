/*
 * OneWire.c
 *
 *  Created on: Dec 2, 2024
 *      Author: lcastedo
 */

#include <main.h>
#include <OneWire.h>

#include <stm32f4xx_ll_usart.h>

#include <stdbool.h>

enum OW_CMD {
	OW_SearchRom = 0xF0,
};
static uint8_t OW_ReadBit(void);

static uint8_t OW_ReadBit(void)
{
	UART_HandleTypeDef huart1;

	const uint8_t ReadBitCMD = 0xFF;
    uint8_t RxBit;

    // Send Read Bit CMD
    HAL_UART_Transmit(&huart1, &ReadBitCMD, 1, 1);
    // Receive The Bit
    HAL_UART_Receive(&huart1, &RxBit, 1, 1);

    return (RxBit & 0x01);
}

uint8_t OW_ReadByte(void)
{
    uint8_t RxByte = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        RxByte >>= 1;
        if (OW_ReadBit())
            RxByte |= 0x80;
    }
    return RxByte;
}

uint8_t OW_Reset(void)
{
	UART_HandleTypeDef huart1;
	const uint8_t ResetByte = 0xF0;
	uint8_t PresenceByte;

	LL_USART_SetBaudRate(huart1.Instance, HAL_RCC_GetPCLK2Freq(),
			huart1.Init.OverSampling, 9600);
	// Send reset pulse
	HAL_UART_Transmit(&huart1, &ResetByte, 1, 1);
	// Wait for the presence pulse
	HAL_UART_Receive(&huart1, &PresenceByte, 1, 1);
	LL_USART_SetBaudRate(huart1.Instance, HAL_RCC_GetPCLK2Freq(),
			huart1.Init.OverSampling, 115200);
	// Check presence pulse
	return (PresenceByte != ResetByte ? 1 : 0);
}

bool OW_ScanBus(void)
{
	bool retval;
	int nomatch_mrkr;
	bool done = false;
	int nomatch_last = 0;
	for (;;)
	{
		retval = false;
		if (done)
		{
			done = false;
			return retval;
		}

		if (!OW_Reset())
		{
			nomatch_last = 0;
			return retval;
		}

		uint64_t romcode = 0;
		nomatch_mrkr = 0;
		OW_WriteByte(OW_SearchRom);
		for (int i = 1; i <= 64; i++)
		{
			bool bit = OW_ReadBit();
			bool nbit = OW_ReadBit();
			if (bit == nbit)  // Discrepancy
			{
				if (bit)
				{
					nomatch_last = 0;
					return retval;
				}
				else
				{
					if (i == nomatch_last)
					{
						bit = 1;
						romcode >>= 1;
						romcode |= 0x8000000000000000;
						OW_WriteBit(bit);
					}
					else
					{
						if (i > nomatch_last)
						{
							bit = 0;
							romcode >>= 1;
							nomatch_mrkr = i;
							OW_WriteBit(bit);
						}
						else
						{
							if (!(romcode & 0x8000000000000000))
							{
								nomatch_mrkr = i;
								OW_WriteBit(false);
							}
							else
							{
								OW_WriteBit(true);

							}
						}
					}
				}
			}
			else  // No discrepancy
			{
				romcode >>= 1;
				if (bit)
					romcode |= 0x8000000000000000;
				OW_WriteBit(bit);
			}
		}
		nomatch_last = nomatch_mrkr;
		if (nomatch_last == 0)
			done = true;
		else
			retval = true;
	}
}

void OW_WriteByte(uint8_t data)
{
	UART_HandleTypeDef huart1;

    uint8_t TxBuffer[8];
    for (int i=0; i<8; i++)
    	TxBuffer[i] = ((data & (1 << i)) != 0) ? 0xFF : 0x00;
    HAL_UART_Transmit(&huart1, TxBuffer, 8, 10);
}
