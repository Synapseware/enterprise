#include "enterprise.h"


volatile uint8_t _idx = 0;
volatile uint8_t _val = 0;
void fadeStatusLed(eventState_t state)
{
	if (_idx == 0)
		dbg_led_on();
	if (_idx >= _val)
		dbg_led_off();
	_idx += 4;
}

volatile uint8_t _i = 0;
void readNextStatusVal(eventState_t state)
{
	_val = pgm_read_byte(&SLEEPY_EYES[_i++]);
	if (_i >= SLEEPY_EYES_LEN)
		_i = 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// uart receive callback method
//	gets called by the UART/USART when a byte of data has been received
volatile char _rxData = 0;
volatile char _dataReceived = 0;
void receiveCallback(char data)
{
	_rxData = data;
	_dataReceived = 0xff;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// processes a communications request from the serial port
void processCommRequest(char data)
{
	uartEndReceive();

	sfx_off();

	_dataReceived = 0;
	spie_process(data);
	switch (data & 0x5F)
	{
		case 'V':
			uart_putstrM(PSTR("\r\nVersion: 0.5\r\n"));
			break;
	}

	sfx_on();

	// setup the UART receive interrupt handler
	uartBeginReceive(&receiveCallback);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// enables the button after a timeout
volatile uint8_t _wakingUp = 0;
void enableButton(eventState_t state)
{
	_wakingUp = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// checks the state of the button
void checkButton(eventState_t state)
{
	// ignore any button presses after waking up
	if (_wakingUp)
		return;

	// check switch pin
	if ((SWITCH_PINS & (1<<SWITCH_PIN)) != 0)
	{
		// enable switch interrupt
		SWITCH_PCMSK |= (1<<SWITCH_PCINT);
		return;
	}

	// shutdown the CPU and all effects
	sfx_off();

	// shutoff the LEDs
	dbg_led_off();
	serial_led_off();
	play_led_off();
	twi_led_off();

	// enable button interrupt which will wake us back up
	SWITCH_PCMSK |= (1<<SWITCH_PCINT);
	_wakingUp = 1;

	switch_en();

	// go to sleep
	sleep_cpu();

	//
	registerOneShot(enableButton, 8000, EVENT_STATE_NONE);

	// wake back up
	sfx_on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void init(void)
{
	play_led_en();
	play_led_off();

	dbg_led_en();
	dbg_led_on();

	// initialize effects
	sfx_init();

	// initialize SPI EEPROM support
	spie_init();

	// initialize USART
	uart_init();

	// setup the UART receive interrupt handler
	uartBeginReceive(&receiveCallback);

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

	setTimeBase(SAMPLE_RATE);

	// setup sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// setup switch input
	SWITCH_DDR		&= ~(1<<SWITCH_PIN);
	SWITCH_PINS		|= (1<<SWITCH_PIN);
	SWITCH_PCMSK	|= (1<<SWITCH_PCINT);
	PCICR			|= (1<<SWITCH_PCICR);


	registerHighPriorityEvent(fadeStatusLed, 0, EVENT_STATE_NONE);
	registerEvent(readNextStatusVal, 500, EVENT_STATE_NONE);

	// enable all interrupts
	sei();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// main program loop
int main()
{
	init();

	uart_putstrAM(PSTR("Enterprise main board booting up...\r\n"), 0);

	spie_showHelp();

	sfx_on();

	sfx_startSample(SFX_EFX_OPENING);

	while(1)
	{
		// process any pending events
		eventsDoEvents();

		// check for UART/USART data
		if (_dataReceived)
			processCommRequest(_rxData);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Should be running @ 8.000kHz - this is the event sync driver method
ISR(TIMER1_COMPA_vect)
{
	eventSync();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Fires when the user presses the button
ISR(SWITCH_PCVECT)
{
	if (_wakingUp)
		return;
	
	// disable pin change interrupt
	SWITCH_PCMSK &= ~(1<<SWITCH_PCINT);

	// require user to press button for at least 1/8 second
	registerOneShot(checkButton, 1000, EVENT_STATE_NONE);
}
