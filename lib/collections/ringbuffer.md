RingBuffer
==========
The RingBuffer class implements a simple ring buffer.  It provides methods to read & write to the buffer, determine if the buffer is full or empty.

## Instantiation
The RingBuffer, at it's core, is a wrapper around a char array of some static size.  The object takes the buffer pointer and buffer size in it's constructor.

```c++
char buffer[128];
RingBuffer ring_buff(buffer, sizeof(buffer)/sizeof(char));
```

## Adding items to the buffer
Adding items to the buffer will succeed as long as the buffer is not full.  Once the buffer is full, subsequent calls to __RingBuffer::Put(char)__ without first doing a __RingBuffer::Get(void)__ will return -1 indicating that the write failed.

```c++
int RingBuffer::Put(char value);
```

## Fetching items from the buffer
Getting an item from the buffer essentially "removes" it from the list.  While the data type for the buffer is __char__, the __RingBuffer::Get(void)__ returns an __int__.  Therefore, it's advised to check the result of the get and if it's -1, discard the value.

```c++
int RingBuffer::Get(void);
```

## Buffer checks and status
The RingBuffer provides several methods for examining it's state.  The obvious methods are called out below.

The buffer will return __true__ if its count value is 0.
```c++
bool RingBuffer::IsEmpty(void);
```

The buffer will return __true__ if the count is equal to the buffer's given size.
```c++
bool RingBuffer::IsFull(void);
```

Returns the current count of items, starting from 0.
```c++
int RingBuffer::Count(void);
```
