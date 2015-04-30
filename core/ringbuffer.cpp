#include "ringbuffer.h"


RingBuffer::RingBuffer(char* data, int size)
{
	_buffer = data;
	_size = size;

	_readPos = 0;
	_writePos = 0;
}


int RingBuffer::Put(char value)
{
	_buffer[_writePos++] = value;
	if (_writePos >= _size)
		_writePos = 0;

	return _writePos;
}


int RingBuffer::Get(void)
{
	if (this->IsEmpty())
		return -1;

	if (_readPos >= _size)
		_readPos = 0;

	return _buffer[_readPos++];
}


bool RingBuffer::IsEmpty(void)
{
	return _readPos == _writePos
		? true
		: false;
}


bool RingBuffer::IsFull(void)
{
	return (_writePos -1) == _readPos
		? true
		: false;
}
