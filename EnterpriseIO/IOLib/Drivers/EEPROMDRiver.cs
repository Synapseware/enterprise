using System;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace IOLib.Drivers
{
	public class EEPROMDriverException : Exception
	{
		public EEPROMDriverException()
		{}

		public EEPROMDriverException(string message)
			: base(message)
		{}

		public EEPROMDriverException(string message, Exception innerException)
			: base(message, innerException)
		{}
	}

	public interface IEEPROMDriver
	{
		ushort PageSize { get; }
		bool Format(int baudRate, string portName);
		bool DownloadData(string dataFileName, int bytesToDownload, int baudRate, string portName);
		bool UploadData(string dataFileName, int baudRate, string portName);
	}

	public static class EEPROMDriverFactory
	{
		public static IEEPROMDriver CreateInstance(string type)
		{
			return new AT24C1024BEEPROMDriver(Log.Write);
		}
	}

	public class AT24C1024BEEPROMDriver : IEEPROMDriver
	{
		private readonly Log.WriteToLog _log;

		public ushort PageSize
		{ get { return 256; } }

		public AT24C1024BEEPROMDriver(Log.WriteToLog log)
		{
			_log = log;
		}

		/// <summary>
		/// Formats the chip
		/// </summary>
		/// <param name="baudRate"></param>
		/// <param name="portName"></param>
		public bool Format(int baudRate, string portName)
		{
			/*
			var buff = new byte[8];

			using (var port = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One))
			{
				_log("Formatting chip...");
				port.Open();

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// send greeting to EEPROM writer/reader
				buff[0] = Commands.Hello;
				port.WriteWithBlock(buff, 0, 1);
				if (port.ReadByteWithBlock() != Responses.AckOK)
					throw new EEPROMDriverException("Chip not ready.");

				_log("Chip is online!");

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// send transfer mode byte 'r' (r for retreive)
				buff[0] = Commands.TransferMode;
				buff[1] = (byte)'f';
				port.WriteWithBlock(buff, 0, 2);
				if (port.ReadByteWithBlock() != 0x55)
					throw new EEPROMDriverException("Chip did not accept transfer mode 'r'");

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				_log("Formatting...");

				var top = Console.CursorTop;
				while (true)
				{
					// read current page address
					port.ReadWithBlock(buff, 0, 2);
					var address = BitConverter.ToUInt16(buff, 0);
					if (buff[0] == 0xFF && buff[1] == 0xFF)
						break;

					Console.SetCursorPosition(0, top);
					_log("\tBlock 0x{0:X8}", address * 256);
				}

				var complete = port.ReadByteWithBlock();
				_log(complete == Responses.Complete ? "Complete" : "Error during format");
			}
			*/
			return true;
		}

		/// <summary>
		/// Downloads the specified number of bytes from the chip
		/// </summary>
		/// <param name="dataFileName"></param>
		/// <param name="bytesToDownload"></param>
		/// <param name="baudRate"></param>
		/// <param name="portName"></param>
		public bool DownloadData(string dataFileName, int bytesToDownload, int baudRate, string portName)
		{
			var buff = new byte[16];

			using (var port = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One))
			{
				port.ReadTimeout = 1000 * 5;	// read timeout is 5 seconds
				port.WriteTimeout = 1000 * 30;	// write timeout is 30 seconds
				port.ReadBufferSize = 1024 * 512;

				try
				{
					_log("Downloading chip data...");
					port.Open();

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// send greeting to EEPROM board
					buff[0] = Commands.Hello;
					port.WriteWithBlock(buff, 0, 1);
					if (!port.WaitForData())
						throw new EEPROMDriverException("Chip not ready.");
					if (0xFF != port.ReadByteWithBlock())
						throw new EEPROMDriverException("Client does not support auto transfer mode.");

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// send transfer mode byte 'R' (r for retrieve)
					buff[0] = Commands.RetrieveFile;
					port.WriteWithBlock(buff, 0, 1);

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// wait for EEPROM to send ACK
					if (0xFF != port.ReadByteWithBlock())
						throw new EEPROMDriverException("Host can not initiate transfer.");

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// send supported block size (32 * 8 = 256 bytes)
					const int blockSize = 32;
					port.WriteWithBlock(new byte[]{32}, 0, 1);

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// get transfer size from host
					port.ReadWithBlock(buff, 0, 4);
					var transferSize = BitConverter.ToUInt32(buff, 0);
					_log("Host will transfer {0} bytes", transferSize);

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// send ACK (we support pretty huge files...)
					buff[0] = 0xFF;
					port.WriteWithBlock(buff, 0, 1);

					// begin download transfer
					_log("Transfer starting...");
					var row = Console.CursorTop;
					const int pageSize = blockSize * 8;
					var block = 0;
					using (var fo = File.Open(dataFileName, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite))
					{
						while (fo.Length < transferSize)
						{
							// wait for a page of data
							while (port.BytesToRead < pageSize)
								Thread.Sleep(5);

							var data = new byte[pageSize];
							port.Read(data, 0, data.Length);
							fo.Write(data, 0, data.Length);
							fo.Flush();

							Console.SetCursorPosition(0, row);
							Console.WriteLine("Block: {0}", ++block);

							// send ACK to host
							if (fo.Length >= bytesToDownload)
							{
								port.Write(new byte[]{0xde}, 0, 1);
								break;
							}

							port.WriteWithBlock(new byte[]{0xff}, 0, 1);
						}

						fo.Flush();
						fo.Close();
					}
				}
				catch(EEPROMDriverException de)
				{
					_log(de.Message);
					return false;
				}
				catch (Exception ex)
				{
					// catch comm exceptions
					_log("Exception: {0}", ex.Message);
				}
			}

			return true;
		}

		/// <summary>
		/// Generates a composite file consisting of all the sample data and sample header information.
		/// </summary>
		/// <param name="dataFileName"></param>
		/// <param name="baudRate"></param>
		/// <param name="portName"></param>
		public bool UploadData(string dataFileName, int baudRate, string portName)
		{
			var buff = new byte[16];

			// read contents of file
			byte[] fileData;
			using (var fi = File.Open(dataFileName, FileMode.Open, FileAccess.Read, FileShare.Read))
			{
				fileData = new byte[fi.Length];
				fi.Read(fileData, 0, fileData.Length);
				fi.Close();
			}

			using (var port = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One))
			{
				port.ReadTimeout = 1000 * 5;	// read timeout is 5 seconds
				port.WriteTimeout = 1000 * 30;	// write timeout is 30 seconds

				try
				{
					_log("Upload data to EEPROM...");
					port.Open();

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// send greeting to EEPROM board
					buff[0] = Commands.Hello;
					port.WriteWithBlock(buff, 0, 1);
					if (!port.WaitForData())
						throw new EEPROMDriverException("Client not ready.");
					if (0xFF != port.ReadByteWithBlock())
						throw new EEPROMDriverException("Client does not support auto mode.");
					_log("Client is in auto mode.");

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// send transfer mode byte 'W' (r for write/store)
					buff[0] = Commands.StoreFile;
					port.WriteWithBlock(buff, 0, 1);

					// wait for client to prepare
					Thread.Sleep(200);

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// let client know we are ready for transfer
					buff[0] = 0xFF;
					port.WriteWithBlock(buff, 0, 1);

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// get transfer block size
					var baseBlockSize = port.ReadByteWithBlock();
					var blockSize = baseBlockSize * 8;
					_log("Block size is: {0} bytes", blockSize);

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// let client know how large the file transfer is
					var fileSize = BitConverter.GetBytes(fileData.Length);
					port.WriteWithBlock(fileSize, 0, fileSize.Length);

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					// client will respond with ACK or NACK.  If NACK, abort
					if (port.ReadByteWithBlock() != 0xFF)
						throw new EEPROMDriverException("Client did not accept transfer because of file size.");

					// begin block transfer
					var row = Console.CursorTop;
					var readPosition = 0;
					while (readPosition < fileData.Length)
					{
						port.WriteWithBlock(fileData, readPosition, blockSize);

						readPosition += blockSize;

						Console.SetCursorPosition(0, row);
						Console.WriteLine("Block: {0}", readPosition / blockSize);

						// wait for ACK
						if (port.ReadByteWithBlock() != 0xFF)
							throw new EEPROMDriverException("Client aborted transfer.");
					}

					return true;
				}
				catch (EEPROMDriverException de)
				{
					_log(de.Message);
					return false;
				}
				catch (TimeoutException)
				{
					_log("Timeout during I/O transfer.  Aborting.");
					return false;
				}
				catch (Exception ex)
				{
					// catch comm exceptions
					_log("Exception: {0}", ex.Message);
					return false;
				}
			}
		}
	}
}
