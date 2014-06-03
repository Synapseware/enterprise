#include "effects.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Effects variables
volatile	SOUND_HEADER	_sfx_header;
volatile	uint8_t			_sfx_sample			= 0;
volatile	uint8_t			_sfx_ambient		= 0;
volatile	uint32_t		_sfx_length			= 0;
volatile	uint8_t			_sfx_playState		= 0;
volatile	uint16_t		_sfx_amb_pos		= 0;

volatile	uint8_t			_sfxAmbientDelay	= 0xff;
volatile	uint8_t			_sfx_onoff			= 0;



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void _sfxReadComplete(uint8_t sfxdata)
{
	_sfx_sample = sfxdata;
	OCR2B = ((uint16_t)_sfx_sample + (uint16_t)_sfx_ambient) >> 1;

	if (!_sfx_length)
		ee_readEnd();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// This callback is used to dump the next value into the PWM stream
void sfx_sampleCallback(eventState_t state)
{
	// don't do anything if sound-effects are not enabled
	if (SFX_ON != _sfx_onoff)
		return;

	// read next background sound sample value from PGM mem
	_sfx_ambient = pgm_read_byte(&AMBIENT_SOUND[_sfx_amb_pos++]);
	if (_sfx_amb_pos > AMBIENT_LEN - 1)
		_sfx_amb_pos = 0;

	// just play the background sound if there's no SFX sample
	if (SAMPLE_PLAYING != _sfx_playState)
	{
		OCR2B = _sfx_ambient;
		return;
	}
	else
	{
		// determine sample state & ambient mix-in
		if (0 == _sfx_length)
		{
			// ramp down
			if (_sfx_sample < 128)
				_sfx_sample++;
			else if (_sfx_sample > 128)
				_sfx_sample--;
			else
			{
				_sfx_playState = SAMPLE_NONE;
				play_led_off();
			}

			OCR2B = ((uint16_t)_sfx_sample + (uint16_t)_sfx_ambient) >> 1;
		}
		else
		{
			_sfx_length--;
			
			// read the next sample value as long as we have data to read
			ee_readA(_sfxReadComplete);
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// enables all sound effects
void sfx_on(void)
{
	_sfx_onoff = SFX_ON;
	ampPwr_on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// disables all sound effects
void sfx_off(void)
{
	_sfx_onoff = SFX_OFF;
	ampPwr_off();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
uint8_t sfx_playing(void)
{
	return _sfx_playState;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Fills the header object with data
void _fillHeader(void)
{
	// load the header into the header struct
	ee_readBytes(0, sizeof(SOUND_HEADER), (uint8_t*) &_sfx_header);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// starts playback of a sample based on it's signature
void _sfx_startSampleComplete(uint8_t result)
{
	if (0 == result)
	{
		_sfx_playState = SAMPLE_PLAYING;
		play_led_on();
	}
	else
		_sfx_playState = SAMPLE_NONE;
}
void sfx_startSample(uint8_t index)
{
	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _sfx_playState)
		return;

	// make sure we don't try to start an invalid sample :)
	if (index >= _sfx_header.samples)
		return;

	// setup for sample playback
	_sfx_playState = SAMPLE_LOADING;
	_sfx_length	= _sfx_header.effects[index].length;
	ee_setpageA(_sfx_header.effects[index].startPage, _sfx_startSampleComplete);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a "random" ambient sound to play
void sfx_playAmbient(eventState_t state)
{
	static uint8_t	next		= 0;
	static uint8_t	delay		= 0;

	// decrement ambient delay if non-zero and return
	if (delay)
	{
		delay--;
		return;
	}

	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _sfx_playState)
	{
		delay = (TCNT2 >> 1) | 0x20;
		return;
	}

	uint8_t idx = 0;
	if (next)
	{
		idx = next;
		next = 0;
	}
	else
		idx = (TCNT2 >> 2) & 0x03;

	// play the specified sound effect
	switch (idx)
	{
		case 0:
			sfx_startSample(SFX_AMBIENT_BEEP_BEEP);
			break;
		case 1:
			sfx_startSample(SFX_AMBIENT_BEEP_UP);
			break;
		case 2:
			sfx_startSample(SFX_AMBIENT_KEY_BEEP);
			break;
		case 3:
			sfx_startSample(SFX_AMBIENT_WHIP_WHIP);
			break;
	}

	// normal exit
	delay = (TCNT2 >> 1) | 0x08;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// plays a background sound effect on a schedule
void sfx_playBackground(eventState_t state)
{
	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _sfx_playState)
		return;

	switch (TCNT2 & 0x01)
	{
		case 0:
			sfx_startSample(SFX_BK_ASTROGATOR);
			break;
		case 1:
			sfx_startSample(SFX_BK_MAIN_SCREEN);
			break;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a sound effects sequence
void sfx_playSequence(eventState_t state)
{
	static uint8_t	next		= 0;
	static uint8_t	delay		= 0;

	// decrement ambient delay if non-zero and return
	if (delay)
	{
		delay--;
		return;
	}

	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _sfx_playState)
	{
		delay = (TCNT2 >> 1) | 0x04;
		return;
	}

	// get entry point to sequences
	if (!next)
	{
		switch (TCNT2 & 0x07)
		{
			// kirk-beam-up sequence
			case 0:
				next = 1;
				break;
			// tribble sequence
			case 1:
				next = 10;
				break;
			// heart beats + bones doctor
			case 2:
				next = 20;
				break;
			// hailing kirk sequence
			case 3:
				next = 30;
				break;
			// bones walk out
			case 4:
				next = 40;
				break;
			case 5:
				next = 50;
				break;
			case 6:
				next = 60;
				break;
		}
	}

	// play the specified sound effect
	switch (next)
	{
		// kirk-beam-up sequence
		case 1:
			sfx_startSample(SFX_EFX_HAILING);
			next++;
			break;
		case 2:
			sfx_startSample(SFX_VOICES_SCOTTY_KIRK);
			next++;
			break;
		case 3:
			sfx_startSample(SFX_VOICES_BEAM_ME_UP);
			next++;
			break;
		case 4:
			sfx_startSample(SFX_VOICES_AYE_SIR);
			next++;
			break;
		case 5:
			sfx_startSample(SFX_EFX_ENERGIZE);
			next = 0;
			break;

		// tribble sequence
		case 10:
			sfx_startSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 11:
			sfx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 12:
			sfx_startSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 13:
			sfx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 14:
			sfx_startSample(SFX_VOICES_FASCINATING);
			next = 0;
			break;

		// heart beat + bones as a doctor
		case 20:
			sfx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 21:
			sfx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 22:
			sfx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 23:
			sfx_startSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 24:
			sfx_startSample(SFX_VOICES_WHAT_DO);
			next = 0;
		break;

		// hailing kirk sequence
		case 30:
			sfx_startSample(SFX_EFX_HAILING);
			next++;
			break;
		case 31:
			sfx_startSample(SFX_VOICES_BRIDGE_KIRK);
			next++;
			break;
		case 32:
			sfx_startSample(SFX_EFX_COMM);
			next++;
			break;
		case 33:
			sfx_startSample(SFX_VOICES_KIRK_HERE);
			next = 0;
			break;

		// going down
		case 40:
			sfx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 41:
			sfx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 42:
			sfx_startSample(SFX_VOICES_GOING_DOWN);
			next = 0;
			break;

		// medically impossible, his brain is gone	
		case 50:
			sfx_startSample(SFX_EFX_HEART);
			next++;
		case 51:
			sfx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 52:
			sfx_startSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 53:
			sfx_startSample(SFX_EFX_HEART);
			next++;
		case 54:
			sfx_startSample(SFX_EFX_HEART);
			next = 0;
			break;

		case 60:
			next = 0;
			break;
	}

	// if we are playing a sequence, set a shorter delay
	if (next)
		delay = 1;
	else
		delay = (TCNT2 >> 1) | 0x80;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// sound effects
void sfx_soundEffectsHandler(eventState_t state)
{
	// let's make an optimization
	// "state" points to the address of the next event record
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// light effects
void sfx_lightEffectsHandler(eventState_t state)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// loads the effects patterns from eeprom and sets up the callback events
void sfx_initializeEffects(void)
{
	EVENT_HEADER hdr;
	for (uint16_t i = effectsStart; i < EEPROM_SIZE;)
	{
		eeprom_read_block(&hdr, &i, sizeof(EVENT_HEADER));

		// move index to next header entry
		i += sizeof (EVENT_HEADER) + (hdr.Count * sizeof(EVENT_ENTRY));

		// break enumeration if null record
		if (hdr.RecordType == RT_NULL)
			break;

		// skip empty headers		
		if (0 == hdr.Count)
			continue;
		
		// register sound effects header
		if (0 != (hdr.RecordType & RT_SOUND))
			registerEvent(sfx_soundEffectsHandler, hdr.Delay, 0);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void sfx_init(void)
{
	audioOut_en();
	ampPwr_en();
	ampPwr_off();

	_sfx_playState		= SAMPLE_NONE;
	_sfx_onoff			= SFX_OFF;

	// timer2, PWM timer, used for audio output
	// Set fast PWM mode  (p.157)
	PRR		&= ~(1<<PRTIM2);
	TCCR2A	= (1<<WGM21) | (1<<WGM20) | (1<<COM2B1) | (1<<COM2B0);
	TCCR2B	= (1<<CS20);

	// initialize the eeprom hardware
	ee_init();

	// load the header data
	_fillHeader();

	registerEvent(sfx_sampleCallback, 0, EVENT_STATE_NONE);
	registerEvent(sfx_playAmbient, 2650, EVENT_STATE_NONE);
	registerEvent(sfx_playSequence, 1000, EVENT_STATE_NONE);
	registerEvent(sfx_playBackground, 28500, EVENT_STATE_NONE);
}
