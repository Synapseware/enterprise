#ifndef __ENTERPRISE_H__
#define __ENTERPRISE_H__

#include "board.h"
#include "leds.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <types.h>

#include <events/events.h>
#include <uart/uart.h>


#include "effects.h"
#include "sermem.h"
#include "leds.h"




int main(void);
void Effects_readCompleteHandler(uint8_t data);
void Effects_startSampleCompleteHandler(uint8_t result);


#endif

