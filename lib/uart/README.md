uart
====
UART support library is meant to interact with the USART in the AVR cores.  It's a very simple, basic clas which is primarily responsible for reading and writing data over the serial port in a byte-wise fashion.  It's main advantage is a consolidation of control register access into a set of methods contained within the Uart class.

## Usage

### Instantiation
Declare a Uart instance with the following C++ code:
```c++
char buff[32];
RingBuffer rbuff(buff, sizeof(buff) * sizeof(char));

static Uart uart(rbuff);
```
> The Uart class is designed for asynchronous usage and works best with a RingBuffer class for temporary holding of data.

### Event handling
To take advantage of any asynchronous processing it's necessary to declare an "event handler" or ISR routine which can connect the interrupt code back with a class instance.  The most convenient way to do this is via an ISR declared like so:
```c++
ISR(UART_RX_vect)
{
    // take our received byte
    char data = UDR0;

    // assuming a variable named uart which is an instance of the Uart class,
    // pass the received data to it via the reciveHandler(char) method
    uart.receiveHandler(data);
}
```
> Note: The Uart class enables the recieve data interrupt flag, so if the global interrupt enable flag is set, then receive interrupts will be called by the AVR core.  It's important to have a handler setup!

* Internally, the Uart::receiveHandler(char data) method checks for a valid C++ function pointer and invokes the function, passing the received data to it.  This two-step call through process is a convenience for using the Uart class instance to interact with the callback model and the event processing.  Of course you could take the received data from the ISR routine and pass it directly to the same function.  These methods exist as a convenience only.
```c++
void (*void)(char)
```
