#include "stitch.h"
#include "stitchfile.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		showHelp();
		return 0;
	}


	// allocate the source file array
	char* sourceFiles = new char[argc * 25];
	const char* outputFile = "~/test.bin";

	// allocate the stitch file class
	StitchFile * stitch = new StitchFile();

	stitch->Process(outputFile, sourceFiles, argc - 2, 256);

	delete(stitch);
	delete(sourceFiles);

	return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void showHelp(void)
{
	cout << "stitch - wave utility builder for the Enterprise" << endl;
	cout << endl;
	cout << "stitch -o outputfile -p {pagesize} {input file 1...n}" << endl;
	cout << "   combines a set of wav files into a single output file suitable for programming to the EEPROM on the Enterprise board." << endl;
	cout << "   stitch -o enterprise.bin -p 256 file1.wav file2.wav file3.wav" << endl;
	cout << "   pagesize should be set to 256 for the AT24C series eeproms." << endl;
	cout << "   -o  Output file name." << endl;
	cout << "   -p  Page size.  This must reflect the page size of the target hardware.  256 bytes is the required size for the AT24C1024 chip." << endl;
	cout << "   {input files}  Give a list of pre-processed WAV files.  Preprocessing is done by the scvt (Sound Convert) utility." << endl;
	cout << endl;
}
