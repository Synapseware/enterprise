#ifndef __TEST_H__
#define __TEST_H__


#include "../board.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <types.h>

#include <events/events.h>
#include <uart/uart.h>

#include <drivers/at24c/at24c.h>
#include <twi/i2c.h>


static const char IC2_OK_MSG[] PROGMEM = { "I2C_OK" };


const static uint8_t SLEEPY_EYES[] PROGMEM = {
	3, 3, 7, 12, 19, 22, 31, 63, 127, 63, 31, 22, 19, 12, 7, 3, 3, 3, 3, 3, 3
};
#define SLEEPY_EYES_LEN sizeof(SLEEPY_EYES)/sizeof(uint8_t)

#endif
