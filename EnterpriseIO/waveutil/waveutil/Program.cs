using System;
using System.Reflection;
using AutofacExtensions;
using IOLib.Operations;

namespace waveutil
{
	class Program
	{
		static void Main(string[] args)
		{
			UserInput = args;

			var container = AutofacApplication.Initialize(Assembly.GetExecutingAssembly());

			Log.Write("wavutil v1.00 by Matthew Potter");

			if (args.Length < 1 || String.IsNullOrEmpty(args[0]))
			{
				Log.Write("Missing source file name.");
				return;
			}

			OperationParser
				.Parse(container)
				.Execute();

			Log.Write("Done");
			Console.ReadLine();
		}

		public static string[] UserInput { get; set; }
	}
}
