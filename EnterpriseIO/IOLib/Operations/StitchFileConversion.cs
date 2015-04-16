using IOLib.Drivers;

namespace IOLib.Operations
{
	public class StitchFileConversion : Operation
	{
		private readonly Configuration _config;
		private readonly StitchFile _stitchFile;
		private readonly IEEPROMDriver _driver;

		public StitchFileConversion(Configuration config, IEEPROMDriver driver, StitchFile stitchFile)
		{
			_config = config;
			_stitchFile = stitchFile;
			_driver = driver;
		}

		public string Name { get { return "stitch"; } }

		public void Execute()
		{
			_stitchFile.Process(_config.DataFilename, _driver.PageSize);
		}
	}
}
