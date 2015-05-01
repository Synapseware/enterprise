#include "uart.h"




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Constructor
Uart::Uart(RingBuffer* rx_buff = NULL)
{
	_uart_rx_buff		= rx_buff;

	_uart_tx_callback	= 0;
	_uart_rx_callback	= 0;
	_txAsyncData		= 0;
	_txAsyncLen			= 0;
	_asyncBusy			= 0;

	_txAsyncCallback	= 0;
	_rxAsyncCallback	= 0;

	// set baud values from macro
	UBRR0H =	UBRRH_VALUE;
	UBRR0L =	UBRRL_VALUE;
	//UBRR0L =	(UBRRN & 0xFF);		// Don't use BAUD macro...
	//UBRR0H =	(UBRRN >> 8);

	UCSR0A =	(0<<U2X0) |			// no clock rate doubling
				(0<<MPCM0);			// no multi-processor communication mode

	// enable uart RX and TX
	UCSR0B =	(1<<RXCIE0) |		// enable receive interrupts
				(0<<TXCIE0) |		// no transmit interrupts
				(0<<UDRIE0) |		// no data register empty interrupts
				(1<<RXEN0) |		// enable RX
				(1<<TXEN0) |		// enable TX
				(0<<UCSZ02) |		// 8 data bits
				(0<<RXB80) |		// 9th bit
				(0<<TXB80);			// 9th bit

	// set 8N1 frame format
	UCSR0C =	(0<<UMSEL01) |		// async UART
				(0<<UMSEL00) |
				(0<<UPM01) |		// disable parity
				(0<<UPM00) |
				(0<<USBS0) |		// 1 stop bit
				(1<<UCSZ01) |		// 8 data bits
				(1<<UCSZ00) |
				(0<<UCPOL0);		// data shapping for async mode should be 0
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Starts asynchronous receive mode
void Uart::readA(uart_rx_callback_t callBack)
{
	// disable interrupts
	//UCSR0B &= ~(1<<RXCIE0);

	// drain the receive buffer
	drain_rx();

	// setup the callback and enable interrupt
	_uart_rx_callback = callBack;
	//UCSR0B |= (1<<RXCIE0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ends asynchronous receive mode
void Uart::readAEnd(void)
{
	//UCSR0B &= ~(1<<RXCIE0);
	_uart_rx_callback = 0;
	drain_rx();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Starts asynchronous transmit mode
void Uart::writeA(uart_tx_callback_t callBack)
{
	_uart_tx_callback = callBack;
	UCSR0B |= (1<<TXCIE0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ends asynchronous transmit mode
void Uart::writeAEnd(void)
{
	UCSR0B &= ~(1<<TXCIE0);
	_uart_tx_callback = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Reads a byte of data from the UART.
int Uart::read(void)
{
	if (NULL != _uart_rx_buff)
	{
		// read data from RX buffer
		return _uart_rx_buff->Get();
	}

	// wait for data
	return dataWaiting()
		? UDR0
		: -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// reads a block of data from the UART.  blocks the caller until complete
void Uart::read(char * buffer, uint16_t length)
{
	while (length--)
	{
		int data = read();
		if (-1 == data)
			continue;

		*buffer++ = (char) data;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// blocks until data is available
void Uart::wait(void)
{
	if (NULL != _uart_rx_buff)
	{
		while (_uart_rx_buff->IsEmpty())
		{}
		return;
	}

	while (0 == (UCSR0A & (1<<RXC0)));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// writes a character of data to the UART
void Uart::write(char data)
{
	// wait for empty receive buffer
	while (0 == (UCSR0A & (1<<UDRE0)));

	// send
	UDR0 = data;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// writes a block of data to the UART
void Uart::write(const char * buffer, uint16_t length)
{
	while (length)
	{
		write(*buffer++);
		length--;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// writes a string from RAM
void Uart::putstr(const char * pstr)
{
	char data;
	while (0 != (data = *pstr++))
		write(data);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// writes a string from PROGMEM (program memory)
void Uart::putstr_P(const char * pstr)
{
	char data;
	while (0 != (data = pgm_read_byte(pstr++)))
		write(data);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// gets a string up to the specified size and stores it in RAM at pstr.
// any null, line feed or new line character will terminate the read
char * Uart::getstr(char * pstr, uint16_t max)
{
	if (0 == max)
		return pstr;

	int data;
	// make sure we have room for the null character
	max -= 1;
	while (max)
	{
		// keep trying to read if there's no data yet
		if (-1 == (data = read()))
			continue;

		// break on a end-of-line type character
		if (data == '\r' || data == '\n' || data == '\0')
			break;

		// write the received data to the output buffer
		*pstr++ = (char) data;
		max--;
	}

	// null terminate the string
	*pstr='\0';

	return pstr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns true if data is avialable in the receive register
bool Uart::dataWaiting(void)
{
	if (NULL != _uart_rx_buff)
	{
		// return true if the buffer isn't empty
		return !_uart_rx_buff->IsEmpty();
	}

	// returns 0 if no data waiting
	return (UCSR0A & (1<<RXC0));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Should be called by the UART RX ISR
void Uart::receiveHandler(char data)
{
	if (NULL != _uart_rx_buff)
		_uart_rx_buff->Put(data);

	if (_uart_rx_callback)
		_uart_rx_callback(data);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Should be called by the UART TX ISR
void Uart::transmitHandler(void)
{
	if (_uart_tx_callback)
		_uart_tx_callback();
}
