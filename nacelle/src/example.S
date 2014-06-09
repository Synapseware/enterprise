#include <avr/io.h>

work    =       16
tmp     =       17
inttmp  =       19
intsav  =       0
SQUARE  =       PD6 

tmconst =       20000000 / 200000
fuzz    =       8
	.section .text
	.global main
main:
        rcall   ioinit
1:
	rjmp    1b
	.global TIMER0_OVF_vect
TIMER0_OVF_vect:
	ldi     inttmp, 256 - tmconst + fuzz
	out     _SFR_IO_ADDR(TCNT0), inttmp
	in      intsav, _SFR_IO_ADDR(SREG)
	sbic    _SFR_IO_ADDR(PORTD), SQUARE
	rjmp    1f
	sbi     _SFR_IO_ADDR(PORTD), SQUARE
	rjmp    2f
1:      cbi     _SFR_IO_ADDR(PORTD), SQUARE
2:
	out     _SFR_IO_ADDR(SREG), intsav
	reti
ioinit:
	sbi     _SFR_IO_ADDR(DDRD), SQUARE
	ldi     work, _BV(TOIE0)
	out     _SFR_IO_ADDR(TIMSK), work
	ldi     work, _BV(CS00)
	out     _SFR_IO_ADDR(TCCR0B), work
	ldi     work, 256 - tmconst
	out     _SFR_IO_ADDR(TCNT0), work
	sei
	ret
	.global __vector_default
__vector_default:
	reti
	.end
