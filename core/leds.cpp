#include "leds.h"

static const uint8_t SLEEPY_EYES[] PROGMEM = {
	3, 3, 7, 12, 19, 22, 31, 63, 127, 63, 31, 22, 19, 12, 7, 3, 3, 3, 3, 3, 3
};
static const uint8_t SLEEPY_EYES_LEN = sizeof(SLEEPY_EYES)/sizeof(uint8_t);

// decay data
static uint8_t _decay_serial		= 0;

// Decay the LED on times
void decayLeds(void)
{
	if (_decay_serial > 0)
		_decay_serial--;
	else
		serial_led_off();
}

//----------------------------------------------------------------------------------------------
// Shows the heart-beat pattern on the debug LED
static uint8_t _val = 0;
void fadeStatusLed(eventState_t state)
{
	static uint8_t level = 0;

	if (0 == _val || level >= _val)
		dbg_led_off();
	else
		dbg_led_on();

	level += 4;
}
void readNextStatusVal(eventState_t state)
{
	static uint8_t idx = 0;
	_val = pgm_read_byte(&SLEEPY_EYES[idx++]);
	if (idx >= SLEEPY_EYES_LEN)
		idx = 0;

	decayLeds();
}

//----------------------------------------------------------------------------------------------
// Turns on the serial status LED
void showSerialStatus(void)
{
	_decay_serial = 10;
	serial_led_on();
}
