#include "enterprise.h"


// ring buffers...
char uart_buffer[UART_RX_BUFFER];
RingBuffer uart_rx_buff(uart_buffer, UART_RX_BUFFER * sizeof(char));

// declare enterprise globals
Uart				uart(&uart_rx_buff);
Events				events(MAX_EVENT_RECORDS);
Sermem				sermem(&uart);
SoundEffects		effects(&events);



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
// uart receive callback method
//	gets called by the UART/USART when a byte of data has been received
volatile char _rxData = 0;
volatile char _dataReceived = 0;
static void receiveCallback(char data)
{
	_rxData = data;
	_dataReceived = 0xff;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// processes a communications request from the serial port
static void processCommRequest(void)
{
	if (!_dataReceived)
		return;

	// don't process any more coms data
	uart.readAEnd();

	// setup the UART receive interrupt handler
	_dataReceived = 0;

	// shutoff effects while processing request
	effects.off();

	switch (_rxData & 0x5F)
	{
		case 'V':
			uart.putstr_P(PSTR("\r\nVersion: 0.5\r\n"));
			break;
		default:
			// process the request
			sermem.process(_rxData);
			break;
	}

	uart.readA(&receiveCallback);
	effects.on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// enables the button after a timeout
volatile uint8_t _wakingUp = 0;
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
// Initialize the Effects system
void initEffects(void)
{
	char message[10];

	// initialize effects
	uint16_t samples = effects.init();
	sprintf(message, "%d", samples);

	uart.putstr_P(PSTR("Found "));
	uart.putstr(message);
	uart.putstr_P(PSTR(" effects on EEPROM\r\n"));
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
// main program loop
int main()
{
	init();

	uart.putstr_P(PSTR("Enterprise main board booting up.\r\n"));
	sermem.showHelp();

	initEffects();

	effects.on();
	effects.startSample(SFX_EFX_OPENING);

	// setup the UART receive interrupt handler
	uart.readA(&receiveCallback);

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
	serial_led_on();

	char data = UDR0;
	uart.receiveHandler(data);

	serial_led_off();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// transmit interrupt vector
ISR(USART_TX_vect)
{
	serial_led_on();

	uart.transmitHandler();

	serial_led_off();
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
