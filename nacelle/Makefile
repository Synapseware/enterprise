# Hey Emacs, this is a -*- makefile -*-

#=================================================================================
# User configuration
#=================================================================================

MCU = attiny2313
F_CPU = 20000000

# Specify the target file, without an extension
TARGET = nacelle

# List all the include directories, one each on it's own line
INCDIR = /usr/share/avra		\
	../libs/drivers				\
	../libs/drivers/stp16cp05



#=================================================================================
# MAKE directives.
#=================================================================================
# Expand the INCDIR to include the -I prefix for each entry
INC = $(INCDIR:%= -I %)

SIZE = avr-size
HEXSIZE = $(SIZE) $(TARGET).hex
EEPSIZE = $(SIZE) $(TARGET).eep



all: build


build:
	@echo Building $(TARGET)...
	@echo
	@avra -W NoPRAGMA $(INC) -o $(TARGET).hex $(TARGET).asm 1>/dev/null
#2>/dev/null
	@echo
	@if test -f $(TARGET).eep.hex; then mv $(TARGET).eep.hex $(TARGET).eep; \
	2>/dev/null; fi
	$(HEXSIZE)
	$(EEPSIZE)

clean:
	@echo Removing build artifacts
	@rm -f $(TARGET).hex
	@rm -f $(TARGET).eep
	@rm -f $(TARGET).cof
	@rm -f $(TARGET).obj
	@echo