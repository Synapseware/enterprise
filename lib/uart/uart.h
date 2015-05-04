#ifndef __UART_H__
#define __UART_H__



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <types.h>

#include <avr/io.h>
#include <util/setbaud.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <asyncTypes.h>
#include <collections/ringbuffer.h>


class Uart
{
public:
	typedef void (*uart_rx_callback_t)(char);
	typedef void (*uart_tx_callback_t)(void);

	Uart(RingBuffer* rx_buff);

	// writes a character of data to the UART
	void write(char x);

	// writes a string from SRAM to the UART
	void write(const char* pstr);

	// writes a block of data from SRAM to the UART
	void write(const char * buffer, int length);

	// writes a string from FLASH to the UART
	void write_P(const char* pstr);

	// writes a block of data from FLASH to the UART
	void write_P(const char* pstr, int length);

	// returns true if data is waiting in the receivers register
	bool dataWaiting(void);

	// reads a byte of data from the buffer or returns -1 if not data available
	int read(void);
	void readA(uart_rx_callback_t callBack);
	void readAEnd(void);
	void read(char * buffer, uint16_t length);
	char * getstr(char * pstr, uint16_t max);

	// blocks the caller until data is available
	void wait(void);

	void receiveHandler(char data);
	void transmitHandler(void);

private:
	RingBuffer *			_rx_buff;
	uart_tx_callback_t		_tx_callback;
	uart_rx_callback_t		_rx_callback;
	const char*				_txAsyncData;
	uint16_t				_txAsyncLen;
	char					_asyncBusy;


	void drain_rx(void)
	{
		// drain the hardware buffer
		unsigned char data;
		while (UCSR0A & (1<<RXC0))
			data = UDR0;

		// drain the RX ring buffer
		if (NULL != _rx_buff)
			while(-1 != _rx_buff->Get());
	}
};


#endif
