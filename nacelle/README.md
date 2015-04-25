Nacelle
=======
Nacelle lighting effects for the Enterprise project.

Platform:	ATTiny2313
Source:		AVR assembly

***Built using AVRA!***

## Theory
The Nacelle "driver" is an ATTiny2313 clocked at 20MHz.  It is configured to push data over a serial interface to a pair of chained ST16CP05 LED driver chips.  This was chosen over the original Charlie-plexed design for wiring simplicity.

The Nacelle driver board is designed to communicate with the Enterprise core board via an I2C interface.  The core will send effects commands to the driver boards at various times to change the speed, direction, brightness, etc, of the nacelle animations.
