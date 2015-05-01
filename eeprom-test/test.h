#ifndef __TEST_H__
#define __TEST_H__


#include "../core/board.h"
#include "../core/leds.h"


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

#endif
