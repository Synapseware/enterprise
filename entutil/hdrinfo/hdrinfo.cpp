#include "hdrinfo.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		showHelp();
		return 0;
	}

	ifstream file(argv[1]);
	if (file.fail())
	{
		cout << "Error opening file " << argv[1] << "\n";
		return -1;
	}

	char buffer[256];
	file.read((char*)buffer, 256);
	file.seekg(0, ios::end);
	int filelen = file.tellg();
	file.close();

	cout << "Header info for " << argv[1] << "\n";
	for (int i = 0; i < sizeof(buffer); i++)
	{
		printf("%02x", (uint8_t) buffer[i]);
		if ((i+1) % 16 == 0 && i > 0)
			cout << "\n";
		else
			cout << " ";
	}
	cout << "\n";

	cout << "Header details:\n";

	uint16_t samples = *(uint16_t*) &buffer[0];
	cout << " Samples: " << samples << "\n";

	for (int i = 0; i < samples; i++)
	{
		int index = i * 6;
		uint16_t start = *(uint16_t*) &buffer[index + 2];
		uint32_t length = *(uint32_t*) &buffer[index + 4];

		cout << "-----------------------------------------------------------------\n";
		cout << (i + 1) << ")\n";
		printf("0x%04x\tStart Page:\t0x%04x\t%d\n", (index + 2), start, start);
		printf("0x%04x\tLength:\t\t0x%04x\t%d\n", (index + 4), length, length);
	}

	cout << "-----------------------------------------------------------------\n";
	cout << "Image size: " << filelen << "\n";
	cout << "pages used: " << filelen / 256 << "\n";

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void showHelp(void)
{
	cout << "hdrinfo {filename}\n";
	cout << "Shows header information for an EEPROM image file.\n";
	cout << "\n";
}