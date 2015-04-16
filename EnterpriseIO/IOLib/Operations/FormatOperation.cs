using System;
using IOLib.Drivers;

namespace IOLib.Operations
{
	public class FormatOperation : Operation
	{
		private readonly IEEPROMDriver _driver;
		private readonly Configuration _config;

		public FormatOperation(IEEPROMDriver driver, Configuration config)
		{
			_driver = driver;
			_config = config;
		}

		public string Name { get { return "format"; } }

		public void Execute()
		{
			if (!_driver.Format(_config.BaudRate, _config.Port))
				throw new Exception("Format failed");
		}
	}
}