#ifndef __TEST_H__
#define __TEST_H__


#include "../core/board.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <types.h>

#include <events/events.h>
#include <uart/uart.h>

#include <at24c/at24c.h>
#include <twi/i2c.h>
#include <collections/ringbuffer.h>


static const char I2C_UKN_MSG[]		PROGMEM = { "Unknown" };
static const char I2C_OK_MSG[]		PROGMEM = { "OK" };
static const char I2C_NO_DEV_MSG[]	PROGMEM = { "No device found" };
static const char I2C_ERROR_MSG[]	PROGMEM = { "Error" };

static const uint8_t SLEEPY_EYES[] PROGMEM = {
	91, 15, 63, 15, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};
#define SLEEPY_EYES_LEN sizeof(SLEEPY_EYES)/sizeof(uint8_t)

#endif
