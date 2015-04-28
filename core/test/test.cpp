#include "test.h"



// declare enterprise globals
Uart				uart;
Events				events(MAX_EVENT_RECORDS);



volatile uint8_t _idx = 0;
volatile uint8_t _val = 0;
static void fadeStatusLed(eventState_t state)
{
	if (_idx == 0)
		dbg_led_on();
	else if (_idx >= _val)
		dbg_led_off();
	_idx += 4;
}

volatile uint8_t _i = 0;
static void readNextStatusVal(eventState_t state)
{
	_val = pgm_read_byte(&SLEEPY_EYES[_i++]);
	if (_i >= SLEEPY_EYES_LEN)
		_i = 0;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initialize the hardware
void init(void)
{
	play_led_en();
	dbg_led_en();
	serial_led_en();

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

	events.registerHighPriorityEvent(fadeStatusLed, 0, EVENT_STATE_NONE);
	events.registerEvent(readNextStatusVal, 750, EVENT_STATE_NONE);

	// enable all interrupts
	sei();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* mapStatus(EE_STATUS status)
{
	switch (status)
	{
		case I2C_OK:
			return IC2_OK_MSG;
	}

	return NULL;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int main(void)
{
	char data[128], msg[128];
	char* result;
	int address = 0;
	int page = 0;
	EE_STATUS status = I2C_OK;

	while(1)
	{
		uart.putstr_P(PSTR("Enter a line of text to write to the EEPROM\r\n"));
		result = uart.getstr(data, sizeof(data));
		if (result == data)
		{
			uart.putstr_P(PSTR("No data received.\r\n"));
			continue;
		}

		int length = result - data;
		sprintf_P(msg, PSTR("Writing %d bytes to EEPROM\r\n", length));
		uart.putstr(msg);

		status = ee_writeBytes(page, length, data);
		if (I2C_OK == status)
			uart.putstr_P(PSTR("Successfully wrote data to EEPROM\r\n"));
		else
		{
			sprintf_P(msg, PSTR("Failed to write.  Error code = %d\r\n"), status);
		}
	}

	return 0;
}