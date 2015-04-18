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



WaveData * WaveParser::parse(const char* filename)
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
	ifstream data(filename, ios::in | ios::binary);

	// make sure this is RIFF file
	data.seekg(0);
	data.read(&buff[0], 4);
	if (0 != strncmp("RIFF", buff, 4)) // 52-49-46-46
	{
		cout << "File is not in RIFF format.\n";
		return NULL;
	}

	// make sure this is a WAVE file
	data.seekg(8);
	data.read(buff, 4);
	if (0 != strncmp("WAVE", buff, 4)) // 57-41-56-45
	{
		cout << "File is not in WAV format.\n";
		return NULL;
	}

	// find the "fmt " chunk length
	int format = 0;
	data.seekg(16);
	data.read((char*)&format, 4);
	int formatEnd = format + data.tellg();

	// get the compression code
	uint16_t compression = 0;
	data.read((char*)&compression, 2);
	if (compression != 1)
	{
		cout << "Compression code is invalid: " << compression << "\n";
		return NULL;
	}

	// get the channel count
	uint16_t channels = 0;
	data.read((char*)&channels, 2);
	if (1 != channels)
	{
		cout << "Can only read single channel audio files.\n";
		return NULL;
	}

	// get sample rate
	uint32_t rate = 0;
	data.read((char*)&rate, 4);
	if (rate > 22000)
	{
		cout << "Sample rate is higher than 22kHz\n";
		return NULL;
	}

	// skip 6 bytes past the current location...
	data.seekg(6, ios_base::cur);

	// check the sample resolution
	uint16_t resolution = 0;
	data.read((char*)&resolution, 2);
	if (8 != resolution)
	{
		cout << "Sample size of " << resolution << " is invalid.  Must be 8 bit encoded audio.\n";
		return NULL;
	}

	// seek to the audio buffer portion
	data.seekg(formatEnd);
	cout << "Header ends at " << formatEnd << "\n";

	// seek to the data block
	while (true)
	{
		data.read(buff, 4);
		if (0 == strncmp("data", buff, 4))
		{
			cout << "Found 'data' block at position " << data.tellg() << "\n";
			break;
		}

		uint32_t next = 0;
		data.read((char*)&next, 4);
		data.seekg(next, ios_base::cur);
	}

	// get length of audio data
	uint32_t length = 0;
	data.read((char*)&length, 4);
	if (length > 16777216)
	{
		cout << "sound data is greater than 16MB (24bit address).\n";
		return NULL;
	}

	cout << "Valid WAVE file detected:\n";
	cout << "  Resolution:  " << resolution << " bit\n";
	cout << "  Sample rate: " << rate << "kHz\n";
	cout << "  Data length: " << length << "\n";
	cout << "\n";

	char* buffer = (char*) malloc(length);
	data.read(buffer, length);
	data.close();

	//WaveData(int length, int sampleRate, const char* data, int len);
	return new WaveData(length, rate, buffer);
}
