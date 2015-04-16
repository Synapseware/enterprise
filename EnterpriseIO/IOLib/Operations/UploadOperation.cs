using System;
using IOLib.Drivers;

namespace IOLib.Operations
{
	public class UploadOperation : Operation
	{
		private readonly IEEPROMDriver _driver;
		private readonly Configuration _config;

		public UploadOperation(IEEPROMDriver driver, Configuration config)
		{
			_driver = driver;
			_config = config;
		}

		public string Name { get { return "upload"; } }

		public void Execute()
		{
			if (!_driver.UploadData(_config.DataFilename, _config.BaudRate, _config.Port))
				throw new Exception("Upload failed");
		}
	}
}