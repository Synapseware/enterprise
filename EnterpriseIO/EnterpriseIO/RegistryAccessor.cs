using System.Security.AccessControl;
using Microsoft.Win32;

namespace EnterpriseIO
{
	public interface RegistryAccessor
	{
		T GetAppValue<T>(string key);
		void SetAppValue(string key, string value);
		void SetAppValue(string key, int value);
		void SetAppValue(string key, bool value);
	}

	public class DefaultRegistryAccessor : RegistryAccessor
	{
		private const string ROOT = @"SOFTWARE\Synapseware\EnterpriseIO";

		private readonly RegistryKey _key;

		public DefaultRegistryAccessor()
		{
			var lm = Registry.LocalMachine;

			var parts = ROOT.Split('\\');
			var path = parts[0];
			for (var i = 1; i < parts.Length; i++)
			{
				path += "\\" + parts[i];
				if (null == lm.OpenSubKey(path))
					lm.CreateSubKey(path, RegistryKeyPermissionCheck.ReadWriteSubTree);
			}

			_key = lm.OpenSubKey(ROOT, true);
		}

		public T GetAppValue<T>(string key)
		{
			var value = _key.GetValue(key);
			if (null == value)
				return default(T);

			return (T) value;
		}

		public void SetAppValue(string key, string value)
		{
			SetAppValue(key, value, RegistryValueKind.String);
		}
		public void SetAppValue(string key, int value)
		{
			SetAppValue(key, value, RegistryValueKind.DWord);
		}
		public void SetAppValue(string key, bool value)
		{
			SetAppValue(key, value ? "True" : "False", RegistryValueKind.String);
		}
		private void SetAppValue(string key, object value, RegistryValueKind rvk)
		{
			_key.SetValue(key, value, rvk);
		}
	}
}
