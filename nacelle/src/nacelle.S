#include "nacelle.h"
;#include <drivers/stp16cp05/stp16cp05.S>
;#include <drivers/usitwi.S>
;#include <drivers/eeprom.S>


;
; ********************************************
; * Nacelle Firmware for the Enterprise      *
; * [Add more info on software version here] *
; * (C)2011 by Matthew Potter                *
; ********************************************
;


; ================================================================================================
;   R E G I S T E R   D E F I N I T I O N S
; ================================================================================================


; ================================================================================================
;   C O N S T A N T S
; ================================================================================================
TIMER0_DELAY            = 9
TIMER1_DELAY            = 600

;   Command bytes:
GET_NEW_ON_VALUES       = 0xF0
COMMIT_ON_VALUES        = 0xF1
UPDATE_ROTATE_SPD       = 0x80
UPDATE_ROTATE_DIR       = 0x40

; -------------------------------------------------
; Constants for usitwi.asm
ATTINY2313              = 1
TW_ADDRESS              = 0x6A


; ================================================================================================
;   D A T A  S E G M E N T
; ================================================================================================
.section .data


; ================================================================================================
;  E E P R O M  S E G M E N T
; ================================================================================================
.section .eeprom
EE_TABLE_DATA:  .byte    0, 7, 15, 31, 63, 255, 200, 150, 120, 90, 70, 50, 30, 20, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


; ================================================================================================
;  C O D E  S E G M E N T
; ================================================================================================
.section .text


; ================================================================================================
; Timer0, compare A handler
; Calls the PWM Channel Driver for processing the next LED
; ================================================================================================
.global TIMER0_COMPA_vect
TIMER0_COMPA_vect:
    push    work
    in      work, _SFR_IO_ADDR(SREG)
    push    work

    rcall   STP16_Refresh

    pop     work
    out     _SFR_IO_ADDR(SREG), work
    pop     work
    reti


; ================================================================================================
; Timer1, compare A handler
; Increments the offset value
; ================================================================================================
.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
    push    work
    in      work, _SFR_IO_ADDR(SREG)
    push    work

    rcall   STP16_UpdateOffset

    pop     work
    out     _SFR_IO_ADDR(SREG), work
    pop     work
    reti


; ================================================================================================
; Main
; ================================================================================================
.global main
main:
    ldi     r16, lo8(RAMEND)            ; Init LSB stack
    out     _SFR_IO_ADDR(SPL), r16

    rcall   Hardware_Init               ; Initialize the hardware
    rcall   EEPROM_Init                 ; Init EEPROM module
    rcall   STP16_Init                  ; Init the STP16 LED driver
    rcall   USI_Init                    ; Init USI module

    sei

Loop:
    wdr
    rcall   USI_ReadNext                ; get TWI data  :)
    rcall   ProcessTWIData              ; process the TWI request data
    rjmp    Loop


; ================================================================================================
; Processes the TWI request
; ================================================================================================
ProcessTWIData:
    lds     work, USI_RESULT
    
    cpi     work, GET_NEW_ON_VALUES      ; get new on_values
    breq    _GET_NEW_ON_VALUES
    
    cpi     work, COMMIT_ON_VALUES       ; save current on_values
    breq    _COMMIT_ON_VALUES
    
    cpi     work, UPDATE_ROTATE_SPD      ; update rotate speed
    breq    _UPDATE_ROTATE_SPD

    cpi     work, UPDATE_ROTATE_DIR      ; update rotate direction
    breq    _UPDATE_ROTATE_DIR
    
    rjmp    _procDone

_GET_NEW_ON_VALUES:
    ; wait for data
    rcall   STP16_GetChannelCount       ; get channel count (returns in work)
    lds     temp, USI_RX_AVAIL
    cp      temp, work
    brlo    _GET_NEW_ON_VALUES
    clr     rEEADDr
_get_data:
    rcall   USI_ReadNext
    lds     work, USI_RESULT
    out     _SFR_IO_ADDR(EEAR), rEEADDr              ; Pass data address to EEAR
    out     _SFR_IO_ADDR(EEDR), work                   ; Pass data to EEDR
    sbi     _SFR_IO_ADDR(EECR), EEMPE                 ; Write logical one to EEMPE
    sbi     _SFR_IO_ADDR(EECR), EEPE                  ; Start eeprom write by setting EEPE
    inc     rEEADDr
    dec     temp
    brne    _get_data
    rjmp    _procDone

_COMMIT_ON_VALUES:
    ; Update to not use buffer, if possible :)
    ldi     work, 0
    sts     EE_ADDR, work
    ;rcall  STP16_GetChannelCount       ; get channel count (returns in work)
    ;rcall  STP16_GetDataBuffer         ; load X with buffer address
    ;rcall  EEPROM_WriteBlock
    rjmp    _procDone

_UPDATE_ROTATE_SPD:
    ; wait for data
    lds     temp, USI_RX_AVAIL
    cpi     temp, 2
    brlo    _UPDATE_ROTATE_SPD

    rcall   USI_ReadNext
    lds     work, USI_RESULT                 ; high order delay byte
    rcall   USI_ReadNext
    lds     temp, USI_RESULT                ; low order delay byte
    out     _SFR_IO_ADDR(OCR1AH), work
    out     _SFR_IO_ADDR(OCR1AL), temp
    rjmp    _procDone

_UPDATE_ROTATE_DIR:
    lds     temp, USI_RX_AVAIL
    tst     temp
    breq    _UPDATE_ROTATE_DIR

    rcall   USI_ReadNext
    lds     work, USI_RESULT
    rcall   STP16_SetDirection
    rjmp    _procDone

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
    ldi     work, (1<<WGM01)                     ; CTC mode
    out     _SFR_IO_ADDR(TCCR0A), work
    ldi     work, (1<<CS01) | (1<<CS00)          ; CLK/64
    out     _SFR_IO_ADDR(TCCR0B), work
    ldi     work, TIMER0_DELAY                   ; 
    out     _SFR_IO_ADDR(OCR0A), work
    in      work, _SFR_IO_ADDR(TIMSK)                          ; enable the timer
    ori     work, (1<<OCIE0A)
    out     _SFR_IO_ADDR(TIMSK), work

    ; setup timer1
    ; timer1 is used to increment the offset index.
    ; CTC, clk/1024, OCRIA = TIMER1_DELAY (1200)
    ; 20,000,000 / 1024 = 19,531.25 / 1200 ~= 16Hz
    ; So, 16 times a second, we change the offset.
    ldi     work, (1<<WGM12)                     ; CTC
    ori     work, (1<<CS12) | (1<<CS10)          ; CLK/1024
    out     _SFR_IO_ADDR(TCCR1B), work
    ldi     r19, hi8(TIMER1_DELAY)             ; high order delay byte
    ldi     r20, lo8(TIMER1_DELAY)              ; low order delay byte
    out     _SFR_IO_ADDR(OCR1AH), r19
    out     _SFR_IO_ADDR(OCR1AL), r20
    in      work, _SFR_IO_ADDR(TIMSK)                          ; enable the timer
    ori     work, (1<<OCIE1A)
    out     _SFR_IO_ADDR(TIMSK), work

    ; setup PB0 as debug output
    in      work, _SFR_IO_ADDR(DDRB)
    ori     work, (1<<PINB0)
    out     _SFR_IO_ADDR(DDRB), work

    ; check for WDRF
    in      work, _SFR_IO_ADDR(MCUSR)
    sbrs    work, WDRF
    rjmp    _init_cont
    sbi     _SFR_IO_ADDR(PINB), PINB0
    rjmp    PC

    ; setup WatchDog!
_init_cont:
    ldi     work, (1<<WDCE)|(1<<WDE)|(1<<WDP2)|(WDP0)
    out     _SFR_IO_ADDR(WDTCSR), work

    clr     work
    clr     r19
    clr     r20

    ret

