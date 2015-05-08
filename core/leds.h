#ifndef __LEDS_H__
#define __LEDS_H__

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <inttypes.h>
#include <events/events.h>
#include "board.h"


void fadeStatusLed(eventState_t state);
void readNextStatusVal(eventState_t state);
void showSerialStatusCallback(eventState_t state);
void showSerialStatus(void);

#endif
