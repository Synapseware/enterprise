#include "scvt.h"
#include "waveparser.h"

using namespace std;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Converts a WAV file to an AVW file, the intermediary format
void convertFile(const char* outputFile, const char* inputFile)
{
	cout << "Output: " << outputFile << "\n";
	cout << "Input:  " << inputFile << "\n";

	WaveParser * parser = new WaveParser();
	WaveData * data = parser->parse(inputFile);
	if (NULL != data)
	{
		// write the sound data out to disk
		ofstream fout(outputFile, ios::out | ios::binary);
		fout.write(data->buffer(), data->length());
		fout.flush();
		fout.close();
		delete(data);
	}

	delete(parser);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		showHelp();
		return 0;
	}


	char* outputFile = NULL;
	char* inputFile = NULL;

	for (int i = 0; i < argc - 1; i++)
	{
		if (0 == strncmp("-o", argv[i], 2))
			outputFile = argv[i + 1];

		else if (0 == strncmp("-i", argv[i], 2))
			inputFile = argv[i + 1];
	}

	if (outputFile == NULL)
	{
		cout << "Missing output file!\n";
		return -1;
	}
	if (inputFile == NULL)
	{
		cout << "Missing input file!\n";
		return -1;
	}

	// write length of file to output stream
	convertFile(outputFile, inputFile);

	return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void showHelp(void)
{
	cout << "scvt - wave utility for single file conversion.\n";
	cout << "\n";
	cout << "scvt -o outputfile {input file}\n";
	cout << "   Converts a standard wave file to the intermediary Enterprise format.\n";
	cout << "\n\n";
}
