Enterprise Utility Programs
===========================

## stitch
Stitch's together a series of raw audio files and generates the proper output bin file.  Stitch should operate on files that were created by the 'scvt' utility.

### Help info
	stitch - EEPROM sound effects utility builder for the Enterprise
	   combines a set of wav files into a single output file suitable for programming to the EEPROM on the Enterprise board.

	stitch -o {outputfile} -p {pagesize} {input file 1...n}
	   -o  Output file name.
	   -p  Page size.  This must reflect the page size of the target hardware.  256 bytes is the required size for the AT24C1024 chip.
	   {input files}  Give a list of pre-processed WAV files.  Preprocessing is done by the scvt (Sound Convert) utility.


## scvt
Converts a single .wav file (RIFF WAVE format) file to the raw Enterprise EEPROM format, which is simply the audio data from the input wave file.  Files must be 8bit, mono, below 22kHz sample rate.  Ideally files will be 8kHz.  The extracted audio is always saved to a new file with an extension of "snd".

### Examples

	scvt spock-bridge.wav

Takes the "spock-bridge.wav" file, verifies that it contains only 8 bit mono sound data with an 8kHz sampling rate then extracts just the audio chunk and saves it to a new file named "spock-bridge.snd" file in the same directory as the input file.  Any existing file will be over written.

### Help info

	scvt {input file}
	   Converts a standard wave file to the intermediary Enterprise format.


## upload
Uploads an effects EEPROM image to the Enterprise.  Default speed is 57600.

### Examples:

	upload -p /dev/ttyACM0 -i {image-file}
	upload -b 9600 -p /dev/ttyACM0 -i {image-file}

### Saving settings:
To save the current port and BAUD rate, specify the -s flag.

### Help info
	upload -i {filename} -p {serial port} -b {baud rate} -s
	Uploads an EEPROM effects file to the Enterprise.
	  -i  The EEPROM image file to upload to the Enterprise.
	  -b  Specify the baud rate to use.  Must be a standard value (e.g. 9600, 57600, etc).
	  -p  Specify the serial port to use.
	         List serial ports with 'll /dev/tty*' at the command prompt.
	  -s  Save the given port and baud rate as the new defaults.
	         Note: settings are saved in the .upload-settings file in the current directory.


## hdrinfo
View information about an EEPROM image by examining the first 256 bytes of the file.

	hdrinfo {image-file}

