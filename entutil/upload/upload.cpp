#include "upload.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int open_port(const char* port)
{
	int fd; // file description for the serial port
	
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	
	if (fd == -1) // if open is unsucessful
	{
		cout << "open_port: Unable to open " << port << "." << endl;
		return -1;
	}

	fcntl(fd, F_SETFL, 0);
	cout << "port is open." << endl;

	return fd;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int configure_port(int fd, int baud)
{
	struct termios port_settings;				// structure to store the port settings in

	cfsetispeed(&port_settings, baud); 			// set baud rates
	cfsetospeed(&port_settings, baud);

	port_settings.c_cflag &= ~PARENB;			// set no parity, stop bits, data bits
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag |= CS8;

//	port_settings.c_cc[VMIN]     = 1;			// block until at least 1 byte is ready

	cfmakeraw(&port_settings);

	tcflush(fd, TCIFLUSH);						// flush the port
	tcsetattr(fd, TCSANOW, &port_settings);		// apply the settings to the port

	return fd;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void saveSettings(int baud, const char* port)
{
	ofstream file(".upload-settings", ios::out);
	if (!file.fail())
	{
		file << baud << "" << endl;
		file << port << "" << endl;
		file.flush();
		file.close();
	}
	else
		cout << "Failed to create settings file" << endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
bool loadSettings(int* baud, char** port)
{
	ifstream file(".upload-settings", ios::in);
	if (file.fail())
		return false;

	file >> *baud;

	char line[32];
	file.getline(line, sizeof(line));
	int len = strlen(line);
	*port = (char*) malloc(len);
	strcpy(*port, line);

	file >> *port;

	file.close();

	cout << "Loaded settings:" << endl;
	cout << "  Baud: " << *baud << "" << endl;
	cout << "  Port: " << *port << "" << endl;

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Reads the specified number of bytes from the serial port
bool readBytes(int fd, char* buffer, int bytesToRead)
{
	cout << "  Reading " << bytesToRead << " bytes" << endl;
	while (bytesToRead)
	{
		int bytesRead = read(fd, buffer, bytesToRead);
		if (bytesRead < 0)
		{
			cout << "read failed!" << endl;
			return false;
		}

		cout << "     read " << bytesRead << " bytes" << endl;
		bytesToRead -= bytesRead;
	}
	cout << "   Done reading" << endl;

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Waits for an ACK response from the Enterprise (an 'A' character)
bool waitForAck(int fd)
{
	int data = 0;
	readBytes(fd, (char*)&data, 1);
	printf("  rsp code: %#x\n", data);
	return 0xFF == data;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Performs a blocking write on the serial port
bool writeBytes(int fd, char* buffer, int bytesToWrite)
{
	while (bytesToWrite)
	{
		int bytesWritten = write(fd, buffer, bytesToWrite);
		if (bytesWritten < 0)
		{
			cout << "Failed to write data to serial port!" << endl;
		}
		bytesToWrite -= bytesWritten;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
bool processUpload(int fd, FILE* file)
{
	char buffer[128];

	// get file size
	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// don't allow excessive transfers
	if (fileSize > MAX_IMAGE_SIZE)
	{
		cout << "EEPROM image size (" << fileSize << ") exceeds maximum storage capacity on the Enterprise (256kb)" << endl;
		return false;
	}

	// begin upload processing
	// Send a 'A' to enter auto mode
	// 		Wait for ACK
	// Send a 'W' to enter write mode
	//		Wait for ACK
	// Send file size as 4 bytes (little Endian)
	//		Wait for ACK
	// Send first page of data
	//		Wait for ACK
	// Send remaining pages of data
	//		Wait for ACK at the end of each page
	// Client sends NACK when complete

	buffer[0] = 'A';
	writeBytes(fd, buffer, 1);
	if (!waitForAck(fd))
	{
		cout << "Could not enter AUTO mode" << endl;
		return false;
	}
	cout << "Entered AUTO mode" << endl;

	// enter write mode
	buffer[0] = 'W';
	writeBytes(fd, buffer, 1);
	if (!waitForAck(fd))
	{
		cout << "Could not enter WRITE mode" << endl;
		return false;
	}
	cout << "Entered WRITE mode" << endl;

	// let client know size of transfer
	writeBytes(fd, (char*)&fileSize, 4);
	if (!waitForAck(fd))
	{
		cout << "File size (" << fileSize << ") declined" << endl;
		return false;
	}

	// upload the file
	char fileData[PAGE_SIZE];
	int pageUploaded = 0;
	while (fileSize > 0)
	{
		cout << "Byte remaining: " << fileSize << endl;
		fread(fileData, sizeof(fileData), 1, file);
		writeBytes(fd, fileData, PAGE_SIZE);

		if (!waitForAck(fd))
		{
			cout << "Block transfer failed!" << endl;
			break;
		}

		fileSize -= PAGE_SIZE;

		pageUploaded++;
		printf("Page uploaded: %d\n", pageUploaded);
	}

	return fileSize == 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// uploads the file to the Enterprise
void upload(int baud, const char* port, const char* filename)
{
	int fd = open_port(port);
	if (-1 == fd)
		return;

	configure_port(fd, baud);
	FILE* file = fopen(filename, "r");

	cout << "Starting upload processing..." << endl;
	if (processUpload(fd, file))
		cout << "Successfully uploaded EEPROM image file." << endl;
	else
		cout << "Upload FAILED." << endl;

	close(fd);
	fclose(file);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int mapBaud(int baud)
{
	//Look up appropriate baud rate constant
	switch (baud)
	{
		case 19200:
			return  B19200;
		case 115200:
			return  B115200;
		case 57600:
			return B57600;
		case 9600:
			return  B9600;
		case 4800:
			return  B4800;
		case 2400:
			return  B2400;
		case 1800:
			return  B1800;
		case 1200:
			return  B1200;
	}

	cout << "Invalid baud rate: " << baud << endl;

	return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		showHelp();
		return 0;
	}

	int baud = 115200;
	char* port = NULL;
	char* file = NULL;
	bool save = false;

	loadSettings(&baud, &port);

	// -b 9600 -p /dev/ttyACM0 -i effects.bin -s
	for (int i = 1; i < argc; i++)
	{
		if (0 == strncmp("-p", argv[i], 2))
			port = argv[++i];
		else if (0 == strncmp("-b", argv[i], 2))
			baud = atoi(argv[++i]);
		else if (0 == strncmp("-s", argv[i], 2))
			save = true;
		else
			file = argv[i];
	}

	int actualBaud = mapBaud(baud);
	if (actualBaud < 1)
	{
		cout << "The specified baud rate of " << baud << " is not valid." << endl;
		return -1;
	}

	if (NULL == file)
	{
		cout << "Upload file not found." << endl;
		return 0;
	}

	cout << "Uploading " << file << " to the Enterprise on port " << port << " at " << baud << " baud." << endl;

	if (save)
		saveSettings(baud, port);

	upload(actualBaud, port, file);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void showHelp(void)
{
	cout << "upload {filename} -p {serial port} -b {baud rate} -s" << endl;
	cout << "Uploads an EEPROM effects file to the Enterprise." << endl;
	cout << "  {fielname} The EEPROM image file to upload to the Enterprise." << endl;
	cout << "  -b  Specify the baud rate to use.  Must be a standard value (e.g. 9600, 57600, etc)." << endl;
	cout << "  -p  Specify the serial port to use." << endl;
	cout << "         List serial ports with 'll /dev/tty*' at the command prompt." << endl;
	cout << "  -s  Save the given port and baud rate as the new defaults." << endl;
	cout << "         Note: settings are saved in the .upload-settings file in the current directory." << endl;
	cout << endl;
}