using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;

namespace IOLib
{
	public static class WaveUtilLauncher
	{
		private const string EXE = "waveutil.exe";

		private static string GetExePath()
		{
			return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + "\\" + EXE;
		}

		private static ProcessStartInfo GetStartInfo(Func<string> args)
		{
			var startInfo = new ProcessStartInfo();
			startInfo.Arguments = args();

			startInfo.FileName = GetExePath();
			startInfo.RedirectStandardOutput = true;
			startInfo.UseShellExecute = false;
			startInfo.WindowStyle = ProcessWindowStyle.Hidden;

			return startInfo;
		}

		private static string LaunchAndWait(Func<string> args, Action<string> status = null)
		{
			var startInfo = GetStartInfo(args);

			try
			{
				using (var proc = new Process())
				{
					proc.StartInfo = startInfo;
					proc.OutputDataReceived += (sndr, e) =>
					{
						if (null == status)
							return;
						status(e.Data);
					};

					proc.Start();
					proc.WaitForExit();

					return proc.StandardOutput.ReadToEnd();
				}
			}
			catch (Exception ex)
			{
				var msg = "An error was encountered: " + ex.Message;
				if (null != status)
					status(msg);
				return msg;
			}
		}

		public static string SingleFileConversion(string outputFile, string sourceFile)
		{
			return LaunchAndWait(() => String.Format("{0} /m single {1}",
				sourceFile.Quote()
			));
		}

		public static string Download(Action<string> status, string fileName, int bytesToDownload)
		{
			return LaunchAndWait
			(
				() => String.Format("{0} /m download {1}",
					fileName.Quote(),
					bytesToDownload
				),
				status
			);
		}

		public static string BatchFileConversion(string outputFile, IEnumerable<string> files)
		{
			return LaunchAndWait
			(
				() => String.Format("/m batch {0} {1}",
					outputFile,
					String.Join(" ", files.Select(f => String.Format("\"{0}\"", f)).ToArray()))
			);
		}
	}
}
