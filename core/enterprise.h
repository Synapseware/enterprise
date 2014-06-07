#ifndef ENTERPRISE_H
#define ENTERPRISE_H

#include "board.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <types.h>

#include <events/events.h>
#include <uart/uart.h>
#include <sermem/sermem.h>

#include "effects.h"


void init(void);
int main(void);


const static uint8_t SLEEPY_EYES[] PROGMEM = {
	0, 3, 7, 12, 19, 22, 31, 63, 127, 255, 127, 63, 31, 22, 19, 12, 7, 3, 0, 0, 0, 0, 0, 0
};
#define SLEEPY_EYES_LEN sizeof(SLEEPY_EYES)/sizeof(uint8_t)

#endif

