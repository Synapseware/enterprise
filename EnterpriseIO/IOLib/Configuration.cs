using System.Collections.Generic;

namespace IOLib
{
	public interface Configuration
	{
		string Port { get; }
		int BaudRate { get; }
		OutputFormat Format { get; }
		string DataFilename { get; }
		IList<string> SourceFiles { get; }
	}
}
