using System.Collections.Generic;
using System.Configuration;
using Configuration = IOLib.Configuration;

namespace EnterpriseIO
{
	public interface ConfigurationWriter
	{
		void Save(Configuration config, IDictionary<string, string> updatedSettings);
	}

	public class ApplicationConfigurationWriter : ConfigurationWriter
	{
		public void Save(Configuration configSettings, IDictionary<string,string> updatedSettings)
		{
			//http://geekswithblogs.net/akraus1/articles/64871.aspx
			var cfg = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);

			cfg.AppSettings.Settings.Add("Port", updatedSettings["port"]);
			cfg.AppSettings.Settings.Add("Baud", updatedSettings["baud"]);
			cfg.AppSettings.Settings.Add("Parity", updatedSettings["parity"]);
			cfg.AppSettings.Settings.Add("DataBits", updatedSettings["databits"]);
			cfg.AppSettings.Settings.Add("StopBits", updatedSettings["stopbits"]);
			cfg.AppSettings.Settings.Add("FlowControl", updatedSettings["flowcontrol"]);

			cfg.Save(ConfigurationSaveMode.Modified);

			ConfigurationManager.RefreshSection("appSettings");
		}
	}
}
