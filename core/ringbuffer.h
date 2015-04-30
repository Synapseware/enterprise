#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__


#include <inttypes.h>
#include <stdbool.h>



class RingBuffer
{
public:
	// takes a pointer to the buffer and the size of the buffer
	RingBuffer(char* data, int size);

	// adds the given char to the buffer.  buffer will wrap around without error
	// and overwrite data if more data is added then has been read
	int Put(char value);

	// Removes the next char from the buffer, or returns -1 if the buffer is empty
	int Get(void);

	// Returns true if the buffer is empty
	bool IsEmpty(void);

private:
	char*	_buffer;
	int		_size;

	int		_readPos;
	int		_writePos;
};




#endif