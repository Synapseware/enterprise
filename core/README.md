Enterprise Core
===============
# Processing
## The core is responsible for:
* Sending new lighting effects to the Nacelle's for storage via EEPROM
* Sound?
* Some lighting effects
* USB interface

## Unsupported functionality:
* Processing battery level
* Uploading new firmware to subordinate processors


## Hardware System Composition
* Main CPU board
  * Main CPU
  * core lighting effects support
  * sound effects & speaker driver
* Power & USB board
  * USB to COM port
  * charging LiPo battery
  * 5V boost/regulator
* Nacelle boards (2 total)
  * nacelle lighting effects

The power & USB board provides communications for the system via a USB to virtual COM port driver chip, as well as a 5V boost/regulator for powering the CPU's.  The board has 6 outputs, 2 for serial RX & TX, USB power, 5V boost, raw battery line and ground.  The connector is meant to mate to the main board so that the main board can receive power and communications lines without the risk of a highly complicated system board.

The main CPU board has a single ATMega328P micro-controller (uC) which drives the sound effects, core lighting effects and is able to communicate with the nacelle driver boards via I2C.


### Board Details

	Power Board pinout (facing up, pins down, L-R)
	1 - RX
	2 - TX
	3 - VBUS
	4 - VBATT
	5 - VBOOST (5V)
	6 - GND

	Nacelle Board pinout (facing up, pins down, L-R)
	1 - MOSI/SDA
	2 - MISO
	3 - SCK/SCL
	4 - Pwr
	5 - GND
	6 = Reset

## Diagnostics
There are 4 LEDs on the main board located near the programming header.  In order from board edge in:
* Heartbeat
* Sound play back
* Serial transfers
* I2C transfers

