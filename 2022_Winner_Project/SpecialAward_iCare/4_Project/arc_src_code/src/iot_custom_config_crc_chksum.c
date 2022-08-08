/*
 * iot_custom_config_crc_chksum.c
 *
 *  Created on: 2019¦~8¤ë4¤é
 *      Author: 902447
 */

#include <iot_custom_config_crc_chksum.h>

uint8_t CRC8(uint8_t data, uint8_t crc)
{
	BYTE_T D, C, NewCRC;

	D.byte = data;
	C.byte = crc;

#ifdef DBG_CONTENT
	DbgPrintf("%x %x %x %x %x %x %x %x\n"
		, D.b7
		, D.b6
		, D.b5
		, D.b4
		, D.b3
		, D.b2
		, D.b1
		, D.b0);
#endif

	NewCRC.b0 = D.b7 ^ D.b6 ^ D.b0 ^ C.b0 ^ C.b6 ^ C.b7;
	NewCRC.b1 = D.b6 ^ D.b1 ^ D.b0 ^ C.b0 ^ C.b1 ^ C.b6;
	NewCRC.b2 = D.b6 ^ D.b2 ^ D.b1 ^ D.b0 ^ C.b0 ^ C.b1 ^ C.b2 ^ C.b6;
	NewCRC.b3 = D.b7 ^ D.b3 ^ D.b2 ^ D.b1 ^ C.b1 ^ C.b2 ^ C.b3 ^ C.b7;
	NewCRC.b4 = D.b4 ^ D.b3 ^ D.b2 ^ C.b2 ^ C.b3 ^ C.b4;
	NewCRC.b5 = D.b5 ^ D.b4 ^ D.b3 ^ C.b3 ^ C.b4 ^ C.b5;
	NewCRC.b6 = D.b6 ^ D.b5 ^ D.b4 ^ C.b4 ^ C.b5 ^ C.b6;
	NewCRC.b7 = D.b7 ^ D.b6 ^ D.b5 ^ C.b5 ^ C.b6 ^ C.b7;

	return NewCRC.byte;
}

void HxGetCRC8(uint8_t *u8pData, uint32_t u32DataSize, uint8_t* pu8CRC8, uint16_t* pu16CheckSum)
{
	uint32_t len;
	uint8_t  u8crc = 0;
	uint16_t u16CheckSum = 0;
	for(len=0; len<u32DataSize; len++)
	{
		u8crc = CRC8(u8pData[len], u8crc);
		u16CheckSum += u8pData[len];
	}

	if(pu8CRC8 != NULL)
		*pu8CRC8 = u8crc;
	if(pu16CheckSum != NULL)
		*pu16CheckSum = u16CheckSum;
}
