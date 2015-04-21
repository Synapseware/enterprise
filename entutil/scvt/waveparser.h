#ifndef __WAVEPARSER_H__
#define __WAVEPARSER_H__

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

// Parses standard WAV files

class WaveData
{
public:
	WaveData(int length, int sampleRate, const char* data);
	~WaveData(void)
	{
		free((void*)_data);
	}

	int length(void);
	int sampleRate(void);
	const char* buffer(void);
private:
	const char*		_data;
	int				_length;
	int				_rate;
};

class WaveParser
{
public:
	WaveData * parse(FILE * file);
private:
};

#endif
