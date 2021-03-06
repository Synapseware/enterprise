#include "test.h"



// declare enterprise globals
char buff[UART_RX_BUFFER]; // buffer is defaulting to 64 bytes
RingBuffer			ring_buff(buff, UART_RX_BUFFER / sizeof(char));

Uart				uart(&ring_buff);
Events				events(MAX_EVENT_RECORDS);




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initialize the hardware
void init(void)
{
	play_led_en();
	dbg_led_en();
	serial_led_en();
	twi_led_en();

	twi_led_off();
	play_led_off();
	dbg_led_off();
	serial_led_off();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// establish event timer & handler
	// timer1, event timer
	// Set CTC mode (Clear Timer on Compare Match) (p.133)
	// Have to set OCR1A *before anything else*, otherwise it gets reset to 0!
	PRR		&=	~(1<<PRTIM1);
	OCR1A	=	(F_CPU / SAMPLE_RATE);
	TCCR1A	=	0;
	TCCR1B	=	(1<<WGM12) |	// CTC
				(1<<CS10);
	TIMSK1	=	(1<<OCIE1A);

	events.setTimeBase(SAMPLE_RATE);

	// setup sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// setup switch input
	SWITCH_DDR		&= ~(1<<SWITCH_PIN);
	SWITCH_PINS		|= (1<<SWITCH_PIN);
	SWITCH_PCMSK	|= (1<<SWITCH_PCINT);
	PCICR			|= (1<<SWITCH_PCICR);

	// initialize the TWI interface
	i2cInit(400);

	events.registerHighPriorityEvent(fadeStatusLed, 0, EVENT_STATE_NONE);
	events.registerHighPriorityEvent(readNextStatusVal, 750, EVENT_STATE_NONE);
	events.registerHighPriorityEvent(showSerialStatusCallback, 50, EVENT_STATE_NONE);

	// enable all interrupts
	sei();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* mapStatus(EE_STATUS status, char* writeto)
{
	const char* pszflash = I2C_UKN_MSG;
	switch (status)
	{
		case I2C_OK:
			pszflash = I2C_OK_MSG;
			break;
		case I2C_ERROR_NODEV:
			pszflash = I2C_NO_DEV_MSG;
			break;
		case I2C_ERROR:
			pszflash = I2C_ERROR_MSG;
			break;
	}

	strcpy_P(writeto, pszflash);

	return writeto;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void showData(char* buffer, int length)
{
	char msg[128];
	sprintf_P(msg, PSTR("Reading %d bytes from EEPROM\r\n"), length);
	uart.write(msg);

	EE_STATUS status = ee_readBytes(0, buffer, length);
	if (I2C_OK == status)
	{
		uart.write_P(PSTR("Read OK\r\n"));

		uart.write_P(PSTR("-----------------------------------------------------\r\n\""));
		uart.write(buffer, length - 1);
		uart.write_P(PSTR("\"\r\n-----------------------------------------------------\r\n"));
	}
	else
	{
		sprintf_P(msg, PSTR("  Failed to write: code (%d) = %s\r\n"), status, mapStatus(status, buffer));
		uart.write(msg);
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void processUserData(void)
{
	char data[256], msg[128];
	char* result;
	int
		address = 0,
		page = 0;
	EE_STATUS status = I2C_OK;

	while(1)
	{
		uart.write_P(PSTR("Enter a line of text to write to the EEPROM\r\n"));
		result = uart.getstr(data, sizeof(data));
		if (result == data)
		{
			uart.write_P(PSTR("No data received.\r\n"));
			continue;
		}

		uart.write_P(PSTR("  Received the following block of data:\r\n"));
		uart.write_P(PSTR("-----------------------------------------------------\r\n\""));
		uart.write(data, result - data);
		uart.write_P(PSTR("\"\r\n-----------------------------------------------------\r\n\r\n"));

		int length = result - data + 1;
		sprintf_P(msg, PSTR("  Writing %d bytes to EEPROM\r\n"), length);
		uart.write(msg);

		status = ee_writeBytes(page, data, length);
		if (I2C_OK == status)
		{
			uart.write_P(PSTR("  Successfully wrote data to EEPROM.  Polling write status...\r\n"));
			ee_poll();
			uart.write_P(PSTR("Done\r\n"));

			showData(data, length);
		}
		else
		{
			sprintf_P(msg, PSTR("  Failed to write: code (%d) = %s\r\n"), status, mapStatus(status, data));
			uart.write(msg);
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int main(void)
{
	init();

	while(1)
	{
		// process any pending events
		events.doEvents();

		processUserData();
	}

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Should be running @ 8.000kHz - this is the event sync driver method
ISR(TIMER1_COMPA_vect)
{
	// trigger event cycle
	events.sync();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// receive buffer interrupt vector
ISR(USART_RX_vect)
{
	showSerialStatus();

	char data = UDR0;
	uart.receiveHandler(data);
}
