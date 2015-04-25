Enterprise
==========
The Enterprise is a project which adds sound and light effects to a small plastic model USS Enterprise from the original Star Trek TV show.  There are various pieces and parts to make this massively over complicated system work.  For me, this project is about learning better techniques for making circuit boards, programming embedded systems and generally wasting massive amounts of time.

# Project Structure
## /board
This is where the Eagle CAD files reside.  You should use Eagle v7 (or greater) to view/edit.
## /core
The heart of the system, driven by an Atmel ATMega328p micro controller clockec at 20MHz.  This is a hand built system and *does not run any Arduino firmware.*  The core is responsible for playing sound effects and coordinating light effects throughout the ship.

## /entutil
Various programs written in C++ (tested only on Linux), mainly for converting 8bit 8KHz wave files to a standard EEPROM image file for uploading to the core.

## /libs
It's horrible, but this is the Git submodule which references the [/libs](https://github.com/Synapseware/libs/libs.git) repository.  It's a container repo for various hardware drivers and more reusable system-level software components.

## /loader
Dead folder.

## /nacelle
A sub project within the main Enterprise project, the [Nacelle](https://github.com/Synapseware/enterprise/tree/master/nacelle) folder contains all the Atmel assembly language files for the nacelle lighting effects boards.

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

