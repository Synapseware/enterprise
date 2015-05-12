#include "effects.h"




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Constructor
SoundEffects::SoundEffects(void)
{
	_sample		 		= 0;
	_length		 		= 0;
	_playState	  		= 0;
	_onoff				= 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Fills the header object with data
void SoundEffects::fillHeader(void)
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
void SoundEffects::readComplete(uint8_t sfxdata)
{

	_sample = sfxdata;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Runs on the 8kHz event handler to play back PWM audio!
void SoundEffects::Render(void)
{
	static uint16_t ambientPos = 0;

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
	if (_length)
	{
		_length--;

		// read the next sample value as long as we have data to read
		ee_readA(readComplete);
	}
	else
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
	}

	// take the average of the sample and the ambient effects
	OCR2B = ((uint16_t)_sample + (uint16_t)ambient) >> 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// enables all sound effects
void SoundEffects::On(void)
{
	_onoff = SFX_ON;
	ampPwr_on();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// disables all sound effects
void SoundEffects::Off(void)
{
	_onoff = SFX_OFF;
	ampPwr_off();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// starts playback of a sample based on it's signature
void SoundEffects::startSampleComplete(uint8_t result)
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
// Starts playback for the specified sample, by index
void SoundEffects::StartSample(uint8_t index)
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
	ee_setpageA(_header.effects[index].startPage, SoundEffects::startSampleComplete);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a "random" ambient sound to play
void SoundEffects::playAmbient(eventState_t state)
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
			StartSample(SFX_AMBIENT_BEEP_BEEP);
			break;
		case 1:
			StartSample(SFX_AMBIENT_BEEP_UP);
			break;
		case 2:
			StartSample(SFX_AMBIENT_KEY_BEEP);
			break;
		case 3:
			StartSample(SFX_AMBIENT_WHIP_WHIP);
			break;
	}

	// normal exit
	delay = (TCNT2 >> 1) | 0x08;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// plays a background sound effect on a schedule
void SoundEffects::playBackground(eventState_t state)
{
	// don't do anything if we are playing a sample
	if (SAMPLE_NONE != _playState)
		return;

	switch (TCNT2 & 0x01)
	{
		case 0:
			StartSample(SFX_BK_ASTROGATOR);
			break;
		case 1:
			StartSample(SFX_BK_MAIN_SCREEN);
			break;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// selects a sound effects sequence
void SoundEffects::playSequence(eventState_t state)
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
			StartSample(SFX_EFX_HAILING);
			next++;
			break;
		case 2:
			StartSample(SFX_VOICES_SCOTTY_KIRK);
			next++;
			break;
		case 3:
			StartSample(SFX_VOICES_BEAM_ME_UP);
			next++;
			break;
		case 4:
			StartSample(SFX_VOICES_AYE_SIR);
			next++;
			break;
		case 5:
			StartSample(SFX_EFX_ENERGIZE);
			next = 0;
			break;

		// tribble sequence
		case 10:
			StartSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 11:
			StartSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 12:
			StartSample(SFX_EFX_TRIBBLE);
			next++;
			break;
		case 13:
			StartSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 14:
			StartSample(SFX_VOICES_FASCINATING);
			next = 0;
			break;

		// heart beat + bones as a doctor
		case 20:
			StartSample(SFX_EFX_HEART);
			next++;
			break;
		case 21:
			StartSample(SFX_EFX_HEART);
			next++;
			break;
		case 22:
			StartSample(SFX_EFX_HEART);
			next++;
			break;
		case 23:
			StartSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 24:
			StartSample(SFX_VOICES_WHAT_DO);
			next = 0;
		break;

		// hailing kirk sequence
		case 30:
			StartSample(SFX_EFX_HAILING);
			next++;
			break;
		case 31:
			StartSample(SFX_VOICES_BRIDGE_KIRK);
			next++;
			break;
		case 32:
			StartSample(SFX_EFX_COMM);
			next++;
			break;
		case 33:
			StartSample(SFX_VOICES_KIRK_HERE);
			next = 0;
			break;

		// going down
		case 40:
			StartSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 41:
			StartSample(SFX_EFX_RED_ALERT);
			next++;
			break;
		case 42:
			StartSample(SFX_VOICES_GOING_DOWN);
			next = 0;
			break;

		// medically impossible, his brain is gone	
		case 50:
			StartSample(SFX_EFX_HEART);
			next++;
		case 51:
			StartSample(SFX_EFX_HEART);
			next++;
			break;
		case 52:
			StartSample(SFX_VOICES_IMPOSSIBLE);
			next++;
			break;
		case 53:
			StartSample(SFX_EFX_HEART);
			next++;
		case 54:
			StartSample(SFX_EFX_HEART);
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
// Register the SoundEffects events
void SoundEffects::RegisterEvents(Events & events)
{
	if (_header.samples < 1)
		return;

	events.registerEvent(playAmbient, 22500, 0);		// plays random ambient sounds
	events.registerEvent(playSequence, 10000, 0);		// plays special sound sequences
	events.registerEvent(playBackground, 38000, 0);		// plays random background sounds
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Initializes the global effects variables and primary effects events
void SoundEffects::Init(void)
{
	audioOut_en();
	ampPwr_en();
	Off();

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
}
