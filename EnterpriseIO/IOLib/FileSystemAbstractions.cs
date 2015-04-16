using System.IO;

namespace IOLib
{
	public interface FileSystemAbstractions
	{
		string[] GetFiles(string path, string searchPattern = null, bool recursive = false);

		Stream Open(string path);

		Stream Create(string path, int bufferSize = -1, FileOptions options = FileOptions.None);
	}

	public class RealFileSystemAbstractions : FileSystemAbstractions
	{
		public string[] GetFiles(string path, string searchPattern = null, bool recursive = false)
		{
			if (null == searchPattern)
				return Directory.GetFiles(path);

			return recursive
				       ? Directory.GetFiles(path, searchPattern, SearchOption.AllDirectories)
				       : Directory.GetFiles(path, searchPattern, SearchOption.TopDirectoryOnly);
		}

		public Stream Open(string path)
		{
			return File.Open(path, FileMode.Open);
		}

		public Stream Create(string path, int bufferSize = -1, FileOptions options = FileOptions.None)
		{
			if (bufferSize < 4096)
				bufferSize = 4096;

			return File.Create(path, bufferSize, options);
		}
	}
}
