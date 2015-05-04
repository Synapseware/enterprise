#ifndef __SERMEM_H__
#define __SERMEM_H__


#include <types.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include <at24c/at24c.h>
#include <uart/uart.h>
#include <events/events.h>

#include "board.h"


#define CMD_MODE_AUTO			'A'
#define CMD_MODE_MANUAL			'M'
#define CMD_BLOCK_SIZE			'B'
#define CMD_TRANSFER_SIZE		'T'
#define CMD_READ				'R'
#define CMD_WRITE				'W'
#define CMD_FORMAT				'F'

#define TRANSFER_ACK			'Y'
#define TRANSFER_NACK			'N'
#define TRANSFER_ERR			'E'
#define TRANSFER_SUCCESS		'K'
#define TRANSFER_DOT			'.'


class Sermem
{
public:
	Sermem(Uart* uart, char* buffer, int length);

	void showHelp(void);
	void process(char data);
	void getFileCallback(void);
	uint8_t format(void);

private:
	uint8_t putFile(void);
	uint8_t getFile(void);
	void tellBlockSize(void);
	void askTransferSize(void);
	void putstr(const char * pstr);

	void ack(void)
	{
		if(!_autoMode)
			_uart->write(TRANSFER_ACK);
	}
	void nack(void)
	{
		if (!_autoMode)
			_uart->write(TRANSFER_NACK);
	}
	void autoMode(void)
	{
		_autoMode = 1;
		ack();
	}
	void manualMode(void)
	{
		_autoMode = 0;
		putstr(PSTR("Manual mode now selected.\r\n"));
	}

	Uart*		_uart;
	char*		_buffer;
	int			_length;

	uint8_t		_transferPageComplete;
	uint16_t	_bytesTransfered;
	uint32_t	_transferSize;

	uint8_t		_autoMode;

	uint8_t		last_rx;
	uint8_t		last_tx;
	uint8_t		rx_complete;
	uint8_t		tx_complete;
};



#endif
