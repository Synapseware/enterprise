AVRDUDE commands
================
For interacting with the Enterprise main board.

# Upload & Fuses

### programming
	avrdude -c usbtiny -p m328p -B 1 -U flash:w:.\release\enterprise.hex

### reading fuses
	avrdude -c usbtiny -p m328p -B 8 -U hfuse:r:.\high.txt:h -U lfuse:r:.\low.txt:h -U efuse:r:.\ext.txt:h

### writing fuses
*(external crystal)*
	avrdude -c usbtiny -p m328p -B 8 -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

*(internal 8mHz r/c)*
	avrdude -c usbtiny -p m328p -B 8 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m



## Build tool test...
echo avrdude -c usbtiny -p $(avrdevice) -B 8 -U flash:w:"$(OutputDirectory)\$(MSBuildProjectName).hex" > upload.bat



avrdude -c usbtiny -p usb162 -U hfuse:r:.\high.txt:h -U lfuse:r:.\low.txt:h -U efuse:r:.\ext.txt:h
avrdude -c usbtiny -p usb162 -U lfuse:w:0xde:m -U hfuse:w:0xd9:m -U efuse:w:0xf7:m
avrdude -c usbtiny -p usb162 -U lfuse:w:0xde:m -U hfuse:w:0xd1:m -U efuse:w:0xf4:m


avrdude -c usbtiny -p usb162 -U lfuse:w:0xff:m
avrdude -c usbtiny -p usb162 -U hfuse:w:0xd1:m
avrdude -c usbtiny -p usb162 -U efuse:w:0xf4:m


## i2c/twi execution notes:
	@ 400kHz (1/4x10+6 = 2.5x10-6), it takes 2.5uS to transfer a single bit. 2.5x10-6 x 8 = 20uS per byte.  The AVR core @ 20MHz can theoretically execute 2x10+8/second or 1 instruction every 50x10-12 (50nS) seconds.  20uS is 200 times slower, so 200 instructions for every i2c byte transfer.
    It's well worth it to have the I2C interface be interrupt driven.

