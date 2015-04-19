#include "upload.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
int open_port(const char* port)
{
	int fd; // file description for the serial port
	
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	
	if (fd == -1) // if open is unsucessful
	{
		cout << "open_port: Unable to open " << port << ".\n";
		return -1;
	}

	fcntl(fd, F_SETFL, 0);
	cout << "port is open.\n";

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
		file << baud << "\n";
		file << port << "\n";
		file.flush();
		file.close();
	}
	else
		cout << "Failed to create settings file\n";
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

	cout << "Loaded settings:\n";
	cout << "  Baud: " << *baud << "\n";
	cout << "  Port: " << *port << "\n";

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

	// begin upload processing
	// Hello = 'A'
	// StoreFile = 'W'
	// RetrieveFile = 'R'
	// Format = 'F'
	write(fd, "A", 1);
	

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

	cout << "Invalid baud rate: " << baud << "\n";

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
		if (0 == strncmp("-b", argv[i], 2))
			baud = atoi(argv[i + 1]);
		else if (0 == strncmp("-p", argv[i], 2))
			port = argv[i + 1];
		else if (0 == strncmp("-i", argv[i], 2))
			file = argv[i + 1];
		else if (0 == strncmp("-s", argv[i], 2))
			save = true;
	}

	cout << "Uploading " << file << " to the Enterprise on port " << port << " at " << baud << " baud.\n";

	int actualBaud = mapBaud(baud);
	if (actualBaud < 1)
		return -1;

	if (save)
		saveSettings(baud, port);

	upload(actualBaud, port, file);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - -
void showHelp(void)
{
	cout << "upload\n";
	cout << "Uploads an effects file to the Enterprise!\n";
}