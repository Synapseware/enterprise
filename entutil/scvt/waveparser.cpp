#include "waveparser.h"


WaveData::WaveData(int length, int sampleRate, const char* data)
{
	_length		= length;
	_rate		= sampleRate;
	_data		= data;
}

int WaveData::length(void)
{
	return _length;
}

int WaveData::sampleRate(void)
{
	return _rate;
}

const char* WaveData::buffer(void)
{
	return _data;
}


WaveData * WaveParser::parse(FILE * file)
{
	// WAV header format:
	// https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
	/*
	The canonical WAVE format starts with the RIFF header:

	0         4   ChunkID          Contains the letters "RIFF" in ASCII form
									(0x52494646 big-endian form).
	4         4   ChunkSize        36 + SubChunk2Size, or more precisely:
									4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
									This is the size of the rest of the chunk 
									following this number.  This is the size of the 
									entire file in bytes minus 8 bytes for the
									two fields not included in this count:
									ChunkID and ChunkSize.
	8         4   Format           Contains the letters "WAVE"
									(0x57415645 big-endian form).

	The "WAVE" format consists of two subchunks: "fmt " and "data":
	The "fmt " subchunk describes the sound data's format:

	12        4   Subchunk1ID      Contains the letters "fmt "
									(0x666d7420 big-endian form).
	16        4   Subchunk1Size    16 for PCM.  This is the size of the
									rest of the Subchunk which follows this number.
	20        2   AudioFormat      PCM = 1 (i.e. Linear quantization)
									Values other than 1 indicate some 
									form of compression.
	22        2   NumChannels      Mono = 1, Stereo = 2, etc.
	24        4   SampleRate       8000, 44100, etc.
	28        4   ByteRate         == SampleRate * NumChannels * BitsPerSample/8
	32        2   BlockAlign       == NumChannels * BitsPerSample/8
									The number of bytes for one sample including
									all channels. I wonder what happens when
									this number isn't an integer?
	34        2   BitsPerSample    8 bits = 8, 16 bits = 16, etc.
				2   ExtraParamSize   if PCM, then doesn't exist
				X   ExtraParams      space for extra parameters

	The "data" subchunk contains the size of the data and the actual sound:

	36        4   Subchunk2ID      Contains the letters "data"
									(0x64617461 big-endian form).
	40        4   Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
									This is the number of bytes in the data.
									You can also think of this as the size
									of the read of the subchunk following this 
									number.
	44        *   Data             The actual sound data.
	*/

	char buff[16];

	// make sure this is RIFF file
	fseek(file, 0, SEEK_SET);
	fread(buff, 4, 1, file);
	if (0 != strncmp("RIFF", buff, 4)) // 52-49-46-46
	{
		cout << "File is not in RIFF format." << endl;
		return NULL;
	}
	//cout << "Valid RIFF format detected." << endl;

	// make sure this is a WAVE file
	fseek(file, 8, SEEK_SET);
	fread(buff, 4, 1, file);
	if (0 != strncmp("WAVE", buff, 4)) // 57-41-56-45
	{
		cout << "File is not in WAV format." << endl;
		return NULL;
	}
	//cout << "Valid WAVE format detected." << endl;

	// find the "fmt " chunk length
	int formatEnd = 0;
	fseek(file, 16, SEEK_SET);
	fread(&formatEnd, 4, 1, file);
	formatEnd += ftell(file);

	// get the compression code
	uint16_t compression = 0;
	fread(&compression, 2, 1, file);
	if (compression != 1)
	{
		cout << "Error: Compression code is invalid: " << compression << endl;
		return NULL;
	}
	//cout << "No compression detected." << endl;

	// get the channel count
	uint16_t channels = 0;
	fread(&channels, 2, 1, file);
	if (1 != channels)
	{
		cout << "Error: Can only read single channel audio files." << endl;
		return NULL;
	}
	//cout << "Monural audio detected." << endl;

	// get sample rate
	uint32_t rate = 0;
	fread(&rate, 4, 1, file);
	if (rate != 8000)
	{
		cout << "Error: Sample rate is not 8kHz." << endl;
		return NULL;
	}
	//cout << "Valid sampling rate of 8kHz detected." << endl;

	// skip 6 bytes past the current location...
	fseek(file, 6, SEEK_CUR);

	// check the sample resolution
	uint16_t resolution = 0;
	fread(&resolution, 2, 1, file);
	if (8 != resolution)
	{
		cout << "Error: Sample size of " << resolution << " is invalid.  Must be 8 bit encoded audio." << endl;
		return NULL;
	}
	//cout << "Valid bit depth of 8 bits detected." << endl;

	// seek to the audio buffer portion
	fseek(file, formatEnd, SEEK_SET);
	//cout << "Header ends at " << formatEnd << "" << endl;

	// seek to the data block
	while (true)
	{
		fread(buff, 4, 1, file);
		if (0 == strncmp("data", buff, 4))
		{
			cout << "Found 'data' block at position " << ftell(file) << endl;
			break;
		}

		uint32_t next = 0;
		fread(&next, 4, 1, file);
		fseek(file, next, SEEK_CUR);
	}

	// get length of audio data
	uint32_t length = 0;
	fread(&length, 4, 1, file);
	if (length > 16777216)
	{
		cout << "Error: Sound data is greater than 16MB (24bit address)." << endl;
		return NULL;
	}

	cout << "Valid WAVE file detected:" << endl;
	cout << "  Resolution:  " << resolution << " bit" << endl;
	cout << "  Sample rate: " << rate << "kHz" << endl;
	cout << "  Data length: " << length << endl;
	cout << endl;

	char* buffer = (char*) malloc(length);
	fread(buffer, length, 1, file);

	//WaveData(int length, int sampleRate, const char* data, int len);
	return new WaveData(length, rate, buffer);
}
