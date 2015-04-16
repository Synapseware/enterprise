using System.Collections.Generic;
using System.Collections.Specialized;
using System.Configuration;
using System.Linq;
using IOLib;

namespace EnterpriseIO
{
	public class WinFormConfiguration : IOLib.Configuration
	{
		private readonly NameValueCollection _settings;
		private readonly IList<string> _args;

		public WinFormConfiguration()
		{
			_settings = ConfigurationManager.AppSettings;
			_args = null;
		}

		public string Port
		{
			get { return _settings["Port"]; }
		}

		public int BaudRate
		{
			get { return int.Parse(_settings["Baud"]); }
		}

		public OutputFormat Format
		{
			get
			{
				return OutputFormat.Binary;
			}
		}

		public string DataFilename
		{
			get { return _args.FirstOrDefault(f => f.ToLower().EndsWith(".wav") || f.ToLower().EndsWith(".dat")); }
		}

		public IList<string> SourceFiles { get; private set; }
	}
}
