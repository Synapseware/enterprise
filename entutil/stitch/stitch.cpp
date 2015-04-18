#include "stitch.h"
#include "stitchfile.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout << "argc: " << argc << "\n";

	for (int i = 0; i < argc; i++)
	{
		cout << "arg " << i << " = '" << argv[i] << "'\n";
	}

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
	cout << "stitch - wave utility builder for the Enterprise\n";
	cout << "\n";
	cout << "stitch -o outputfile -p {pagesize} {input file 1...n}\n";
	cout << "   combines a set of wav files into a single output file suitable for programming to the EEPROM on the Enterprise board.\n";
	cout << "   stitch -o enterprise.bin -p 256 file1.wav file2.wav file3.wav\n";
	cout << "   pagesize should be set to 256 for the AT24C series eeproms.\n";
	cout << "\n\n";
}
