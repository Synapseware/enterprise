#ifndef __UPLOAD_H__
#define __UPLOAD_H__


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fstream>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


using namespace std;

static const char* ARG_FILE = "-i";
static const char* ARG_PORT = "-p";
static const char* ARG_BAUD = "-b";
static const char* ARG_SAVE = "-s";


static const uint16_t	PAGE_SIZE			= 256;
static const int	MAX_IMAGE_SIZE			= 262144;


void showHelp(void);




#endif
