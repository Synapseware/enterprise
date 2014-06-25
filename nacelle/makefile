# Hey Emacs, this is a -*- makefile -*-


TARGET = nacelle

INCDIR = /usr/share/avra

#OMG - we can include files from OTHER directories.
avra -fO -I $(INCDIR) -o $(TARGET).hex $(TARGET).asm
avra -W NoPragma -fO -I /usr/share/avra/ -I ~/src/synapseware/enterprise/libs/drivers/ -I ~/src/synapseware/enterprise/libs/drivers/stp16cp05/ -o nacelle.hex nacelle.asm
