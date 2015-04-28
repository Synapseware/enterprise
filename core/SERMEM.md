SERMEM (or Serial Memory Interface)
===================================
A basic serial interface to the onboard memory on the Enterprise.  This class provides a mechanism by which the user can format, write and read the effects data stored in the Enterprise's onboard computer system.

*Note:* All data is transfered via the USB to Serial interface.  The default BAUD rate is 57.6k which seems to strike a good balance between speed and error rates.

# Writing data to the EEPROM
This sequence describes the process that a user should follow to write data to EEPROM.

## Command sequence
* Enter the appropriate mode (A for Auto, M for Manual).
    - Auto mode is best suited to a programmatic interface because messages from the Enterprise are suppressed and only status codes are returned
    - Auto mode is initiated by sending an 'A'
    - Manual mode is initiated by sending a 'M'
* Tell the Enterprise how much data you'll be sending.
    - Send a 'T' followed by the number of bytes, printed as string.  For example, to let the Enterprise know you'll be sending a 4K block, send the string "T4096".
* Start the Write process by sending a 'W'.
    - If the transfer size is invalid (0 or >256k) the Enterprise will respond with a message (if not in Auto mode) and an error code of 'E'.
* Start uploading the file data.  When complete the Enterprise will respond with 'N' (or NACK).

Here's the call sequence:

    Computer                                Enterprise
        A                                       A
        T{transfer size}                        A
        W                                       A
        {upload bytes}                          N
    (complete)

The Enterprise responds with single byte response codes.  'A' is ACK, 'N' is NACK, 'E' is error.


## Transfer Speeds
These are observed transfer speeds using a logic analyzer.

    Interface                   Rate                   Byte Transfer time
    ------------------------    -------------------    --------------------
    Serial/UART:                38.4K (BAUD)           ~ 234.5mS/byte
    I2C                         400K                   21.5uS/byte

    Math:
    UART: 1/38400 * 9 = 234.5mS   (8N1)
    I2C:  1/400000 * 9 = 22.5uS   (8 data + 1 for ack/nack)

