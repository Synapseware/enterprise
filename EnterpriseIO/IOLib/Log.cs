using System;
using System.Diagnostics;

namespace IOLib
{
	public static class Log
	{
		public delegate void WriteToLog(string fmt, params object[] args);

		public static void Write(string fmt, params object[] args)
		{
			Debug.WriteLine(fmt, args);
			Console.WriteLine(fmt, args);
		}
	}
}
