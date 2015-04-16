using System;

namespace IOLib.Operations
{
	/// <summary>
	/// Displays the header bytes for copy to program memory
	/// </summary>
	public class ShowHeaderOperation : Operation
	{
		private readonly FileSystemAbstractions _fsa;
		private readonly Configuration _configuration;

		public ShowHeaderOperation(FileSystemAbstractions fsa, Configuration configuration)
		{
			_fsa = fsa;
			_configuration = configuration;
		}

		public void Execute()
		{
			var dataFileName = _configuration.DataFilename;

			var header = new byte[256];
			using (var fi = _fsa.Open(dataFileName))
			{
				fi.Read(header, 0, header.Length);
				fi.Close();
			}

			var col = 0;
			foreach (var t in header)
			{
				col++;
				Console.Write("0x{0:x2}, ", t);
				if (col % 16 == 0)
					Console.WriteLine();
			}
		}

		public string Name { get { return "header"; } }
	}
}
