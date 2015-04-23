#include "effects.h"




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// This callback is used to dump the next value into the PWM stream
void sampleCallbackHandler(eventState_t state)
{
	if (0 == state)
		return;

	SoundEffects * effects = (SoundEffects*)state;
	effects->sampleCallback();
}
void playAmbientHandler(eventState_t state)
{
	if (0 == state)
		return;

	SoundEffects * effects = (SoundEffects*)state;
	effects->playAmbient();
}
void playSequenceHandler(eventState_t state)
{
	if (0 == state)
		return;

	SoundEffects * effects = (SoundEffects*)state;
	effects->playSequence();
}
void playBackgroundHandler(eventState_t state)
{
	if (0 == state)
		return;

	SoundEffects * effects = (SoundEffects*)state;
	effects->playBackground();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
SoundEffects::SoundEffects(Events* events)
{
	_events				= events;
	_sample		 		= 0;
	_ambient			= 0;
	_ambientPos			= 0;
	_length		 		= 0;
	_playState	  		= 0;
	_onoff				= 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Runs on the 8kHz event handler to play back PWM audio!
void SoundEffects::sampleCallback(void)
{
	// don't do anything if sound-effects are not enabled
	if (SFX_ON != _onoff)
		return;

	// read next background sound sample value from PGM mem
	_ambient = pgm_read_byte(&AMBIENT_SOUND[_ambientPos++]);
	if (_ambientPos > AMBIENT_LEN - 1)
		_ambientPos = 0;

	// just play the background sound if there's no SFX sample
	if (SAMPLE_PLAYING != _playState || 0 == _header.samples)
	{
		OCR2B = _ambient;
		return;
	}

	// determine sample state & ambient mix-in
	if (0 == _length)
	{
		// ramp down
		if (_sample < 128)
			_sample++;
		else if (_sample > 128)
			_sample--;
		else
		{
			_playState = SAMPLE_NONE;
			play_led_off();
		}

		OCR2B = ((uint16_t)_sample + (uint16_t)_ambient) >> 1;
	}
	else
	{
		_length--;

		// read the next sample value as long as we have data to read
		ee_readA(&Effects_readCompleteHandler);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void SoundEffects::readComplete(uint8_t sfxdata)
{
	_sample = sfxdata;

	// take the average of the sample and the ambient effects
	OCR2B = ((uint16_t)_sample + (uint16_t)_ambient) >> 1;

	if (!_length)
		ee_readEnd();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// starts playback of a sample based on it's signature
void SoundEffects::startSampleComplete(uint8_t result)
{
	if (0 == result)
	{
		_playState = SAMPLE_PLAYING;
		play_led_on();
	}
	else
		_playState = SAMPLE_NONE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// enables all sound effects
void SoundEffects::on(void)
{
	_onoff = SFX_ON;
	ampPwr_on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// disables all sound effects
void SoundEffects::off(void)
{
	_onoff = SFX_OFF;
	ampPwr_off();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
uint8_t SoundEffects::playing(void)
{

	return _playState;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Starts playback for the specified sample, by index
void SoundEffects::startSample(uint8_t index)
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
	ee_setpageA(_header.effects[index].startPage, &Effects_startSampleCompleteHandler);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a "random" ambient sound to play
void SoundEffects::playAmbient(void)
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
			startSample(SFX_AMBIENT_BEEP_BEEP);
			break;
		case 1:
			startSample(SFX_AMBIENT_BEEP_UP);
			break;
		case 2:
			startSample(SFX_AMBIENT_KEY_BEEP);
			break;
		case 3:
			startSample(SFX_AMBIENT_WHIP_WHIP);
			break;
	}

	// normal exit
	delay = (TCNT2 >> 1) | 0x08;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// plays a background sound effect on a schedule
void SoundEffects::playBackground(void)
{
	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _playState)
		return;

	switch (TCNT2 & 0x01)
	{
		case 0:
			startSample(SFX_BK_ASTROGATOR);
			break;
		case 1:
			startSample(SFX_BK_MAIN_SCREEN);
			break;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a sound effects sequence
void SoundEffects::playSequence(void)
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
			startSample(SFX_EFX_HAILING);
			next++;
			break;
		case 2:
			startSample(SFX_VOICES_SCOTTY_KIRK);
			next++;
			break;
		case 3:
			startSample(SFX_VOICES_BEAM_ME_UP);
			next++;
			break;
		case 4:
			startSample(SFX_VOICES_AYE_SIR);
			next++;
			break;
		case 5:
			startSample(SFX_EFX_ENERGIZE);
			next = 0;
			break;

		// tribble sequence
		case 10:
			startSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 11:
			startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 12:
			startSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 13:
			startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 14:
			startSample(SFX_VOICES_FASCINATING);
			next = 0;
			break;

		// heart beat + bones as a doctor
		case 20:
			startSample(SFX_EFX_HEART);
			next++;
			break;
		case 21:
			startSample(SFX_EFX_HEART);
			next++;
			break;
		case 22:
			startSample(SFX_EFX_HEART);
			next++;
			break;
		case 23:
			startSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 24:
			startSample(SFX_VOICES_WHAT_DO);
			next = 0;
		break;

		// hailing kirk sequence
		case 30:
			startSample(SFX_EFX_HAILING);
			next++;
			break;
		case 31:
			startSample(SFX_VOICES_BRIDGE_KIRK);
			next++;
			break;
		case 32:
			startSample(SFX_EFX_COMM);
			next++;
			break;
		case 33:
			startSample(SFX_VOICES_KIRK_HERE);
			next = 0;
			break;

		// going down
		case 40:
			startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 41:
			startSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 42:
			startSample(SFX_VOICES_GOING_DOWN);
			next = 0;
			break;

		// medically impossible, his brain is gone	
		case 50:
			startSample(SFX_EFX_HEART);
			next++;
		case 51:
			startSample(SFX_EFX_HEART);
			next++;
			break;
		case 52:
			startSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 53:
			startSample(SFX_EFX_HEART);
			next++;
		case 54:
			startSample(SFX_EFX_HEART);
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
uint16_t SoundEffects::init(void)
{
	audioOut_en();
	ampPwr_en();
	off();

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

	// this handler plays the main ambient sound
	_events->registerEvent(sampleCallbackHandler, 0, this);

	if (_header.samples > 0)
	{
		_events->registerEvent(playAmbientHandler, 2650, this);		// plays random ambient sounds
		_events->registerEvent(playSequenceHandler, 1000, this);	// plays special sound sequences
		_events->registerEvent(playBackgroundHandler, 28500, this);	// plays random background sounds
	}

	return _header.samples;
}
