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

#include "../../lib//collections/ringbuffer.h"


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


bool test_getstr(void)
{
	file = fopen("data", "r");

	char buffer[256];
	memset(buffer, 0, sizeof(buffer));

	char* result = getstr(buffer, sizeof(buffer));
	cout << "Result is equal to buffer? " << (result == buffer ? "Yes" : "No") << endl;

	uint32_t length = atoi(buffer);
	cout << "Length: " << length << endl;

	fclose(file);

	return length == 257312
		? true
		: false;
}


void print_buffer(const char* buffer, int length)
{
	cout << "     -------- begin buffer view ------" << endl;
	for (int i = 0; i < length; i++)
	{
		printf("      Item %2d = %c\n", i, buffer[i]);
	}
	cout << "     ------ end buffer view --------" << endl << endl;
}

bool test_ringbuffer(void)
{
	cout << endl << "--------------------------------------------------" << endl;
	cout << "RingBuffer test" << endl << endl;

	char buffer[16];
	int bufferSize = sizeof(buffer) * sizeof(char);
	int bufferSlots = sizeof(buffer);
	memset(buffer, ' ', sizeof(buffer) * sizeof(char));

	RingBuffer * ring = new RingBuffer(buffer, bufferSize);

	cout << "  buffer initialized with " << bufferSlots << " elements" << endl;
	cout << "    buffer takes " << bufferSize << " bytes of memory" << endl;
	cout << "    buffer count " << ring->Count() << endl;
	cout << "    buffer is empty: " << (ring->IsEmpty() ? "true" : "false") << endl;
	cout << "    buffer is full: " << (ring->IsFull() ? "true" : "false") << endl << endl;
	cout << "  get on empty buffer returned " << ring->Get() << endl;

	cout << "  adding some items to the buffer" << endl;
	int i = 0;
	for (i = 0; i < sizeof(buffer) - 1; i++)
	{
		if (-1 == ring->Put((char) i + 'A'))
			cout << "    add failed at insert count = " << i << endl;
	}
	cout << "  added " << i << " items to the buffer" << endl;
	print_buffer(buffer, sizeof(buffer));

	cout << "  buffer status after adds" << endl;
	cout << "    buffer count " << ring->Count() << endl;
	cout << "    buffer is empty: " << (ring->IsEmpty() ? "true" : "false") << endl;
	cout << "    buffer is full: " << (ring->IsFull() ? "true" : "false") << endl << endl;

	cout << "  adding one more item to the buffer" << endl;
	if (-1 == ring->Put('3'))
		cout << "     add failed" << endl;
	else
		cout << "      buffer did not block add" << endl;
	cout << "    buffer count " << ring->Count() << endl;
	cout << "    buffer is empty: " << (ring->IsEmpty() ? "true" : "false") << endl;
	cout << "    buffer is full: " << (ring->IsFull() ? "true" : "false") << endl << endl;

	char first = ring->Get();
	cout << "  first entry is correct: " << ('A' == first ? "yes" : "no") << endl;
	cout << "    buffer count " << ring->Count() << endl;
	cout << "    buffer is empty: " << (ring->IsEmpty() ? "true" : "false") << endl;
	cout << "    buffer is full: " << (ring->IsFull() ? "true" : "false") << endl << endl;

	int count = 0;
	while(!ring->IsEmpty())
	{
		count++;
		ring->Get();
	}

	cout << "  drained " << count << " items from the ring buffer" << endl;
	cout << "    buffer count " << ring->Count() << endl;
	cout << "    buffer is empty: " << (ring->IsEmpty() ? "true" : "false") << endl;
	cout << "    buffer is full: " << (ring->IsFull() ? "true" : "false") << endl << endl;

	free(ring);
	return false;
}




int main(int argc, char* argv[])
{
	// test the getstr function
	if (test_getstr())
		cout << "getstr passed" << endl;
	else
		cout << "getstr failed" << endl;

	// test the ringbuffer
	if (test_ringbuffer())
		cout << "ringbuffer passed" << endl;
	else
		cout << "ringbuffer failed" << endl;
}

