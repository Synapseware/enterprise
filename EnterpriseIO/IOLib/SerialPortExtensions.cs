using System;
using System.IO.Ports;
using System.Threading;

namespace IOLib
{
	public static class SerialPortExtensions
	{
		public static void ReadWithBlock(this SerialPort port, byte[] buff, int offset, int count)
		{
			while (count != port.BytesToRead) { }

			port.Read(buff, offset, count);
		}

		public static byte ReadByteWithBlock(this SerialPort port)
		{
			while (1 != port.BytesToRead) { }

			var data = Convert.ToByte(port.ReadByte());

			return data;
		}

		public static void WriteWithBlock(this SerialPort port, byte[] buff, int offset, int count)
		{
			for (var i = offset; i < offset + count; i++)
			{
				// wait for write buffer to empty
				while (0 != port.BytesToWrite) { }

				port.Write(buff, i, 1);
			}
		}

		public static bool WaitForBytes(this SerialPort port, int milliseconds)
		{
			var reset = new ManualResetEvent(false);
			port.DataReceived += (sender, e) => reset.Set();

			return reset.WaitOne(milliseconds);
		}

		public static void EmptyReadBuffer(this SerialPort port)
		{
			while (port.BytesToRead > 0)
				port.ReadByte();
		}

		public static bool WaitForData(this SerialPort port)
		{
			var reset = new ManualResetEvent(false);
			port.DataReceived += (sender, e) => reset.Set();
			var delay = 5;
			while (delay > 0)
			{
				if (0 != port.BytesToRead)
					return true;

				reset.WaitOne(100);
				delay--;
			}

			return false;
		}
	}
}
