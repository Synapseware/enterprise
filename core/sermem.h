#ifndef _SPIEEPROM_H
#define _SPIEEPROM_H


extern "C" {
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include <drivers/at24c/at24c.h>
}
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


class Sermem
{
public:
	Sermem(Uart* uart);

	void showHelp(void);

	uint8_t putFile(void);
	uint8_t getFile(void);
	void format(void);
	void writeCannedData(void);
	void putstr(const char * pstr);
	void process(char data);
	void getFileCallback(void);

private:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Writes the EEPROM block size value as a string to the user
	void tellBlockSize(void)
	{
		char buff[32];
		memset(buff, 0, sizeof(buff));

		putstr(PSTR("Block size:\n"));
		sprintf_P(buff, PSTR("%d\n"), AT24C1024_PAGE_SIZE);
		_uart->putstr(buff);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Asks the user to send the transfer size
	void askTransferSize(void)
	{
		char buff[16];
		memset(buff, 0, sizeof(buff));

		putstr(PSTR("Transfer size?\n"));

		_uart->read(buff, sizeof(buff)-1);

		_transferSize = atoi(buff);
		_uart->write(TRANSFER_ACK);
	}

	Uart*		_uart;
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
