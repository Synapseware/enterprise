Enterprise Utility Program
==========================

## stitch
Stitch's together a series of raw audio files and generates the proper output bin file.  Stitch should operate on files that were created by the 'scvt' utility.

## scvt
Converts a single .wav file (RIFF WAVE format) file to the raw Enterprise EEPROM format, which is simply the audio data from the input wave file.  Files must be 8bit, mono, below 22kHz sample rate.  Ideally files will be 8kHz.


## upload
Uploads effects firmware to the Enterprise.  Default speed is 57600.

	upload -p /dev/ttyACM0 -i effects.bin
	upload -s 9600 -p /dev/ttyACM0 -i effects.bin
