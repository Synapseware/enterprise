#include "stitchfile.h"



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Processes the source files and generates the output file
bool StitchFile::Process(const char * outputFile, const char* sourceFiles[], int fileCount, int pageSize)
{
	ofstream outfile;
	outfile.open(outputFile, ios::out | ios::binary);

	// write the number of samples to the output file
	uint16_t count = fileCount;
	outfile.write((char*)&count, 2);

	// save header position
	int headerPos = outfile.tellp();
	int dataPos = pageSize;

	// write each wave file to the output stream
	for (int i = 0; i < fileCount; i++)
	{
		// write out header info
		uint16_t startPage = (dataPos / pageSize);
		outfile.seekp(headerPos);
		outfile.write((char*)&startPage, 2);
		headerPos = outfile.tellp();

		// write the contents of the source file to the output stream
		outfile.seekp(dataPos);
		ifstream sourceFile(sourceFiles[i], ios::in | ios::binary);
		outfile << sourceFile.rdbuf();
		sourceFile.close();

		// finish out the file with 0's
		while ((outfile.tellp() & pageSize) != 0)
			outfile.write((char*)'\0', 1);

		// save our data position
		dataPos = outfile.tellp();
	}

	outfile.flush();
	outfile.close();


	return true;
}

