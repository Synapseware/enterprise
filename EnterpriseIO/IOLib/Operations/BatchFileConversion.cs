using IOLib.Drivers;

namespace IOLib.Operations
{
	public class BatchFileConversion : Operation
	{
		private readonly Configuration _config;
		private readonly StitchFile _stitchFile;
		private readonly IEEPROMDriver _driver;

		public BatchFileConversion(Configuration config, IEEPROMDriver driver, StitchFile stitchFile)
		{
			_config = config;
			_stitchFile = stitchFile;
			_driver = driver;
		}

		public void Execute()
		{
			_stitchFile.Process(_config.DataFilename, _config.SourceFiles, _driver.PageSize);
		}

		public string Name { get { return "batch"; } }
	}
}
