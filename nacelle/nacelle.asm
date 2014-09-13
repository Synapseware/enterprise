;
; ********************************************
; * [Add Project title here]                 *
; * [Add more info on software version here] *
; * (C)2011 by [Matthew Potter]              *
; ********************************************
;
; Included header file for target AVR type
.NOLIST
.INCLUDE "tn2313def.inc" ; Header for ATTINY2313
.LIST


; ================================================================================================
;   R E G I S T E R   D E F I N I T I O N S
; ================================================================================================
.DEF	rmp						= r16
.DEF	rtmp					= r17

.DEF	rREAD_ADDR				= r9
.DEF	rPWM_VAL				= r18
.DEF	rACTIVE_LED				= r19
.DEF	rOFFSET					= r20
.DEF	rSTP_LOOP				= r21
.DEF	rSTP_BIT				= r22
.DEF	rSTP_DATA				= r23


.DEF	rEE_Addr				= r0
.DEF	rEE_Data				= r1

; ================================================================================================
;	C O N S T A N T S
; ================================================================================================
.EQU	TIMER0_DELAY			= 9
.EQU	TIMER1_DELAY			= 600

;	Command bytes:
.EQU	GET_NEW_ON_VALUES		= 0xF0
.EQU	COMMIT_ON_VALUES		= 0xF1
.EQU	UPDATE_ROTATE_SPD		= 0x80
.EQU	UPDATE_ROTATE_DIR		= 0x40

; -------------------------------------------------
; Constants for usitwi.asm
.EQU	ATTINY2313				= 1
.EQU	TW_ADDRESS				= 0x6A


; ================================================================================================
;	D A T A  S E G M E N T
; ================================================================================================
.DSEG
	

; ================================================================================================
;  E E P R O M  S E G M E N T
; ================================================================================================
.ESEG
	.ORG $0000
	;EE_TABLE_DATA:		.DB		255, 63, 31, 15, 7, 0, 0, 0, 255, 63, 31, 15, 7, 0, 0, 0, 255, 63, 31, 15, 7, 0, 0, 0, 255, 63, 31, 15, 7, 0, 0, 0
	EE_TABLE_DATA:		.DB		0, 7, 15, 31, 63, 255, 200, 150, 120, 90, 70, 50, 30, 20, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
								

; ================================================================================================
;   R E S E T   A N D   I N T   V E C T O R S
; ================================================================================================
.CSEG
.ORG $0000
	rjmp Main					; 1 - RESET
	reti						; 2 - INT0
	reti						; 3 - INT1
	reti						; 4 - TIMER1 CAPT
	rjmp ISR_TimerCompare1A		; 5 - TIMER1 COMPA
	reti						; 6 - TIMER1 OVF
	reti						; 7 - TIMER0 OVF
	reti						; 8 - USART0, RX
	reti						; 9 - USART0, UDRE
	reti						; 10 - USART0, TX
	reti						; 11 - ANALOG COMP
	reti						; 12 - PCINT
	reti						; 13 - TIMER1 COMPB
	rjmp ISR_TimerCompare0A		; 14 - TIMER0 COMPA
	reti						; 15 - TIMER0 COMPB
	rjmp ISR_USIStart			; 16 - USI START
	rjmp ISR_USIOverflow		; 17 - USI OVERFLOW
	reti						; 18 - EE READY
	reti						; 19 - WDT OVERFLOW


; ================================================================================================
; Timer0, compare A handler
; Calls the PWM Channel Driver for processing the next LED
; ================================================================================================
ISR_TimerCompare0A:
	push	rmp
	in		rmp, SREG
	push	rmp

	rcall	STP16_Refresh

	pop		rmp
	out		SREG, rmp
	pop		rmp
	reti


; ================================================================================================
; Timer1, compare A handler
; Increments the offset value
; ================================================================================================
ISR_TimerCompare1A:
	push	rmp
	in		rmp, SREG
	push	rmp

	rcall	STP16_UpdateOffset

	pop		rmp
	out		SREG, rmp
	pop		rmp
	reti


; ================================================================================================
; Main
; ================================================================================================
Main:
	ldi		rmp, LOW(RAMEND)			; Init LSB stack
	out		SPL, rmp

	rcall	Hardware_Init				; Initialize the hardware
	rcall	EEPROM_Init					; Init EEPROM module
	rcall	STP16_Init					; Init the STP16 LED driver
	rcall	USI_Init					; Init USI module

	sei

Loop:
	wdr
	rcall	USI_ReadNext				; get TWI data  :)
	rcall	ProcessTWIData				; process the TWI request data
	rjmp	Loop


; ================================================================================================
; Processes the TWI request
; ================================================================================================
ProcessTWIData:
	lds		rmp, USI_RESULT
	
	cpi		rmp, GET_NEW_ON_VALUES		; get new on_values
	breq	_GET_NEW_ON_VALUES
	
	cpi		rmp, COMMIT_ON_VALUES		; save current on_values
	breq	_COMMIT_ON_VALUES
	
	cpi		rmp, UPDATE_ROTATE_SPD		; update rotate speed
	breq	_UPDATE_ROTATE_SPD

	cpi		rmp, UPDATE_ROTATE_DIR		; update rotate direction
	breq	_UPDATE_ROTATE_DIR
	
	rjmp	_procDone

_GET_NEW_ON_VALUES:
	; wait for data
	rcall	STP16_GetChannelCount		; get channel count (returns in rmp)
	lds		rtmp, USI_RX_AVAIL
	cp		rtmp, rmp
	brlo	_GET_NEW_ON_VALUES
	clr		rEE_Addr
_get_data:
	rcall	USI_ReadNext
	lds		rmp, USI_RESULT
	out		EEAR, rEE_Addr				; Pass data address to EEAR
	out		EEDR, rmp					; Pass data to EEDR
	sbi		EECR, EEMPE					; Write logical one to EEMPE
	sbi		EECR, EEPE					; Start eeprom write by setting EEPE
	inc		rEE_Addr
	dec		rtmp
	brne	_get_data
	rjmp	_procDone

_COMMIT_ON_VALUES:
	; Update to not use buffer, if possible :)
	ldi		rmp, 0
	sts		EE_ADDR, rmp
	;rcall	STP16_GetChannelCount		; get channel count (returns in rmp)
	;rcall	STP16_GetDataBuffer			; load X with buffer address
	;rcall	EEPROM_WriteBlock
	rjmp	_procDone

_UPDATE_ROTATE_SPD:
	; wait for data
	lds		rtmp, USI_RX_AVAIL
	cpi		rtmp, 2
	brlo	_UPDATE_ROTATE_SPD

	rcall	USI_ReadNext
	lds		rmp, USI_RESULT					; high order delay byte
	rcall	USI_ReadNext
	lds		rtmp, USI_RESULT				; low order delay byte
	out		OCR1AH, rmp
	out		OCR1AL, rtmp
	rjmp	_procDone

_UPDATE_ROTATE_DIR:
	lds		rtmp, USI_RX_AVAIL
	tst		rtmp
	breq	_UPDATE_ROTATE_DIR

	rcall	USI_ReadNext
	lds		rmp, USI_RESULT
	rcall	STP16_SetDirection
	rjmp	_procDone

_procDone:
	ret


; ================================================================================================
; Initialize hardware (timers, ports, etc)
; ================================================================================================
Hardware_Init:
	; setup timer0
	; timer0 is used to refresh the display,
	; and must be frequent enough to avoid
	; visual flicker
	; 20,000,000 / 64 = 312,500 ticks/sec
	; 312,500 / TIMER0_DELAY+1 (10) = 31,250 ints/sec
	; 16 * 256/8 * 60hz = 30,720, so we are OK!  :)
	ldi 	rmp, (1<<WGM01)						; CTC mode
	out 	TCCR0A, rmp
	ldi 	rmp, (1<<CS01) | (1<<CS00)			; CLK/64
	out 	TCCR0B, rmp
	ldi 	rmp, TIMER0_DELAY					; 
	out 	OCR0A, rmp
	in		rmp, TIMSK							; enable the timer
	ori		rmp, (1<<OCIE0A)
	out		TIMSK, rmp

	; setup timer1
	; timer1 is used to increment the offset index.
	; CTC, clk/1024, OCRIA = TIMER1_DELAY (1200)
	; 20,000,000 / 1024 = 19,531.25 / 1200 ~= 16Hz
	; So, 16 times a second, we change the offset.
	ldi		rmp, (1<<WGM12)						; CTC
	ori		rmp, (1<<CS12) | (1<<CS10)			; CLK/1024
	out		TCCR1B, rmp
	ldi		r19, HIGH(TIMER1_DELAY)				; high order delay byte
	ldi		r20, LOW(TIMER1_DELAY)				; low order delay byte
	out		OCR1AH, r19
	out		OCR1AL, r20
	in		rmp, TIMSK							; enable the timer
	ori		rmp, (1<<OCIE1A)
	out		TIMSK, rmp

	; setup PB0 as debug output
	in		rmp, DDRB
	ori		rmp, (1<<PINB0)
	out		DDRB, rmp

	; check for WDRF
	in		rmp, MCUSR
	sbrs	rmp, WDRF
	rjmp	_init_cont
	sbi		PINB, PINB0
	rjmp	PC

	; setup WatchDog!
_init_cont:
	ldi		rmp, (1<<WDCE)|(1<<WDE)|(1<<WDP2)|(WDP0)
	out		WDTCSR, rmp

	clr		rmp
	clr		r19
	clr		r20

	ret


.INCLUDE "stp16cp05.asm"
.INCLUDE "usitwi.asm"
.INCLUDE "eeprom.asm"


