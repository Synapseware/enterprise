#include "scvt.h"
#include "waveparser.h"

using namespace std;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Converts a WAV file to an AVW file, the intermediary format
bool convertFile(const char* inputFile)
{
	FILE * file = fopen(inputFile, "r");
	if (NULL == file)
	{
		cout << "Error: Could not open input file " << inputFile << endl;
		return false;
	}

	int len = strlen(inputFile);
	char* outputFile = (char*)malloc(len + 1);
	strcpy(outputFile, inputFile);
	strcpy(outputFile + len - 3, "snd");

	cout << "Input:  " << inputFile << endl;
	cout << "Output: " << outputFile << endl;

	WaveParser * parser = new WaveParser();
	WaveData * data = parser->parse(file);
	if (NULL != data)
	{
		// write the sound data out to disk
		ofstream fout(outputFile, ios::out | ios::binary);
		fout.write(data->buffer(), data->length());
		fout.flush();
		fout.close();
		delete(data);
	}
	else
	{
		cout << "Error: Parsing failed." << endl;
	}

	delete(parser);

	return true;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		showHelp();
		return 0;
	}

	char* inputFile = NULL;
	inputFile = argv[1];

	if (inputFile == NULL || 0 == strlen(inputFile))
	{
		cout << "Error: missing or invalid input file." << endl;
		return -1;
	}

	// write length of file to output stream
	convertFile(inputFile);

	return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void showHelp(void)
{
	cout << "scvt - wave utility for single file conversion." << endl;
	cout << endl;
	cout << "scvt {input file}" << endl;
	cout << "   Converts a standard wave file to the intermediary Enterprise format, snd" << endl;
	cout << endl;
}
