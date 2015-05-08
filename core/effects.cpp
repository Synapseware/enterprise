#include "effects.h"



// the SFX buffer has a base size + an overhead size.  The idea is that
// the sound data will never entirely drain (unless it's at the end)
// so that the data can be constantly read without needing to time the
// buffer running empty and getting refilled
static const uint8_t BUFFER_OVERHEAD = 10;
static char sound_buffer_raw[64 + BUFFER_OVERHEAD];
static RingBuffer sfx_buffer(sound_buffer_raw, sizeof(sound_buffer_raw)/ sizeof(char));

static Events *       _events;
static SOUND_HEADER   _header;
static uint8_t        _sample;
static uint32_t       _length;
static uint8_t        _playState;
static uint8_t        _onoff;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Fills the header object with data
static void fillHeader(void)
{
	// load the header into the header struct
	memset(&_header, 0, sizeof(SOUND_HEADER));
	ee_readBytes(0, &_header, 254);
	i2cSendStop();
	if (0xFFFF == _header.samples)
	{
		_header.samples = 0;
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// callback - writes the data to the buffer
void efx_readComplete(uint8_t sfxdata)
{
	sfx_buffer.Put(sfxdata);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// fills the ring buffer by loading 
void efx_fill_buffer(eventState_t state)
{
	uint8_t fillCount = (sizeof(sound_buffer_raw) / sizeof(char)) - BUFFER_OVERHEAD;
	while (fillCount-- && _length--)
	{
		ee_readA(&efx_readComplete);
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// reads the next byte of data from the ring buffer, and queues a fill
// operation if the buffer is low
uint8_t efx_read_next(void)
{
	int data = sfx_buffer.Get();
	if (sfx_buffer.Count() < BUFFER_OVERHEAD)
	{
		// queue another SFX block download
		_events->registerOneShot(efx_fill_buffer, 0, 0);
	}

	return (uint8_t) data;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Runs on the 8kHz event handler to play back PWM audio!
void efx_renderAudioData(void)
{
	static uint16_t	ambientPos	= 0;
	static uint8_t	sample		= 0;

	// don't do anything if sound-effects are not enabled
	if (SFX_ON != _onoff)
		return;

	// read next background sound sample value from PGM mem
	uint8_t ambient = pgm_read_byte(&AMBIENT_SOUND[ambientPos++]);
	if (ambientPos > AMBIENT_LEN - 1)
		ambientPos = 0;

	// just play the background sound if there's no SFX sample
	if (SAMPLE_PLAYING != _playState)
	{
		OCR2B = ambient;
		return;
	}

	// determine sample state & ambient mix-in
	if (_length && !sfx_buffer.IsEmpty())
	{
		_length--;

		// read the next sample value as long as we have data to read
		//ee_readA(&efx_readComplete);
		sample = efx_read_next();
	}
	else
	{
		// ramp down
		if (sample < 128)
			sample++;
		else if (sample > 128)
			sample--;
		else
		{
			_playState = SAMPLE_NONE;
		}
	}

	// take the average of the sample and the ambient effects
	OCR2B = ((uint16_t)sample + (uint16_t)ambient) >> 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// enables all sound effects
void efx_on(void)
{
	_onoff = SFX_ON;
	ampPwr_on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// disables all sound effects
void efx_off(void)
{
	_onoff = SFX_OFF;
	ampPwr_off();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
uint8_t efx_playing(void)
{

	return _playState;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Starts playback for the specified sample, by index
void efx_startSample(uint8_t index)
{
	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _playState || 0 == _header.samples)
		return;

	// make sure we don't try to start an invalid sample :)
	if (index >= _header.samples)
		return;

	// setup for sample playback
	_playState = SAMPLE_LOADING;
	_length	= _header.effects[index].length;
	ee_setpageA(_header.effects[index].startPage, &efx_startSampleComplete);

	efx_fill_buffer(0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// starts playback of a sample based on it's signature
void efx_startSampleComplete(uint8_t result)
{
	if (SFX_RESULT_SUCCESS == result)
	{
		_playState = SAMPLE_PLAYING;
		play_led_on();
	}
	else
	{
		_playState = SAMPLE_NONE;
		play_led_off();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a "random" ambient sound to play
void efx_playAmbient(eventState_t state)
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
	if (SAMPLE_NONE != _playState)
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
			efx_startSample(SFX_AMBIENT_BEEP_BEEP);
			break;
		case 1:
			efx_startSample(SFX_AMBIENT_BEEP_UP);
			break;
		case 2:
			efx_startSample(SFX_AMBIENT_KEY_BEEP);
			break;
		case 3:
			efx_startSample(SFX_AMBIENT_WHIP_WHIP);
			break;
	}

	// normal exit
	delay = (TCNT2 >> 1) | 0x08;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// plays a background sound effect on a schedule
void efx_playBackground(eventState_t state)
{
	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _playState)
		return;

	switch (TCNT2 & 0x01)
	{
		case 0:
			efx_startSample(SFX_BK_ASTROGATOR);
			break;
		case 1:
			efx_startSample(SFX_BK_MAIN_SCREEN);
			break;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a sound effects sequence
void efx_playSequence(eventState_t state)
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
	if (SAMPLE_NONE != _playState)
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
			efx_startSample(SFX_EFX_HAILING);
			next++;
			break;
		case 2:
			efx_startSample(SFX_VOICES_SCOTTY_KIRK);
			next++;
			break;
		case 3:
			efx_startSample(SFX_VOICES_BEAM_ME_UP);
			next++;
			break;
		case 4:
			efx_startSample(SFX_VOICES_AYE_SIR);
			next++;
			break;
		case 5:
			efx_startSample(SFX_EFX_ENERGIZE);
			next = 0;
			break;

		// tribble sequence
		case 10:
			efx_startSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 11:
			efx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 12:
			efx_startSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 13:
			efx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 14:
			efx_startSample(SFX_VOICES_FASCINATING);
			next = 0;
			break;

		// heart beat + bones as a doctor
		case 20:
			efx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 21:
			efx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 22:
			efx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 23:
			efx_startSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 24:
			efx_startSample(SFX_VOICES_WHAT_DO);
			next = 0;
		break;

		// hailing kirk sequence
		case 30:
			efx_startSample(SFX_EFX_HAILING);
			next++;
			break;
		case 31:
			efx_startSample(SFX_VOICES_BRIDGE_KIRK);
			next++;
			break;
		case 32:
			efx_startSample(SFX_EFX_COMM);
			next++;
			break;
		case 33:
			efx_startSample(SFX_VOICES_KIRK_HERE);
			next = 0;
			break;

		// going down
		case 40:
			efx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 41:
			efx_startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 42:
			efx_startSample(SFX_VOICES_GOING_DOWN);
			next = 0;
			break;

		// medically impossible, his brain is gone	
		case 50:
			efx_startSample(SFX_EFX_HEART);
			next++;
		case 51:
			efx_startSample(SFX_EFX_HEART);
			next++;
			break;
		case 52:
			efx_startSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 53:
			efx_startSample(SFX_EFX_HEART);
			next++;
		case 54:
			efx_startSample(SFX_EFX_HEART);
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
// Initializes the global effects variables and primary effects events
int efx_init(Events* events)
{
	_events				= events;
	_sample		 		= 0;
	_length		 		= 0;
	_playState	  		= 0;
	_onoff				= 0;


	audioOut_en();
	ampPwr_en();
	efx_off();

	_playState		= SAMPLE_NONE;

	// timer2, PWM timer, used for audio output
	// Set fast PWM mode  (p.157)
	PRR		&= ~(1<<PRTIM2);
	TCCR2A	= (1<<WGM21) | (1<<WGM20) | (1<<COM2B1) | (1<<COM2B0);
	TCCR2B	= (1<<CS20);

	// initialize the eeprom hardware
	ee_init(TWI_SPEED);

	// load the header data
	fillHeader();

	// this handler renders the audio data to the PWM pin
	//_events->registerEvent(efx_renderAudioData, 0, 0);
	if (_header.samples > 0)
	{
		_events->registerEvent(efx_playAmbient, 2650, 0);		// plays random ambient sounds
		_events->registerEvent(efx_playSequence, 1000, 0);	// plays special sound sequences
		_events->registerEvent(efx_playBackground, 28500, 0);	// plays random background sounds
	}

	return _header.samples;
}
