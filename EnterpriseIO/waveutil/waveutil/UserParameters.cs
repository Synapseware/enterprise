using System.Linq;
using IOLib;

namespace waveutil
{
	public class CommandLineUserParameters : UserParameters
	{
		private readonly string[] _args;

		public CommandLineUserParameters(string[] args)
		{
			_args = args;
		}

		public string[] Parameters
		{
			get { return _args; }
		}

		public int Count
		{
			get { return _args.Count(); }
		}

		public string this[int index]
		{
			get { return _args[index]; }
		}
	}
}
