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
int configure_port(int fd, int baud)      // configure the port
{
	struct termios port_settings;      // structure to store the port settings in

	cfsetispeed(&port_settings, baud);    // set baud rates
	cfsetospeed(&port_settings, baud);

	port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag |= CS8;
	
	tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port

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
	while (bytesToRead)
	{
		int bytesRead = read(fd, buffer, bytesToRead);
		if (bytesRead < 0)
		{
			fputs("read failed!\n", stderr);
			return false;
		}
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// Waits for an ACK response from the Enterprise (an 'A' character)
bool waitForAck(int fd)
{
	char data = '\0';
	readBytes(fd, &data, 1);
	return 'A' == data;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
bool processUpload(int fd, const char* filename)
{
	char buffer[128];

	// open file
	FILE* file = fopen(filename, "r");
	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// begin upload processing
	// Hello = 'A'
	// StoreFile = 'W'
	// RetrieveFile = 'R'
	// Format = 'F'
	write(fd, "A", 1);
	waitForAck(fd);
	cout << "Entered auto mode" << endl;

	// enter write mode
	write(fd, "W", 1);
	waitForAck(fd);
	cout << "Entered write mode" << endl;

	// get page size
	readBytes(fd, buffer, 2);
	uint16_t pageSize = *(uint16_t*) buffer;
	cout << "Enterprise says pagesize is " << pageSize << endl;

	// validate filesize against pagesize
	if (pageSize % fileSize != 0)
	{
		fclose(file);
		cout << "File does not have valid pagesize alignment." << endl;
		return false;
	}

	// write filesize
	write(fd, (char*)&fileSize, sizeof(uint32_t));
	if (!waitForAck(fd))
	{
		fclose(file);
		cout << "Filesize is too large for transfer." << endl;
		return false;
	}
	cout << "Filesize is OK" << endl;

	// upload the file
	char* fileData = (char*) malloc(pageSize * sizeof(char));
	int pageUploaded = 0;
	while (fileSize > 0)
	{
		fread(fileData, sizeof(fileData), 1, file);
		write(fd, fileData, sizeof(fileData));

		if (!waitForAck(fd))
		{
			cout << "Block transfer failed!" << endl;
			break;
		}

		fileSize -= pageSize;

		pageUploaded++;
		printf("Page uploaded: %d\n", pageUploaded);
	}

	free(fileData);

	fclose(file);

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
// uploads the file to the Enterprise
void upload(int baud, const char* port, const char* filename)
{
	int fd = open_port(port);
	if (-1 == fd)
		return;

	configure_port(fd, baud);

	if (processUpload(fd, filename))
		cout << "Successfully uploaded EEPROM image file." << endl;

	close(fd);
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
	if (argc < 3)
	{
		showHelp();
		return 0;
	}

	int baud = 57600;
	char* port = NULL;
	char* file = NULL;
	bool save = false;

	loadSettings(&baud, &port);

	// -b 9600 -p /dev/ttyACM0 -i effects.bin -s
	for (int i = 1; i < argc; i++)
	{
		if (0 == strncmp("-i", argv[i], 2))
			file = argv[i + 1];
		else if (0 == strncmp("-p", argv[i], 2))
			port = argv[i + 1];
		else if (0 == strncmp("-b", argv[i], 2))
			baud = atoi(argv[i + 1]);
		else if (0 == strncmp("-s", argv[i], 2))
			save = true;
	}

	int actualBaud = mapBaud(baud);
	if (actualBaud < 1)
	{
		cout << "The specified baud rate of " << baud << " is not valid." << endl;
		return -1;
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
	cout << "upload -i {filename} -p {serial port} -b {baud rate} -s" << endl;
	cout << "Uploads an EEPROM effects file to the Enterprise." << endl;
	cout << "  -i  The EEPROM image file to upload to the Enterprise." << endl;
	cout << "  -b  Specify the baud rate to use.  Must be a standard value (e.g. 9600, 57600, etc)." << endl;
	cout << "  -p  Specify the serial port to use." << endl;
	cout << "         List serial ports with 'll /dev/tty*' at the command prompt." << endl;
	cout << "  -s  Save the given port and baud rate as the new defaults." << endl;
	cout << "         Note: settings are saved in the .upload-settings file in the current directory." << endl;
	cout << endl;
}