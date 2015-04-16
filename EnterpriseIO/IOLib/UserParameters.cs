using System.Linq;

namespace IOLib
{
	public interface UserParameters
	{
		string[] Parameters { get; }
		int Count { get; }
		string this[int index] { get; }
	}
}
