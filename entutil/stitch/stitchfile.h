#ifndef __STICHFILE_H__
#define __STICHFILE_H__

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdint.h>

using namespace std;


class StitchFile
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
		uint16_t			startPage;				// + 2		2
		uint32_t			length;					// + 4		4
	} SOUND_EFFECT;									// 6 bytes

	// SOUND_HEADER reflects the actual header record
	typedef struct
	{
		uint16_t			samples;				// + 0	// # of samples (0-255)
		SOUND_EFFECT		effects[];				// + 2	// array of sound effects
	} SOUND_HEADER;									// 2 + 42 * 6 bytes = 254 bytes


	bool Process(const char* outputFile, const char[] sourceFiles, int fileCount, unsigned short pageSize);
private:
};


#endif
