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

	char* output = NULL;
	char** files = (char**) malloc(argc * sizeof(char*));
	int pageSize = 256;
	int fileCount = 0;

	memset(files, 0, argc * sizeof(char*));

	for (int i = 1; i < argc; i++)
	{
		if (0 == strncmp("-o", argv[i], 2))
			output = argv[++i];
		else if (0 == strncmp("-p", argv[i], 2))
			pageSize = atoi(argv[++i]);
		else
		{
			files[fileCount++] = argv[i];
		}
	}

	if (0 == fileCount)
	{
		cout << "Error: No input files specified." << endl;
		return -1;
	}

	if (NULL == output)
	{
		cout << "Error: No output file specified." << endl;
		return -1;
	}

	cout << "Stitching " << fileCount << " files together, into " << output << " with a pageSize of " << pageSize << endl;;

	// allocate the stitch file class
	StitchFile * stitch = new StitchFile();

	bool error = false;
	if (!stitch->Process(output, (const char**)files, fileCount, pageSize))
	{
		cout << "Failed to generate output file." << endl;
		error = true;
	}

	delete(stitch);
	free(files);

	return error
		? -1
		: 0;
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
