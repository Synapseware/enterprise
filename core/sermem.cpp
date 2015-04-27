#include "sermem.h"



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// connects the ISR with an instance of Sermem
void getFileCallbackHandler(eventState_t state)
{
	if (0 == state)
		return;

	Sermem * obj = (Sermem*)state;
	obj->getFileCallback();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Constructor
Sermem::Sermem(Uart* uart)
{
	_uart					= uart;

	_transferPageComplete	= 0;
	_bytesTransfered		= 0;
	_transferSize			= 0;

	_autoMode				= 0;

	last_rx					= 0;
	last_tx					= 0;
	rx_complete				= 0;
	tx_complete				= 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Receives a file from a serial transfer and stores it in the EEPROM
// Putfile supports the YMODEM protocol
//		
uint8_t Sermem::putFile(void)
{
	uint16_t bytesTransfered = 0;

	if (0 == _transferSize || _transferSize > AT24C1024_MAX_DATA)
	{
		putstr(PSTR("Invalid transfer size\r\n"));
		_uart->write(TRANSFER_ERR);
		return TRANSFER_ERR;
	}

	// begin the page write, starting with page 0
	uint16_t page = 0;
	if (I2C_OK != ee_putByteStart(page++))
	{
		putstr(PSTR("Failed to start EEPROM write sequence\r\n"));
		_uart->write(TRANSFER_ERR);
		return TRANSFER_ERR;
	}

	// we're OK so far, send our initial ACK
	_uart->write(TRANSFER_ACK);

	// get data!
	while (_transferSize)
	{
		// increment the byte counter and save the byte to the EEPROM
		uint8_t data = _uart->read();
		ee_putByte(data);

		// update transfer info
		bytesTransfered++;
		_transferSize--;

		// wait for a pages worth of data, or the end of the write transmission of data
		if (bytesTransfered != AT24C1024_PAGE_SIZE)
			continue;

		// end the write cycle
		ee_putBytesEnd();

		// poll the device for write-complete
		ee_poll();

		// close the current page
		bytesTransfered = 0;
		ee_putByteStart(page++);

		// ack the page
		_uart->write(TRANSFER_ACK);
	}

	// final wrap up
	if (bytesTransfered != 0)
	{
		ee_putBytesEnd();
		ee_poll();
	}

	// nack the end of the transfer
	putstr(PSTR("Transfer complete\r\n"));
	_uart->write(TRANSFER_NACK);

	// return success
	return TRANSFER_SUCCESS;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Gets another byte to send
void Sermem::getFileCallback(void)
{
	_bytesTransfered++;

	// get our data byte
	uint8_t data = ee_read();

	// on the 256th byte, reset the count, terminate the async transmit
	if (_bytesTransfered >= AT24C1024_PAGE_SIZE)
	{
		// reset the byte count
		_bytesTransfered = 0;
		_transferPageComplete = 1;

		// disable the async transmit
		_uart->writeAEnd();
	}
	else
	{
		// decrement total transfer size
		_transferSize--;

		// write the byte we've read
		_uart->write(data);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Reads all the data from the chip and sends it to the host
uint8_t Sermem::getFile(void)
{
	// Step 1:
	// Wait for ACK from host to let us know they are ready to send
	_uart->write(TRANSFER_ACK);

	// Step 2:
	// Get block size for transfer from client
	uint8_t transferSizeMultiple = 0;
	_uart->read((char*)&transferSizeMultiple, sizeof(uint8_t));
	if (0 == transferSizeMultiple)
		return TRANSFER_ERR;
	_transferSize = transferSizeMultiple * 8;

	// Step 3:
	// Let client know how large of a transfer this will be
	uint32_t totalTransferSize = AT24C1024_MAX_DATA;
	_uart->write((char*)&totalTransferSize, sizeof(uint32_t));

	// Step 4:
	// Make sure client ACK'd our size
	if (TRANSFER_ACK != _uart->read())
		return TRANSFER_ERR;

	// Step 5:
	// Send entire file to client!  :)

	// spin while transfer in progress
	uint16_t page = 0;
	while(_transferSize && page < AT24C1024_PAGE_COUNT)
	{
		// setup for sequential read from EEPROM chip
		ee_setpage(page++);

		// enable async transmit
		//TODO: GetfileCallback is busted...
		//_uart->beginTransmit(getFileCallbackHandler);

		// send first byte to start async transmit routine
		_uart->write(ee_read());
		_bytesTransfered++;

		// wait for page to finish transmitting
		while (!_transferPageComplete)
		{}
		_transferPageComplete = 0;

		// get ACK code from client
		if (TRANSFER_ACK != _uart->read())
			break;
	}

	// make sure async mode is disabled
	_uart->writeAEnd();

	// send final NACK
	_uart->write(TRANSFER_NACK);

	// return success
	return TRANSFER_SUCCESS;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// formats the chip by writing 0xff to all cells
void Sermem::format(void)
{
	_uart->write('A');
	putstr(PSTR("Are you sure want to format?  This will erase all EEPROM data.\r\n"));
	if ((_uart->read() & 0x5F) == 'Y')
		_uart->write('A');
	else
	{
		putstr(PSTR("Format aborted.\r\n"));
		_uart->write('N');
		return;
	}

	uint16_t length = 0;
	uint16_t page = 0;
	while (page < AT24C1024_PAGE_COUNT)
	{
		ee_setpage(page++);

		while (length++ < AT24C1024_PAGE_SIZE)
			ee_putByte(0xFF);
		length = 0;

		ee_putBytesEnd();

		putstr(PSTR("."));
	}
	putstr(PSTR("\r\n"));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Writes the EEPROM block size value as a string to the user
void Sermem::tellBlockSize(void)
{
	char buff[32];
	memset(buff, 0, sizeof(buff));

	putstr(PSTR("Block size:\r\n"));
	sprintf_P(buff, PSTR("%d\r\n"), AT24C1024_PAGE_SIZE);
	_uart->putstr(buff);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Asks the user to send the transfer size
void Sermem::askTransferSize(void)
{
	char buff[16];
	memset(buff, 0, sizeof(buff));

	putstr(PSTR("Transfer size?\r\n"));

	uint32_t transferSize = _transferSize;

	if (_uart->getstr(buff, sizeof(buff)-1) != buff)
		transferSize = (uint32_t) atoi(buff);

	if (!_autoMode)
	{
		memset(buff, 0, sizeof(buff));
		sprintf_P(buff, PSTR("%d"), transferSize);

		putstr(PSTR("Transfer size: "));
		_uart->putstr(buff);
		putstr(PSTR(".\r\n"));
	}

	// update the global value
	if (transferSize > 0)
		_transferSize = transferSize;

	_uart->write(TRANSFER_ACK);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Sermem::putstr(const char * pstr)
{
	if (_autoMode)
		return;

	_uart->putstr_P(pstr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Sermem::showHelp(void)
{
	putstr(PSTR("Sound Effects EEPROM Commands:\r\n\
  A: Auto-mode for scripted interfacing\r\n\
  M: Manual-mode for non-scripted interfacing\r\n\
  R: Retrieves the entire contents of the EEPROM\r\n\
  W: Stores a file on the EEPROM\r\n\
  F: Formats the EEPROM\r\n\
  B: Read block size\r\n\
  T: Set transfer size\r\n\
  H: Display help (this text)\r\n"));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Processes serial data
void Sermem::process(char data)
{
	switch (data & 0x5F) // mask all characters to upper-case ASCII :)
	{
		// auto-mode
		case CMD_MODE_AUTO:
			_uart->write(TRANSFER_ACK);
			_autoMode = 1;
			break;

		// manual mode
		case CMD_MODE_MANUAL:
			_autoMode = 0;
			putstr(PSTR("Manual mode now selected.\r\n"));
			break;

		// retrieve EEPROM contents
		case CMD_READ:
			getFile();
			break;

		// write new EEPROM data
		case CMD_WRITE:
			putstr(PSTR("Waiting for file transfer.\r\n"));
			if (putFile())
				putstr(PSTR("File successfully transfered to EEPROM.\r\n"));
			else
				putstr(PSTR("File transfer timed out.  Please send file within 30 seconds.\r\n"));
			break;

		// format the EEPROM
		case CMD_FORMAT:
			format();
			break;

		// retrieve the block size
		case CMD_BLOCK_SIZE:
			tellBlockSize();
			break;

		// get the transfer size
		case CMD_TRANSFER_SIZE:
			askTransferSize();
			break;
		
		// show the help text
		default:
			showHelp();
			break;
	}
}

