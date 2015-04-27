#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fstream>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

FILE * file;

char read(void)
{
	char data = '\0';
	fread(&data, 1, 1, file);

	return data;
}


char * getstr(char * pstr, uint16_t max)
{
	char data;
	max--;			// make sure we have room for the null character
	while (max)
	{
		data = read();
		if (data == '\r' || data == '\n' || data == '\0')
			break;
		*pstr++ = data;
		max--;
	}
	*pstr='\0';
	
	return pstr;
}


int main(int argc, char* argv[])
{
	file = fopen("data", "r");

	char buffer[256];
	memset(buffer, 0, sizeof(buffer));

	char* result = getstr(buffer, sizeof(buffer));
	cout << "Result is equal to buffer? " << (result == buffer ? "Yes" : "No") << endl;

	uint32_t length = atoi(buffer);
	cout << "Length: " << length << endl;

	fclose(file);
}