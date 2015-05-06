#include "hdrinfo.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// gets the size of the file
int getFileSize(FILE* file)
{
	int current = ftell(file);

	fseek(file, 0, SEEK_END);
	int filesize = ftell(file);
	fseek(file, current, SEEK_SET);

	return filesize;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// writes header info to the console
void showHeader(FILE* file, const char* filename, int wrap)
{
	uint8_t buffer[256];
	int length = sizeof(buffer)/sizeof(uint8_t);
	memset(buffer, 0, length);

	cout << "Header info for " << filename << endl;
	if (fread(buffer, length, 1, file) < 1)
	{
		cout << " Failed to load header bytes" << endl;
		return;
	}

	cout << " Raw header data:" << endl;
	for (int i = 0; i < length; i++)
	{
		if (i > 0 && i % wrap == 0)
			cout << endl;

		printf(" 0x%02x", buffer[i]);
	}
	cout << endl << endl;

	cout << "Header details:" << endl;

	uint16_t samples = *(uint16_t*) &buffer[0];
	cout << " Samples: " << samples << endl;

	for (int i = 0; i < samples; i++)
	{
		int index = i * 6;
		uint16_t start = *(uint16_t*) &buffer[index + 2];
		uint32_t length = *(uint32_t*) &buffer[index + 4];

		cout << "-----------------------------------------------------------------" << endl;
		cout << (i + 1) << ")" << endl;
		printf("0x%04x\tStart Page:\t0x%04x\t%d\n", (index + 2), start, start);
		printf("0x%04x\tLength:\t\t0x%04x\t%d\n", (index + 4), length, length);
	}

	int filelen = getFileSize(file);

	cout << "-----------------------------------------------------------------" << endl;
	cout << "Image size: " << filelen << endl;
	cout << "pages used: " << filelen / 256 << endl;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		showHelp();
		return 0;
	}

	char* filename = NULL;
	int wrap = 16;

	// process input options
	for (int i = 1; i < argc; i++)
	{
		if (0 == strncmp("-w", argv[i], 2))
		{
			if (i < argc - 1)
			{
				int x = atoi(argv[i+1]);
				if (x > 0)
				{
					wrap = x;
					i++;
				}
			}
		}
		else
			filename = argv[i];
	}

	if (filename == NULL || 0 == strlen(filename))
	{
		cout << "Error: missing or invalid input file." << endl;
		return -1;
	}

	FILE* file = fopen(filename, "r");
	if (NULL == file)
	{
		cout << "Error: Could not open input file " << filename << endl;
		return false;
	}

	showHeader(file, filename, wrap);

	fclose(file);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void showHelp(void)
{
	cout << "hdrinfo {filename}" << endl;
	cout << "Shows header information for an EEPROM image file." << endl;
	cout << endl;
}