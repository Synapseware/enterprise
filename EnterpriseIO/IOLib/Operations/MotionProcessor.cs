using System;
using System.IO.Ports;
using System.Threading;

namespace IOLib.Operations
{
	public class MotionProcessor : Operation
	{
		private readonly bool _streamData;
		private readonly Configuration _config;

		public MotionProcessor(Configuration config, bool streamData)
		{
			_config = config;
			_streamData = streamData;
		}

		public string Name { get { return "motion"; } }

		public void Execute()
		{
			var rot = new[] { '|', '/', '-', '\\' };
			var rotpos = 0;
			var buff = new byte[4];
			using (var port = new SerialPort(_config.Port, _config.BaudRate, Parity.None, 8, StopBits.One))
			{
				Log.Write("Reading motion data...");
				port.Open();

				if (_streamData)
					Log.Write("X\tY\tZ");

				var top = Console.CursorTop;
				while (true)
				{
					buff[0] = Commands.Hello;
					port.WriteWithBlock(buff, 0, 1);

					port.ReadWithBlock(buff, 1, 3);

					if (_streamData)
					{
						Log.Write("{0:D3}\t{1:D3}\t{2:D3}", buff[1], buff[2], buff[3]);
					}
					else
					{
						Console.SetCursorPosition(0, top);
						Log.Write("Axis  {0}", rot[rotpos++]);
						Log.Write("\tx: {0:D3}", buff[1]);
						Log.Write("\ty: {0:D3}", buff[2]);
						Log.Write("\tz: {0:D3}", buff[3]);
					}
					rotpos = rotpos % rot.Length;

					if (Console.KeyAvailable)
					{
						if (ConsoleKey.Escape == Console.ReadKey().Key)
							break;
					}

					Thread.Sleep(100);
				}
			}
		}
	}
}