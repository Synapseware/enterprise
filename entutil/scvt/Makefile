# Standard Enterprise Utility Makefile
# Ramun McCallum
# 2015

TARGET = scvt
LIB = ../lib
OUTDIR = ../bin
OBJDIR = ../build


# List all C source files here
SRC = 

# List all C++ source files here
CPPSRC = $(LIB)/waveparser.cpp				\
		$(TARGET).cpp


# C defines
DEFS =

# C++ defines
CPPDEFS = 




#---------------- Build configurations ----------------

# Define programs and commands.
SHELL = sh
CC = g++
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp


# Messages
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:



# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -B $(LIB) $(CFLAGS) -Wall
ALL_CPPFLAGS =  -B $(LIB) -Wall -c
LDFLAGS = -B $(LIB) -o $(OUTDIR)/$(TARGET)


# Define all object files.
OBJ = $(SRC:%.c=%.o) $(CPPSRC:%.cpp=%.o)




all: clean begin build end


# Compile the target file
build: $(TARGET)


begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo



# Compile the C++ source
$(OBJDIR)/%.o : %.cpp
#	$< means .o, $@ means .cpp
	@echo
	@echo $(MSG_COMPILING_CPP) $< into $@
	@echo $(CC) $(ALL_CPPFLAGS) $< -o $@
#	@$(CC) $(ALL_CPPFLAGS) $< -o $<
	@echo

# Compile the target
#.SECONDARY: $(OBJ)
#.PRECIOUS: $(OBJ)
$(TARGET): $(OBJDIR)/%.o
	@echo
	@echo $(MSG_LINKING) $(TARGET) ": " $<
	@echo $(CC) $(OBJDIR)/$(notdir $(OBJ)) -o $(OUTDIR)/$(TARGET)
#	@$(CC) $(LDFLAGS) $@
	@echo


clean:
	@echo
	@echo $(MSG_CLEANING) $(TARGET)
	@$(RM) -f $(TARGET)
	@find ../ -type f -name "*.o" -delete
	@echo

find:
	@echo
	@echo "Finding all .o files"
	@find ../ -type f -name "*.o"
	@echo
