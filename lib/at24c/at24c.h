#ifndef __AT24C_H__
#define __AT24C_H__


#include <inttypes.h>
#include <stdbool.h>

#include <twi/i2c.h>
#include <asyncTypes.h>

#ifndef TWI_SPEED
	#warning TWI_SPEED not defined.  Defaulting to 100kHz
	#define TWI_SPEED			100
#endif


#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Setup AT24C1024B info
// 0b10100000 = 160 or 0xA0
#define AT24C1024_ADDRESS		0b10100000
#define AT24C1024_DEVICE_PAGES	512
#define AT24C1024_DEVICE_SIZE	131072
#define AT24C1024_PAGE_SIZE		256
#define AT24C1024_DEVICE_COUNT	2
#define AT24C1024_PAGE_COUNT	AT24C1024_DEVICE_PAGES * AT24C1024_DEVICE_COUNT
#define AT24C1024_MAX_DATA		AT24C1024_DEVICE_SIZE * AT24C1024_DEVICE_COUNT

// async enumerations
#define ASYNC_COMPLETE			0x00
#define ASYNC_IDLE				0xFF

// send enum values
#define ASYNC_SEND_START		1
#define ASYNC_SEND_DEVICE		2
#define ASYNC_SEND_ADDRMSB		3
#define ASYNC_SEND_ADDRLSB		4
#define ASYNC_SEND_STOP			5

// read next enum values
#define ASYNC_NEXT_START		1
#define ASYNC_NEXT_DEVICE		2
#define ASYNC_NEXT_NACK			3
#define ASYNC_NEXT_READ			4
#define ASYNC_NEXT_STOP			5

#define ASYNC_MULTI_START		1
#define ASYNC_MULTI_DEVICE		2
#define ASYNC_MULTI_ADDRMSB		3
#define ASYNC_MULTI_ADDRLSB		4
#define ASYNC_MULIT_READ_START	5
#define ASYNC_MULTI_READ		6
#define ASYNC_MULTI_NEXT		7
#define ASYNC_MULTI_STOP		8



//---------------------------------------------------------------------------


void ee_mapdevice(uint16_t page);

EE_STATUS ee_setpage(uint16_t page);
void ee_setpageA(uint16_t page, fStatusCallback callBack);

uint8_t ee_read(void);
void ee_readA(fStatusCallback callBack);

EE_STATUS ee_readBytes(uint16_t page, void * data, int length);
void ee_readBytesA(uint16_t page, void * data, int length, fStatusCallback callBack);

EE_STATUS ee_writePage(uint16_t page, void * data);
EE_STATUS ee_writeBytes(uint16_t page, void * data, int length);

EE_STATUS ee_putByteStart(uint16_t page);
void ee_putByte(uint8_t data);
void ee_poll(void);
bool ee_busy(void);
void ee_end(void);

void ee_init(unsigned short bitrateKHz);



#ifdef  __cplusplus
}
#endif

#endif
