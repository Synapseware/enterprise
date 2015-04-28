#ifndef EFFECTS_H
#define EFFECTS_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "board.h"
#include <events/events.h>
#include <uart/uart.h>

#include <drivers/at24c/at24c.h>
#include <twi/i2c.h>

#include "enterprise.h"


// playback rate, in Hz
static const uint16_t	SAMPLE_RATE			= 8000;
static const uint8_t	SAMPLE_LOADING		= 0xEE;
static const uint8_t	SAMPLE_PLAYING		= 0xFF;
static const uint8_t	SAMPLE_NONE			= 0x00;
static const uint8_t	SFX_OFF				= 0x00;
static const uint8_t	SFX_ON				= 0xFF;

static const uint16_t   effectsStart		= 0x00;	 // start address of effects records in EEPROM

static const uint8_t	RT_NULL				= 0xFF;	 // null record
static const uint8_t	RT_SOUND			= 0x01;	 // sound effects
static const uint8_t	RT_LIGHT			= 0x02;	 // lighting effects
static const uint8_t	RT_MOTION			= 0x04;	 // motion effects
static const uint8_t	RT_FIXED			= 0x08;	 // fixed schedule (not set implies a random scheduling)


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
class SoundEffects
{
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
	// Header structure...
	//	0	0x02	2		Signature
	//	1
	//	2	0x04	2		Start Page
	//	3
	//	4	0x06	2		Total Pages
	//	5
	//	6	0x08	1		Bytes Used In Last Page
	//	7			1		Reserved
	//	8	0x0A	2		Sample Rate
	//	9
	//	A	start of next entry...

	// SOUND_EFFECT reflects an individual entry in the header record
	typedef struct
	{
		uint16_t		startPage;				// + 2		2
		uint32_t		length;					// + 4		4
	} volatile SOUND_EFFECT;					// 6 bytes

	// SOUND_HEADER reflects the actual header record
	typedef struct
	{
		uint16_t          samples;              // + 0	// # of samples (0-255)
		SOUND_EFFECT      effects[42];          // + 2	// array of sound effects
	} volatile SOUND_HEADER;                    // 2 + 42 * 6 bytes = 254 bytes

	/*
	Event entry will contain:
		- the sample to play (by index)
		- the delay to the next sample
		- the next sample (by index)
	*/

	/*
	Dynamic events require a few things...
	- a playback function
		are we playing back a random event?
		a fixed event?
		a sound event?
		a lighting event?
		a motion event?
	- an event schedule (done, events.c)
	- storage (done, EEPROM)
	- generic playback event

	Event header "records" will require:
		- record type (sound, light, motion)
		- callback interval
		- number of samples in the effects record

	Event entry will contain:
		- the sample to play (by index)
		- the delay to the next sample
		- the next sample (by index)
	*/

	#ifdef __AVR_ATmega328P__
		#define		EEPROM_SIZE		1024
	#endif

	/*
	typedef struct
	{
		uint8_t		RecordType;
		uint8_t		Count;
		uint16_t	Delay;
	} EVENT_HEADER;

	typedef struct 
	{
		uint8_t		ItemIndex;
	} EVENT_ENTRY;
	*/

	SoundEffects(Events* events);
	int init(void);
	void on(void);
	void off(void);
	uint8_t playing(void);
	void startSample(uint8_t index);
	void playAmbient(void);
	void playBackground(void);
	void playSequence(void);

	void startSampleComplete(uint8_t result);
	void sampleCallback(void);
	void readComplete(uint8_t sfxdata);

private:
	int fillHeader(void);

	Events *       _events;
	SOUND_HEADER   _header;
	uint8_t        _sample;
	uint8_t	       _ambient;
	uint32_t       _length;
	uint8_t        _playState;
	uint16_t       _ambientPos;
	uint8_t        _ambientDelay;
	uint8_t        _onoff;
};



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// ambient sounds
static const uint8_t SFX_AMBIENT_BEEP_BEEP		= 0;
static const uint8_t SFX_AMBIENT_BEEP_UP		= 1;
static const uint8_t SFX_AMBIENT_KEY_BEEP		= 2;
static const uint8_t SFX_AMBIENT_WHIP_WHIP		= 3;

// background sounds
static const uint8_t SFX_BK_ASTROGATOR			= 4;
static const uint8_t SFX_BK_MAIN_SCREEN			= 5;

// sound effects
static const uint8_t SFX_EFX_COMM				= 6;
static const uint8_t SFX_EFX_DOORS				= 7;
static const uint8_t SFX_EFX_ENERGIZE			= 8;
static const uint8_t SFX_EFX_HAILING			= 9;
static const uint8_t SFX_EFX_HEART				= 10;
static const uint8_t SFX_EFX_OPENING			= 11;
static const uint8_t SFX_EFX_RED_ALERT			= 12;
static const uint8_t SFX_EFX_REPLICATOR			= 13;
static const uint8_t SFX_EFX_TRIBBLE			= 14;

// voices
static const uint8_t SFX_VOICES_DOCTOR			= 15;
static const uint8_t SFX_VOICES_IMPOSSIBLE		= 16;
static const uint8_t SFX_VOICES_BRIDGE_KIRK		= 17;
static const uint8_t SFX_VOICES_WHAT_DO			= 18;
static const uint8_t SFX_VOICES_GOING_DOWN		= 19;
static const uint8_t SFX_VOICES_BEAM_ME_UP		= 20;
static const uint8_t SFX_VOICES_KIRK_HERE		= 21;
static const uint8_t SFX_VOICES_AYE_SIR			= 22;
static const uint8_t SFX_VOICES_SCOTTY_KIRK		= 23;
static const uint8_t SFX_VOICES_FASCINATING		= 24;


const static uint8_t AMBIENT_SOUND[] PROGMEM = {
    0x7C, 0x78, 0x75, 0x71, 0x6E, 0x6B, 0x67, 0x65, 0x62, 0x60, 0x5E, 0x5C, 0x5B, 0x5A, 0x5A, 0x5B, 0x5B, 0x5C, 0x5E, 0x5F,
    0x61, 0x63, 0x64, 0x66, 0x67, 0x69, 0x6A, 0x6B, 0x6D, 0x6E, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7D, 0x7F, 0x82, 0x84,
    0x86, 0x88, 0x89, 0x8A, 0x8B, 0x8B, 0x8A, 0x89, 0x87, 0x85, 0x82, 0x7F, 0x7B, 0x78, 0x74, 0x70, 0x6D, 0x6A, 0x68, 0x66,
    0x65, 0x64, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6A, 0x6B, 0x6C, 0x6C, 0x6D, 0x6D, 0x6E, 0x6E, 0x70, 0x71, 0x73,
    0x75, 0x78, 0x7A, 0x7D, 0x81, 0x84, 0x87, 0x89, 0x8C, 0x8E, 0x90, 0x91, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x91, 0x91,
    0x90, 0x8F, 0x8E, 0x8C, 0x8A, 0x88, 0x85, 0x82, 0x7E, 0x7B, 0x77, 0x73, 0x6F, 0x6C, 0x69, 0x67, 0x65, 0x64, 0x63, 0x63,
    0x63, 0x64, 0x66, 0x69, 0x6C, 0x70, 0x75, 0x7A, 0x80, 0x86, 0x8C, 0x93, 0x99, 0x9F, 0xA5, 0xAB, 0xB0, 0xB5, 0xBA, 0xBE,
    0xC1, 0xC3, 0xC5, 0xC7, 0xC7, 0xC7, 0xC6, 0xC5, 0xC3, 0xC1, 0xBE, 0xBB, 0xB7, 0xB3, 0xAE, 0xAA, 0xA5, 0xA0, 0x9A, 0x95,
    0x90, 0x8B, 0x85, 0x80, 0x7B, 0x77, 0x72, 0x6F, 0x6B, 0x68, 0x66, 0x64, 0x62, 0x61, 0x61, 0x61, 0x62, 0x63, 0x64, 0x66,
    0x69, 0x6B, 0x6E, 0x71, 0x73, 0x76, 0x78, 0x7A, 0x7C, 0x7E, 0x7F, 0x81, 0x82, 0x84, 0x85, 0x86, 0x87, 0x88, 0x8A, 0x8B,
    0x8C, 0x8D, 0x8E, 0x8E, 0x8E, 0x8E, 0x8D, 0x8C, 0x8A, 0x87, 0x85, 0x82, 0x7E, 0x7B, 0x77, 0x74, 0x70, 0x6D, 0x6A, 0x68,
    0x66, 0x64, 0x63, 0x62, 0x62, 0x62, 0x63, 0x64, 0x65, 0x67, 0x6A, 0x6C, 0x70, 0x73, 0x78, 0x7D, 0x82, 0x89, 0x8F, 0x96,
    0x9D, 0xA4, 0xAB, 0xB2, 0xB8, 0xBD, 0xC2, 0xC5, 0xC8, 0xCA, 0xCB, 0xCB, 0xCA, 0xC8, 0xC6, 0xC3, 0xBF, 0xBB, 0xB6, 0xB1,
    0xAC, 0xA6, 0xA0, 0x9B, 0x95, 0x8F, 0x8A, 0x85, 0x81, 0x7D, 0x7A, 0x77, 0x74, 0x72, 0x71, 0x6F, 0x6E, 0x6D, 0x6C, 0x6B,
    0x69, 0x67, 0x65, 0x63, 0x60, 0x5E, 0x5B, 0x58, 0x55, 0x53, 0x51, 0x4F, 0x4E, 0x4D, 0x4D, 0x4D, 0x4C, 0x4C, 0x4C, 0x4C,
    0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4D, 0x4E, 0x4F, 0x51, 0x52, 0x54, 0x56, 0x57, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5D,
    0x5C, 0x5C, 0x5B, 0x5A, 0x59, 0x58, 0x58, 0x57, 0x57, 0x56, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5D, 0x5F, 0x62, 0x66, 0x69,
    0x6D, 0x71, 0x75, 0x79, 0x7D, 0x80, 0x84, 0x87, 0x89, 0x8C, 0x8D, 0x8F, 0x8F, 0x8F, 0x8F, 0x8E, 0x8C, 0x8A, 0x87, 0x84,
    0x81, 0x7D, 0x7A, 0x76, 0x73, 0x71, 0x6E, 0x6C, 0x6A, 0x69, 0x68, 0x67, 0x67, 0x67, 0x68, 0x69, 0x6B, 0x6D, 0x6F, 0x72,
    0x75, 0x79, 0x7D, 0x81, 0x85, 0x89, 0x8D, 0x92, 0x96, 0x9A, 0x9D, 0xA1, 0xA4, 0xA7, 0xAA, 0xAD, 0xB0, 0xB2, 0xB5, 0xB7,
    0xBA, 0xBC, 0xBE, 0xC0, 0xC2, 0xC3, 0xC4, 0xC5, 0xC5, 0xC4, 0xC3, 0xC1, 0xBF, 0xBB, 0xB7, 0xB2, 0xAD, 0xA7, 0xA1, 0x9B,
    0x95, 0x90, 0x8B, 0x87, 0x84, 0x82, 0x80, 0x80, 0x81, 0x83, 0x85, 0x89, 0x8C, 0x90, 0x94, 0x97, 0x9B, 0x9E, 0xA1, 0xA3,
    0xA5, 0xA6, 0xA7, 0xA7, 0xA7, 0xA6, 0xA5, 0xA3, 0xA1, 0x9E, 0x9C, 0x98, 0x95, 0x92, 0x8E, 0x8B, 0x88, 0x85, 0x83, 0x82,
    0x81, 0x81, 0x81, 0x82, 0x84, 0x86, 0x89, 0x8C, 0x8F, 0x92, 0x95, 0x97, 0x99, 0x9B, 0x9D, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
    0x9C, 0x9B, 0x99, 0x96, 0x94, 0x90, 0x8D, 0x89, 0x85, 0x80, 0x7C, 0x77, 0x72, 0x6D, 0x68, 0x63, 0x5F, 0x5A, 0x55, 0x50,
    0x4B, 0x46, 0x42, 0x3E, 0x3A, 0x37, 0x34, 0x31, 0x2F, 0x2D, 0x2C, 0x2B, 0x2A, 0x2A, 0x2B, 0x2C, 0x2D, 0x2F, 0x31, 0x33,
    0x36, 0x38, 0x3B, 0x3E, 0x41, 0x44, 0x48, 0x4B, 0x4F, 0x52, 0x56, 0x59, 0x5C, 0x5E, 0x61, 0x62, 0x64, 0x65, 0x65, 0x66,
    0x66, 0x66, 0x66, 0x66, 0x65, 0x65, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x5F, 0x5E, 0x5D,
    0x5C, 0x5C, 0x5C, 0x5D, 0x5E, 0x60, 0x63, 0x67, 0x6B, 0x70, 0x75, 0x7B, 0x81, 0x87, 0x8D, 0x92, 0x97, 0x9C, 0xA1, 0xA4,
    0xA7, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB3, 0xB5, 0xB7, 0xB9, 0xBA, 0xBC, 0xBE, 0xC0, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC6,
    0xC6, 0xC5, 0xC4, 0xC3, 0xC1, 0xBF, 0xBC, 0xB9, 0xB5, 0xB1, 0xAD, 0xA9, 0xA4, 0xA0, 0x9C, 0x98, 0x95, 0x93, 0x91, 0x91,
    0x90, 0x91, 0x91, 0x92, 0x94, 0x95, 0x97, 0x99, 0x9B, 0x9D, 0x9F, 0xA1, 0xA3, 0xA4, 0xA6, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC,
    0xAE, 0xAF, 0xB1, 0xB3, 0xB5, 0xB7, 0xB9, 0xBC, 0xBE, 0xC0, 0xC1, 0xC2, 0xC3, 0xC3, 0xC2, 0xC1, 0xC0, 0xBE, 0xBB, 0xB9,
    0xB5, 0xB2, 0xAF, 0xAB, 0xA8, 0xA4, 0xA1, 0x9D, 0x9A, 0x97, 0x94, 0x91, 0x8D, 0x8A, 0x87, 0x84, 0x80, 0x7C, 0x78, 0x73,
    0x6E, 0x69, 0x64, 0x5E, 0x58, 0x53, 0x4D, 0x48, 0x42, 0x3E, 0x3A, 0x37, 0x34, 0x32, 0x31, 0x31, 0x32, 0x33, 0x34, 0x36,
    0x38, 0x3B, 0x3D, 0x40, 0x42, 0x45, 0x47, 0x49, 0x4A, 0x4A, 0x4A, 0x4A, 0x49, 0x48, 0x46, 0x45, 0x43, 0x42, 0x40, 0x3F,
    0x3F, 0x3F, 0x3F, 0x3F, 0x40, 0x41, 0x43, 0x44, 0x46, 0x48, 0x4B, 0x4D, 0x50, 0x53, 0x55, 0x58, 0x5A, 0x5C, 0x5E, 0x60,
    0x61, 0x63, 0x64, 0x66, 0x68, 0x69, 0x6B, 0x6E, 0x70, 0x73, 0x76, 0x79, 0x7C, 0x7F, 0x82, 0x85, 0x87, 0x8A, 0x8D, 0x90,
    0x92, 0x95, 0x97, 0x99, 0x9A, 0x9B, 0x9C, 0x9C, 0x9C, 0x9B, 0x99, 0x97, 0x95, 0x92, 0x8E, 0x8B, 0x88, 0x84, 0x81, 0x7F,
    0x7C, 0x7A, 0x79, 0x78, 0x77, 0x77, 0x77, 0x77, 0x78, 0x7A, 0x7B, 0x7D, 0x80, 0x82, 0x85, 0x87, 0x8A, 0x8C, 0x8F, 0x91,
    0x93, 0x95, 0x97, 0x99, 0x9B, 0x9D, 0x9E, 0x9F, 0x9F, 0x9E, 0x9D, 0x9C, 0x99, 0x96, 0x93, 0x8F, 0x8B, 0x87, 0x83, 0x80,
    0x7D, 0x7A, 0x78, 0x76, 0x75, 0x75, 0x75, 0x76, 0x77, 0x78, 0x7A, 0x7C, 0x7E, 0x7F, 0x81, 0x83, 0x85, 0x87, 0x89, 0x8B,
    0x8E, 0x91, 0x94, 0x98, 0x9C, 0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB3, 0xB7, 0xBA, 0xBD, 0xC0, 0xC2, 0xC3, 0xC5, 0xC6, 0xC6,
    0xC6, 0xC5, 0xC3, 0xC1, 0xBE, 0xBB, 0xB7, 0xB3, 0xAE, 0xA9, 0xA4, 0xA0, 0x9C, 0x98, 0x94, 0x91, 0x8F, 0x8D, 0x8B, 0x89,
    0x87, 0x86, 0x84, 0x82, 0x80, 0x7E, 0x7C, 0x7A, 0x78, 0x76, 0x74, 0x72, 0x71, 0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x69,
    0x68, 0x67, 0x66, 0x64, 0x63, 0x61, 0x5F, 0x5D, 0x5B, 0x59, 0x57, 0x55, 0x53, 0x52, 0x50, 0x4F, 0x4E, 0x4D, 0x4D, 0x4D,
    0x4D, 0x4D, 0x4D, 0x4D, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4E, 0x4F, 0x52, 0x54, 0x58,
    0x5C, 0x61, 0x67, 0x6C, 0x72, 0x78, 0x7D, 0x82, 0x87, 0x8B, 0x8E, 0x90, 0x91, 0x92, 0x92, 0x91, 0x90, 0x8F, 0x8E, 0x8C,
    0x8B, 0x8B, 0x8B, 0x8B, 0x8C, 0x8D, 0x8E, 0x90, 0x91, 0x93, 0x94, 0x96, 0x96, 0x97, 0x97, 0x96, 0x94, 0x93, 0x90, 0x8E,
    0x8B, 0x88, 0x85, 0x83, 0x80, 0x7E, 0x7C, 0x7A, 0x79, 0x79, 0x79, 0x79, 0x79, 0x7A, 0x7B, 0x7C, 0x7C, 0x7D, 0x7E, 0x7E,
    0x7E, 0x7E, 0x7D, 0x7D, 0x7C, 0x7B, 0x7B, 0x7A, 0x7A, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79, 0x78,
    0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
    0x78, 0x79, 0x7A, 0x7B, 0x7D, 0x7E, 0x80, 0x83, 0x85, 0x87, 0x89, 0x8B, 0x8D, 0x8E, 0x90, 0x91, 0x92, 0x93, 0x93, 0x93,
    0x92, 0x92, 0x91, 0x90, 0x8F, 0x8D, 0x8C, 0x8A, 0x89, 0x87, 0x85, 0x83, 0x81, 0x7F, 0x7D, 0x7A, 0x77, 0x74, 0x71, 0x6E,
    0x6C, 0x69, 0x67, 0x65, 0x63, 0x62, 0x62, 0x63, 0x64, 0x66, 0x68, 0x6B, 0x6F, 0x73, 0x77, 0x7C, 0x80, 0x85, 0x89, 0x8D,
    0x91, 0x95, 0x99, 0x9C, 0x9E, 0xA1, 0xA3, 0xA5, 0xA7, 0xA8, 0xA9, 0xAA, 0xAA, 0xAB, 0xAB, 0xAB, 0xAA, 0xAA, 0xA9, 0xA8,
    0xA7, 0xA5, 0xA4, 0xA2, 0xA0, 0x9F, 0x9D, 0x9C, 0x9B, 0x9B, 0x9A, 0x9A, 0x9A, 0x9A, 0x9B, 0x9B, 0x9C, 0x9D, 0x9D, 0x9E,
    0x9E, 0x9E, 0x9D, 0x9C, 0x9B, 0x98, 0x96, 0x93, 0x90, 0x8D, 0x8B, 0x88, 0x86, 0x84, 0x82, 0x81, 0x81, 0x82, 0x83, 0x84,
    0x86, 0x88, 0x8B, 0x8D, 0x90, 0x92, 0x94, 0x95, 0x95, 0x95, 0x95, 0x94, 0x92, 0x90, 0x8E, 0x8C, 0x8A, 0x88, 0x87, 0x85,
    0x83, 0x81, 0x80, 0x7E, 0x7C, 0x7A, 0x78, 0x76, 0x73, 0x71, 0x6F, 0x6D, 0x6B, 0x69, 0x67, 0x65, 0x64, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x64, 0x65, 0x65, 0x66, 0x67, 0x67, 0x67, 0x67, 0x66, 0x66, 0x65, 0x64, 0x63, 0x61, 0x60, 0x5E, 0x5D, 0x5C,
    0x5A, 0x59, 0x59, 0x58, 0x58, 0x59, 0x5A, 0x5C, 0x5E, 0x60, 0x63, 0x67, 0x6A, 0x6E, 0x72, 0x76, 0x7A, 0x7E, 0x82, 0x86,
    0x89, 0x8C, 0x8F, 0x91, 0x93, 0x94, 0x95, 0x96, 0x96, 0x96, 0x95, 0x94, 0x92, 0x90, 0x8E, 0x8C, 0x89, 0x87, 0x85, 0x83,
    0x81, 0x7F, 0x7D, 0x7B, 0x7A, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0x73, 0x73, 0x73, 0x73, 0x74, 0x75, 0x76, 0x78, 0x7A,
    0x7C, 0x7E, 0x7F, 0x81, 0x82, 0x82, 0x82, 0x81, 0x80, 0x7E, 0x7C, 0x79, 0x77, 0x75, 0x74, 0x73, 0x73, 0x73, 0x74, 0x76,
    0x78, 0x7B, 0x7E, 0x81, 0x85, 0x88, 0x8B, 0x8D, 0x8F, 0x91, 0x91, 0x91, 0x90, 0x8F, 0x8D, 0x8B, 0x88, 0x85, 0x82, 0x7E,
    0x7B, 0x77, 0x74, 0x71, 0x6E, 0x6B, 0x68, 0x65, 0x63, 0x60, 0x5E, 0x5C, 0x5A, 0x59, 0x58, 0x57, 0x57, 0x56, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x5C, 0x5D, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6B, 0x6D, 0x6F, 0x71,
    0x73, 0x75, 0x77, 0x79, 0x7B, 0x7D, 0x7E, 0x80, 0x82, 0x84, 0x87, 0x8A, 0x8C, 0x90, 0x93, 0x96, 0x99, 0x9B, 0x9D, 0x9E,
    0x9F, 0x9F, 0x9E, 0x9D, 0x9C, 0x9A, 0x98, 0x96, 0x94, 0x93, 0x91, 0x90, 0x8F, 0x8E, 0x8E, 0x8D, 0x8C, 0x8B, 0x8A, 0x89,
    0x88, 0x88, 0x87, 0x87, 0x87, 0x87, 0x89, 0x8A, 0x8D, 0x8F, 0x92, 0x96, 0x99, 0x9C, 0xA0, 0xA3, 0xA5, 0xA7, 0xA8, 0xA8,
    0xA8, 0xA6, 0xA4, 0xA0, 0x9C, 0x98, 0x93, 0x8E, 0x8A, 0x85, 0x81, 0x7D, 0x79, 0x76, 0x74, 0x72, 0x70, 0x6F, 0x6F, 0x6F,
    0x6F, 0x70, 0x71, 0x73, 0x75, 0x78, 0x7B, 0x7E, 0x82, 0x87, 0x8B, 0x91, 0x96, 0x9B, 0xA0, 0xA5, 0xA9, 0xAC, 0xAF, 0xB1,
    0xB3, 0xB3, 0xB3, 0xB1, 0xB0, 0xAD, 0xAA, 0xA7, 0xA4, 0xA0, 0x9D, 0x99, 0x96, 0x93, 0x91, 0x8F, 0x8D, 0x8C, 0x8B, 0x8B,
    0x8A, 0x8A, 0x89, 0x89, 0x88, 0x87, 0x85, 0x83, 0x81, 0x7F, 0x7C, 0x78, 0x75, 0x72, 0x6F, 0x6C, 0x69, 0x68, 0x66, 0x66,
    0x66, 0x67, 0x69, 0x6B, 0x6E, 0x71, 0x74, 0x78, 0x7B, 0x7E, 0x81, 0x84, 0x86, 0x88, 0x89, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
    0x89, 0x88, 0x87, 0x87, 0x86, 0x85, 0x84, 0x84, 0x83, 0x82, 0x82, 0x81, 0x80, 0x7F, 0x7E, 0x7D, 0x7C, 0x7C, 0x7B, 0x7A,
    0x7A, 0x79, 0x79, 0x79, 0x79, 0x79, 0x78, 0x77, 0x76, 0x74, 0x72, 0x70, 0x6E, 0x6B, 0x68, 0x66, 0x63, 0x61, 0x5E, 0x5C,
    0x5A, 0x57, 0x55, 0x53, 0x50, 0x4D, 0x4B, 0x48, 0x45, 0x43, 0x41, 0x3F, 0x3E, 0x3D, 0x3E, 0x3F, 0x40, 0x43, 0x46, 0x49,
    0x4E, 0x52, 0x57, 0x5D, 0x62, 0x68, 0x6E, 0x75, 0x7B, 0x81, 0x87, 0x8C, 0x92, 0x96, 0x9A, 0x9D, 0xA0, 0xA2, 0xA3, 0xA3,
    0xA3, 0xA2, 0xA0, 0x9D, 0x9A, 0x97, 0x93, 0x8F, 0x8A, 0x86, 0x81, 0x7D, 0x79, 0x76, 0x73, 0x71, 0x70, 0x6F, 0x6F, 0x70,
    0x71, 0x73, 0x76, 0x79, 0x7D, 0x81, 0x86, 0x8B, 0x90, 0x94, 0x98, 0x9C, 0x9F, 0xA1, 0xA2, 0xA2, 0xA1, 0xA0, 0x9D, 0x9A,
    0x96, 0x92, 0x8D, 0x88, 0x82, 0x7D, 0x79, 0x74, 0x70, 0x6D, 0x6A, 0x68, 0x67, 0x66, 0x65, 0x65, 0x66, 0x67, 0x68, 0x6A,
    0x6C, 0x6F, 0x71, 0x74, 0x76, 0x79, 0x7C, 0x7E, 0x80, 0x82, 0x83, 0x84, 0x85, 0x86, 0x86, 0x87, 0x87, 0x88, 0x8A, 0x8B,
    0x8D, 0x90, 0x93, 0x97, 0x9B, 0xA0, 0xA4, 0xA9, 0xAD, 0xB1, 0xB5, 0xB8, 0xBA, 0xBB, 0xBB, 0xBB, 0xBA, 0xB8, 0xB6, 0xB3,
    0xB0, 0xAD, 0xA9, 0xA5, 0xA0, 0x9B, 0x96, 0x91, 0x8C, 0x87, 0x82, 0x7E, 0x7A, 0x77, 0x74, 0x72, 0x70, 0x6F, 0x6F, 0x70,
    0x70, 0x72, 0x74, 0x76, 0x78, 0x7B, 0x7D, 0x80, 0x82, 0x84, 0x86, 0x87, 0x88, 0x89, 0x89, 0x89, 0x88, 0x88, 0x86, 0x85,
    0x83, 0x82, 0x7F, 0x7D, 0x7A, 0x78, 0x75, 0x72, 0x6E, 0x6B, 0x68, 0x64, 0x61, 0x5E, 0x5A, 0x57, 0x53, 0x50, 0x4C, 0x48,
    0x45, 0x41, 0x3E, 0x3C, 0x3A, 0x38, 0x37, 0x37, 0x38, 0x3A, 0x3C, 0x3F, 0x43, 0x47, 0x4C, 0x51, 0x56, 0x5A, 0x5F, 0x64,
    0x68, 0x6C, 0x6F, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7B, 0x7C, 0x7E, 0x7F, 0x81, 0x83, 0x86, 0x88, 0x8B, 0x8E, 0x91, 0x94,
    0x97, 0x9A, 0x9D, 0x9F, 0xA1, 0xA3, 0xA5, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAA, 0xA9, 0xA8, 0xA6,
    0xA5, 0xA3, 0xA0, 0x9E, 0x9B, 0x98, 0x95, 0x92, 0x8E, 0x8B, 0x87, 0x84, 0x80, 0x7D, 0x79, 0x76, 0x73, 0x71, 0x6F, 0x6D,
    0x6B, 0x6B, 0x6A, 0x6A, 0x6A, 0x6B, 0x6C, 0x6E, 0x70, 0x72, 0x75, 0x78, 0x7C, 0x80, 0x84, 0x89, 0x8E, 0x92, 0x96, 0x9A,
    0x9E, 0xA1, 0xA3, 0xA4, 0xA5, 0xA5, 0xA5, 0xA4, 0xA3, 0xA1, 0xA0, 0x9E, 0x9D, 0x9C, 0x9A, 0x99, 0x98, 0x98, 0x98, 0x98,
    0x99, 0x9A, 0x9B, 0x9C, 0x9E, 0x9F, 0xA1, 0xA2, 0xA3, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA3, 0xA2, 0xA0, 0x9F, 0x9D, 0x9A,
    0x97, 0x95, 0x91, 0x8E, 0x8B, 0x87, 0x84, 0x80, 0x7C, 0x78, 0x74, 0x71, 0x6D, 0x69, 0x66, 0x62, 0x60, 0x5D, 0x5B, 0x59,
    0x58, 0x57, 0x57, 0x57, 0x57, 0x58, 0x59, 0x5B, 0x5C, 0x5E, 0x5F, 0x61, 0x62, 0x62, 0x62, 0x62, 0x61, 0x60, 0x5E, 0x5C,
    0x5A, 0x58, 0x55, 0x53, 0x51, 0x50, 0x4F, 0x4E, 0x4E, 0x4F, 0x51, 0x53, 0x57, 0x5B, 0x5F, 0x64, 0x69, 0x6F, 0x75, 0x7B,
    0x81, 0x87, 0x8D, 0x92, 0x97, 0x9C, 0xA0, 0xA3, 0xA5, 0xA6, 0xA7, 0xA6, 0xA4, 0xA1, 0x9E, 0x9A, 0x96, 0x91, 0x8C, 0x86,
    0x81, 0x7B, 0x76, 0x71, 0x6D, 0x69, 0x66, 0x63, 0x62, 0x61, 0x61, 0x61, 0x63, 0x64, 0x66, 0x69, 0x6B, 0x6E, 0x70, 0x72,
    0x74, 0x76, 0x78, 0x79, 0x7A, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x82, 0x84, 0x87, 0x8A, 0x8D, 0x91, 0x95, 0x99,
    0x9D, 0xA2, 0xA7, 0xAB, 0xB0, 0xB4, 0xB7, 0xBA, 0xBC, 0xBD, 0xBE, 0xBD, 0xBC, 0xBA, 0xB6, 0xB2, 0xAE, 0xA8, 0xA2, 0x9B,
    0x93, 0x8B, 0x83, 0x7B, 0x74, 0x6D, 0x66, 0x61, 0x5D, 0x5B, 0x59, 0x59, 0x5A, 0x5C, 0x60, 0x64, 0x68, 0x6D, 0x72, 0x77,
    0x7B, 0x7F, 0x82, 0x85, 0x87, 0x88, 0x88, 0x87, 0x85, 0x83, 0x81, 0x7E, 0x7B, 0x78, 0x75, 0x73, 0x71, 0x70, 0x70, 0x70,
    0x71, 0x73, 0x76, 0x7A, 0x7E, 0x83, 0x87, 0x8C, 0x90, 0x94, 0x98, 0x9B, 0x9E, 0xA1, 0xA3, 0xA4, 0xA6, 0xA6, 0xA7, 0xA7,
    0xA7, 0xA6, 0xA5, 0xA4, 0xA2, 0xA0, 0x9E, 0x9C, 0x99, 0x95, 0x92, 0x8E, 0x8B, 0x87, 0x84, 0x80, 0x7D, 0x7B, 0x79, 0x78,
    0x77, 0x77, 0x77, 0x78, 0x79, 0x7A, 0x7C, 0x7D, 0x7E, 0x80, 0x81, 0x83, 0x84, 0x86, 0x88, 0x89, 0x8B, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x92, 0x91, 0x90, 0x8F, 0x8D, 0x8B, 0x88, 0x85, 0x82, 0x7E, 0x7B, 0x77, 0x74, 0x70, 0x6D, 0x6A, 0x67,
    0x64, 0x61, 0x5F, 0x5C, 0x59, 0x57, 0x55, 0x53, 0x51, 0x50, 0x4F, 0x4E, 0x4E, 0x4F, 0x50, 0x51, 0x53, 0x56, 0x59, 0x5C,
    0x5F, 0x62, 0x65, 0x68, 0x6A, 0x6C, 0x6E, 0x6F, 0x70, 0x71, 0x71, 0x71, 0x71, 0x70, 0x70, 0x6F, 0x6E, 0x6E, 0x6D, 0x6C,
    0x6C, 0x6B, 0x6B, 0x6B, 0x6C, 0x6D, 0x6E, 0x70, 0x72, 0x74, 0x77, 0x7A, 0x7D, 0x80, 0x83, 0x86, 0x89, 0x8C, 0x8E, 0x8F,
    0x90, 0x91, 0x91, 0x90, 0x8F, 0x8D, 0x8B, 0x88, 0x86, 0x83, 0x80, 0x7D, 0x7B, 0x78, 0x76, 0x74, 0x72, 0x71, 0x71, 0x71,
    0x71, 0x72, 0x73, 0x74, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7E,
    0x7E, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7E, 0x7F, 0x81, 0x82, 0x84, 0x86, 0x88, 0x8B, 0x8D, 0x8F, 0x91, 0x94, 0x96, 0x97,
    0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9D, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9D, 0x9C, 0x9B, 0x9A, 0x99, 0x97, 0x96, 0x94,
    0x92, 0x90, 0x8F, 0x8D, 0x8C, 0x8B, 0x8A, 0x89, 0x88, 0x88, 0x87, 0x87, 0x88, 0x88, 0x88, 0x88, 0x87, 0x86, 0x85, 0x84,
    0x82, 0x80, 0x7D, 0x7B, 0x78, 0x75, 0x73, 0x70, 0x6D, 0x6B, 0x68, 0x66, 0x64, 0x62, 0x61, 0x5F, 0x5D, 0x5B, 0x59, 0x57,
    0x55, 0x52, 0x50, 0x4D, 0x4B, 0x49, 0x48, 0x47, 0x46, 0x46, 0x47, 0x48, 0x4A, 0x4C, 0x4F, 0x53, 0x56, 0x5B, 0x5F, 0x63,
    0x67, 0x6B, 0x6F, 0x73, 0x76, 0x78, 0x7B, 0x7E, 0x80, 0x83, 0x86, 0x89, 0x8D, 0x91, 0x95, 0x99, 0x9E, 0xA2, 0xA7, 0xAB,
    0xAF, 0xB2, 0xB5, 0xB7, 0xB9, 0xBA, 0xBA, 0xBA, 0xB9, 0xB8, 0xB6, 0xB4, 0xB1, 0xAD, 0xAA, 0xA6, 0xA1, 0x9D, 0x98, 0x94,
    0x90, 0x8D, 0x89, 0x87, 0x85, 0x83, 0x81, 0x80, 0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x78, 0x77, 0x77, 0x76, 0x76,
    0x76, 0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x77, 0x76, 0x75, 0x74, 0x72, 0x70, 0x6F, 0x6D, 0x6B, 0x6A, 0x69, 0x69,
    0x69, 0x69, 0x6A, 0x6C, 0x6E, 0x70, 0x73, 0x76, 0x79, 0x7C, 0x7F, 0x83, 0x86, 0x8A, 0x8D, 0x90, 0x93, 0x96, 0x98, 0x9A,
    0x9B, 0x9D, 0x9D, 0x9E, 0x9F, 0x9F, 0x9F, 0x9F, 0xA0, 0xA0, 0xA0, 0xA1, 0xA1, 0xA1, 0xA2, 0xA2, 0xA2, 0xA2, 0xA1, 0xA1,
    0xA0, 0xA0, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0xA0, 0xA0, 0xA1, 0xA1, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
    0xA2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA2, 0xA1, 0xA0, 0x9E, 0x9C, 0x9B, 0x98, 0x96, 0x94, 0x92, 0x91, 0x8F, 0x8D,
    0x8C, 0x8B, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8B, 0x8C, 0x8D, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x91, 0x91, 0x91, 0x91, 0x90,
    0x90, 0x8E, 0x8C, 0x8A, 0x88, 0x85, 0x82, 0x7F, 0x7C, 0x79, 0x76, 0x74, 0x72, 0x70, 0x6F, 0x6E, 0x6D, 0x6D, 0x6C, 0x6B,
    0x6B, 0x6A, 0x69, 0x67, 0x66, 0x64, 0x62, 0x60, 0x5E, 0x5C, 0x5A, 0x58, 0x57, 0x57, 0x57, 0x57, 0x58, 0x59, 0x5A, 0x5C,
    0x5E, 0x60, 0x61, 0x62, 0x64, 0x64, 0x65, 0x65, 0x64, 0x62, 0x61, 0x5F, 0x5C, 0x5A, 0x57, 0x55, 0x53, 0x51, 0x50, 0x4F,
    0x4E, 0x4E, 0x4E, 0x4E, 0x4E, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x57, 0x58, 0x58, 0x58, 0x58,
    0x58, 0x57, 0x57, 0x56, 0x55, 0x54, 0x52, 0x50, 0x4E, 0x4D, 0x4A, 0x48, 0x46, 0x44, 0x42, 0x41, 0x3F, 0x3E, 0x3D, 0x3D,
    0x3D, 0x3E, 0x3F, 0x41, 0x43, 0x46, 0x4A, 0x4E, 0x52, 0x57, 0x5B, 0x5F, 0x63, 0x66, 0x69, 0x6C, 0x6E, 0x70, 0x71, 0x71,
    0x71, 0x71, 0x71, 0x70, 0x6F, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x70, 0x71, 0x73, 0x75, 0x78, 0x7B, 0x7E, 0x82, 0x86, 0x8A,
    0x8F, 0x94, 0x99, 0x9F, 0xA4, 0xAA, 0xAF, 0xB4, 0xB8, 0xBD, 0xC1, 0xC5, 0xC8, 0xCC, 0xCF, 0xD3, 0xD6, 0xD9, 0xDC, 0xDE,
    0xE0, 0xE2, 0xE2, 0xE2, 0xE2, 0xE0, 0xDE, 0xDB, 0xD7, 0xD3, 0xCE, 0xC9, 0xC4, 0xBE, 0xB9, 0xB4, 0xAE, 0xA9, 0xA4, 0xA0,
    0x9C, 0x98, 0x94, 0x90, 0x8D, 0x8A, 0x87, 0x84, 0x82, 0x80, 0x7E, 0x7D, 0x7B, 0x7B, 0x7A, 0x7A, 0x7A, 0x7B, 0x7C, 0x7D,
    0x7F, 0x80, 0x82, 0x84, 0x87, 0x89, 0x8C, 0x8E, 0x90, 0x93, 0x94, 0x95, 0x96, 0x96, 0x96, 0x95, 0x94, 0x92, 0x90, 0x8D,
    0x8B, 0x88, 0x85, 0x83, 0x81, 0x80, 0x7F, 0x7E, 0x7E, 0x7E, 0x7F, 0x80, 0x81, 0x83, 0x85, 0x88, 0x8A, 0x8D, 0x8F, 0x91,
    0x93, 0x95, 0x97, 0x99, 0x9A, 0x9B, 0x9D, 0x9E, 0x9F, 0xA0, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA6, 0xA7, 0xA7, 0xA7, 0xA6,
    0xA6, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1, 0x9F, 0x9D, 0x9B, 0x99, 0x96, 0x93, 0x90, 0x8D, 0x8A, 0x87, 0x83, 0x81, 0x7D, 0x7D
};
static const uint16_t AMBIENT_LEN = sizeof(AMBIENT_SOUND)/sizeof(uint8_t);


#endif
