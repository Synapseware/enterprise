/*
 * board.h
 *
 * Created: 11/29/2011 8:27:26 PM
 *  Author: Matthew
 */ 


#ifndef BOARD_H_
#define BOARD_H_


//----------------------------------------------------------------------------------------------
// Buttons
#define SWITCH_DDR			DDRD
#define SWITCH_PIN			PORTD6
#define SWITCH_PINS			PIND
#define SWITCH_PORT			PORTD
#define SWITCH_PCINT		PCINT22
#define SWITCH_PCMSK		PCMSK2
#define SWITCH_PCICR		PCIE2
#define SWITCH_PCVECT		PCINT2_vect
#define switch_en()			SWITCH_DDR |= (1<<SWITCH_PIN)


//----------------------------------------------------------------------------------------------
// LED1 - TWI constants and LED


//----------------------------------------------------------------------------------------------
// LED2 - Serial LED
#define SERIAL_LED_PORT		PORTC
#define SERIAL_LED_DDR		DDRC
#define SERIAL_LED_PIN		PORTC1

#define serial_led_off()	SERIAL_LED_PORT |= (1<<SERIAL_LED_PIN)
#define serial_led_on()		SERIAL_LED_PORT &= ~(1<<SERIAL_LED_PIN)
#define serial_led_en()		SERIAL_LED_PORT |= (1<<SERIAL_LED_PIN)


//----------------------------------------------------------------------------------------------
// LED3 - Play LED
#define LED_PLAY_PORT		PORTC
#define LED_PLAY_DDR		DDRC
#define LED_PLAY_PIN		PORTC2

#define play_led_off()		LED_PLAY_PORT |= (1<<LED_PLAY_PIN)
#define play_led_on()		LED_PLAY_PORT &= ~(1<<LED_PLAY_PIN)
#define play_led_en()		LED_PLAY_DDR |= (1<<LED_PLAY_PIN)


//----------------------------------------------------------------------------------------------
// LED4 - Debug LED
#define LED_DBG_PORT		PORTC
#define LED_DBG_DDR			DDRC
#define LED_DBG_LINE		PORTC3

#define dbg_led_off()		LED_DBG_PORT |= (1<<LED_DBG_LINE)
#define dbg_led_on()		LED_DBG_PORT &= ~(1<<LED_DBG_LINE)
#define dbg_led_en()		LED_DBG_DDR |= (1<<LED_DBG_LINE)


//----------------------------------------------------------------------------------------------
// Audio out
#define AUDIO_DDR			DDRD
#define AUDIO_PORT			PORTD
#define AUDIO_OUT_PIN		PORTD3

#define audioOut_en()		AUDIO_DDR |= (1<<AUDIO_OUT_PIN)


//----------------------------------------------------------------------------------------------
// Audio amp power
#define AMPPWR_DDR			DDRD
#define AMPPWR_PORT			PORTD
#define AMPPWR_OUT_PIN		PORTD4

#define ampPwr_en()			AMPPWR_DDR |= (1<<AMPPWR_OUT_PIN)
#define ampPwr_on()			AMPPWR_PORT |= (1<<AMPPWR_OUT_PIN)
#define ampPwr_off()		AMPPWR_PORT &= ~(1<<AMPPWR_OUT_PIN)


//----------------------------------------------------------------------------------------------
// Nacelle power
#define NCL_DDR				DDRD
#define NCL_PORT			PORTD
#define NCL_PIN				PORTD5

#define nacellePower_off()	NCL_PORT |= (1<<NCL_PIN)
#define nacellePower_on()	NCL_PORT &= ~(1<<NCL_PIN)
#define nacellePower_en()	NCL_DDR |= (1<<NCL_PIN)


//----------------------------------------------------------------------------------------------
// STP16CP05
#define STP_OE				PORTB1
#define STP_CLK				PORTB5
#define STP_LE				PORTB2
#define STP_SDI				PORTB4




#endif /* BOARD_H_ */