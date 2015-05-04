#include "enterprise.h"




const static uint8_t SLEEPY_EYES[] PROGMEM = {
	3, 3, 7, 12, 19, 22, 31, 63, 127, 63, 31, 22, 19, 12, 7, 3, 3, 3, 3, 3, 3
};
const uint8_t SLEEPY_EYES_LEN = sizeof(SLEEPY_EYES)/sizeof(uint8_t);


//----------------------------------------------------------------------------------------------
// Shows the heart-beat pattern on the debug LED
static uint8_t _val = 0;
static void fadeStatusLed(eventState_t state)
{
	uint8_t _idx = 0;

	if (0 == _val || _idx >= _val)
		dbg_led_off();
	else
		dbg_led_on();

	_idx += 4;
}
static void readNextStatusVal(eventState_t state)
{
	static uint8_t idx = 0;
	_val = pgm_read_byte(&SLEEPY_EYES[idx++]);
	if (idx >= SLEEPY_EYES_LEN)
		idx = 0;
}

//----------------------------------------------------------------------------------------------
// Used to show activity across the serial port
static uint8_t decay = 0;
static void showSerialStatusCallback(eventState_t state)
{
	if (decay > 0)
	{
		decay--;
		return;
	}

	serial_led_off();
}
static void showSerialStatus(void)
{
	decay = 4;
	serial_led_on();
}


// Scratch buffer
char scratch[128];


// UART RX ring buffer
char uart_buffer[UART_RX_BUFFER];
RingBuffer uart_rx_buff(uart_buffer, UART_RX_BUFFER / sizeof(char));


// declare enterprise globals
Uart				uart(&uart_rx_buff);
Events				events(MAX_EVENT_RECORDS);
Sermem				sermem(&uart, scratch, sizeof(scratch) / sizeof(char));
SoundEffects		effects(&events);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// enables the button after a timeout
static uint8_t _wakingUp = 0;
static void enableButton(eventState_t state)
{

	_wakingUp = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// checks the state of the button
static void checkButton(eventState_t state)
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

	//TODO: Button debounce

	// shutdown the CPU and all effects
	effects.off();

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
	events.registerOneShot(enableButton, 8000, EVENT_STATE_NONE);

	// wake back up
	effects.on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// processes a communications request from the serial port
static void processCommRequest(void)
{
	int data = uart.read();
	if (-1 == data)
		return;

	// shutoff effects while processing request
	//effects.off();

	switch (data & 0x5F)
	{
		case 'V':
			uart.write_P(PSTR("\r\nVersion: 0.5\r\n"));
			break;
		default:
			// process the request
			sermem.process(data);
			break;
	}

	//effects.on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initialize the Effects system
void initEffects(void)
{
	// initialize effects
	uart.write_P(PSTR("Initializing effects header\r\n"));
	int samples = effects.init();
	int len = sprintf_P(scratch, PSTR("  Found %d samples on the EEPROM\r\n"), samples);
	uart.write(scratch, len);
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
	events.registerHighPriorityEvent(readNextStatusVal, 1000, EVENT_STATE_NONE);
	events.registerHighPriorityEvent(showSerialStatusCallback, 100, EVENT_STATE_NONE);

	// enable all interrupts
	sei();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// main program loop
int main()
{
	init();

	uart.write_P(PSTR("Enterprise main board booting up.\r\n"));
	sermem.showHelp();

	//initEffects();

	//effects.on();
	//effects.startSample(SFX_EFX_OPENING);

	while(1)
	{
		// process any pending events
		events.doEvents();

		// process any communications data
		processCommRequest();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Should be running @ 8.000kHz - this is the event sync driver method
ISR(TIMER1_COMPA_vect)
{
	// trigger event cycle
	events.sync();
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
	events.registerOneShot(checkButton, 1000, EVENT_STATE_NONE);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// receive buffer interrupt vector
ISR(USART_RX_vect)
{
	showSerialStatus();

	char data = UDR0;
	uart.receiveHandler(data);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// transmit interrupt vector
ISR(USART_TX_vect)
{
	showSerialStatus();

	uart.transmitHandler();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Callback handler for the read complete event
void Effects_readCompleteHandler(uint8_t data)
{
	// pass the received data to the Effects core
	effects.readComplete(data);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Effects_startSampleCompleteHandler(uint8_t result)
{
	// Signal the result of the start sample request
	effects.startSampleComplete(result);
}
