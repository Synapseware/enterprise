using System;

namespace IOLib
{
	public class SampleEntry
	{
		private SampleEntry()
		{ }

		public static SampleEntry ConvertFrom(string fileName, AudioFile source, ushort pageSize)
		{
			var entry = new SampleEntry
			{
				Rate = Convert.ToUInt16(source.Rate),
				Pages = Convert.ToUInt16(source.Length / pageSize),
				LastPageSize = Convert.ToByte(source.Length % pageSize),
				Filename = fileName
			};

			// set last page to include partial data
			if (entry.LastPageSize != 0)
				entry.Pages++;

			// copy wave file data
			entry.Data = new byte[source.Data.Length];
			source.Data.CopyTo(entry.Data, 0);

			return entry;
		}

		public ushort Rate { get; private set; }
		public ushort Pages { get; private set; }
		public byte LastPageSize { get; private set; }
		public byte[] Data { get; private set; }
		public string Filename { get; private set; }
	}
}
