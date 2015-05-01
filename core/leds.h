#ifndef __LEDS_H__
#define __LEDS_H__

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <inttypes.h>
#include <events/events.h>


const static uint8_t SLEEPY_EYES[] PROGMEM = {
	3, 3, 7, 12, 19, 22, 31, 63, 127, 63, 31, 22, 19, 12, 7, 3, 3, 3, 3, 3, 3
};
#define SLEEPY_EYES_LEN sizeof(SLEEPY_EYES)/sizeof(uint8_t)


//----------------------------------------------------------------------------------------------
// Shows the heart-beat pattern on the debug LED
static volatile uint8_t _val = 0;
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
	uint8_t idx = 0;
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
	decay = 7;
	serial_led_on();
}





#endif
