using System;

namespace IOLib.Operations
{
	public interface Operation
	{
		void Execute();
		string Name { get; }
	}

	public class NullOperation : Operation
	{
		public void Execute()
		{
			Console.WriteLine(@"
wavutil - Converts WAV files to EEPROM file format.  Supported commands are:

download	[filename] [bytes to download]
	Downloads data from the EEPROM and saves the data in the filename given.

format
	Formats the EEPROM.

header		[filename]
	Displays the header for an EEPROM file.

motion
	Reads X, Y and Z accelerometer data from the device.

single		[filename] [format]
	Converts a single WAV file to an AVR wave file format, or to text.
	format = {*text|binary}

stitch		[filename]
	Combines all the WAV files found in the target file directory and
	creates the output file.

upload		[filename]
	Uploads the specified file to the EEPROM.

");
		}

		public string Name { get { return null; } }
	}
}
