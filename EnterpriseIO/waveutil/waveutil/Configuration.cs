using System.Collections.Generic;
using System.Collections.Specialized;
using System.Configuration;
using System.Linq;
using IOLib;

namespace waveutil
{
	public class DefaultConfiguration : IOLib.Configuration
	{
		private readonly NameValueCollection _settings;
		private readonly IList<string> _args;

		private OutputFormat? _outputFormat;
		private string _dataFileName;
		private IList<string> _wavfiles; 

		public DefaultConfiguration()
		{
			_settings = ConfigurationManager.AppSettings;
			_args = Program.UserInput;
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
				if (null == _outputFormat)
				{
					var outputFormat = OutputFormat.Text;

					var list = _args.ToList();

					var formatIdx = list.FindIndex(s => s.ToLower() == "/o");
					if (formatIdx > -1 && formatIdx < list.Count - 2 && list[formatIdx + 1].ToLower() == "binary")
						outputFormat = OutputFormat.Binary;

					_outputFormat = outputFormat;
				}
				return _outputFormat.Value;
			}
		}

		/// <summary>
		/// Returns the .dat file given in the input parameters
		/// </summary>
		public string DataFilename
		{
			get { return _dataFileName ?? (_dataFileName = _args.FirstOrDefault(f => f.ToLower().EndsWith(".dat"))); } // f.ToLower().EndsWith(".wav") || 
		}

		public IList<string> SourceFiles
		{
			get
			{
				return _args.Where(f => f.ToLower().EndsWith(".wav")).ToList();
			}
		}
	}
}
