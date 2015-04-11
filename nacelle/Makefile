# Hey Emacs, this is a -*- makefile -*-

#=================================================================================
# User configuration
#=================================================================================

MCU = attiny2313

# Fuse values
L_FUSE = 0xFF
H_FUSE = 0x9F
E_FUSE = 0xFF

# Processor frequency
F_CPU = 20000000

# Specify the target file, without an extension
TARGET = nacelle

# List all the include directories, one each on it's own line
LIBSDIR = /usr/share/avra/includes		\
	../libs/drivers						\
	../libs/drivers/stp16cp05



# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS = $(LIBSDIR)



# Output format. (can be srec, ihex, binary)
FORMAT = ihex


AVRA = ~/src/synapseware/avra/src/avra

#---------------- Programming Options (avrdude) ----------------

# Programming hardware
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = stk500v2

# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = com6    # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)






#=================================================================================
# MAKE directives.
#=================================================================================
# Expand the LIBSDIR to include the -I prefix for each entry
INC = $(LIBSDIR:%= -I %)

SIZE = avr-size
HEXSIZE = $(SIZE) $(TARGET).hex
EEPSIZE = $(SIZE) $(TARGET).eep



all: sizebefore clean compile sizeafter

build: sizebefore compile sizeafter

sizebefore:
	@if test -f $(TARGET).hex; then echo; $(HEXSIZE); \
	2>/dev/null; fi
	@if test -f $(TARGET).eep; then echo; $(EEPSIZE); \
	2>/dev/null; fi

sizeafter:
	@if test -f $(TARGET).hex; then echo; $(HEXSIZE); \
	2>/dev/null; fi
	@if test -f $(TARGET).eep; then echo; $(EEPSIZE); \
	2>/dev/null; fi


compile:
	@echo Building $(TARGET)...
	@$(AVRA) $(INC) -o $(TARGET).hex $(TARGET).asm 1>/dev/null
	@echo
	@if test -f $(TARGET).eep.hex; then mv $(TARGET).eep.hex $(TARGET).eep; \
	2>/dev/null; fi


# Program the device.  (removed from line below: $(TARGET).hex $(TARGET).eep)
program: compile
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

# Write fuse values
wfuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:$(L_FUSE):m -U hfuse:w:$(H_FUSE):m -U efuse:w:$(E_FUSE):m

# Read fuse values
rfuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U hfuse:r:./high.txt:h -U lfuse:r:./low.txt:h -U efuse:r:./ext.txt:h


clean:
	@rm -f $(TARGET).hex
	@rm -f $(TARGET).eep
	@rm -f $(TARGET).cof
	@rm -f $(TARGET).obj
	@rm -f $(TARGET).lst
	@echo