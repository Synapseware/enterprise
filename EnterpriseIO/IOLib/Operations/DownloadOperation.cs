using IOLib.Drivers;

namespace IOLib.Operations
{
	public class DownloadOperation : Operation
	{
		private readonly Configuration _config;
		private readonly int _bytesToDownload;
		private readonly IEEPROMDriver _driver;

		public DownloadOperation(IEEPROMDriver driver, Configuration config, int bytesToDownload)
		{
			_driver = driver;
			_config = config;
			_bytesToDownload = bytesToDownload;
		}

		public string Name { get { return "download"; } }

		public void Execute()
		{
			_driver.DownloadData(_config.DataFilename, _bytesToDownload, _config.BaudRate, _config.Port);
		}
	}
}