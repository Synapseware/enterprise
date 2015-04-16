using System;
using System.IO;
using System.Text;

namespace IOLib.Operations
{
	public class SingleFileConversion : Operation
	{
		private readonly FileSystemAbstractions _fsa;
		private readonly AudioFileParser _parser;
		private readonly Configuration _config;

		public SingleFileConversion(FileSystemAbstractions fsa, AudioFileParser parser, Configuration config)
		{
			_fsa = fsa;
			_parser = parser;
			_config = config;
		}

		public string Name { get { return "single"; } }

		public void Execute()
		{
			var waveFile = _parser.Parse(_config.DataFilename);

			using (var fi = new MemoryStream(waveFile.Data))
			{
				Log.Write("Processing {0}", _config.DataFilename);
				Log.Write("Wave file properties:");
				Log.Write("\tLength:     {0}", waveFile.Length);
				Log.Write("\tRate:       {0}", waveFile.Rate);
				Log.Write("\tResolution: 8 bit");

				var ext = _config.Format == OutputFormat.Binary
					? ".avw"	// AVR wave file format
					: ".txt";	// text outputFormat - comma delimited list of sample values

				var outputFile = Path.Combine(Path.GetDirectoryName(_config.DataFilename) ?? "", Path.GetFileNameWithoutExtension(_config.DataFilename) + ext);

				// create outputFormat file
				GenerateOutputFile(fi, waveFile.Length, outputFile);

				fi.Close();
			}
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="outputFormat"></param>
		/// <param name="fi"></param>
		/// <param name="length"></param>
		/// <param name="outputFile"></param>
		private void GenerateOutputFile(Stream fi, uint length, string outputFile)
		{
			using (var fo = File.Create(outputFile))
			{
				// write outputFormat header to file
				WriteOutputHeader(OutputFormat.Text, length, fo);

				var pos = 0;
				while (length > 0)
				{
					if (pos % 20 == 0 && pos > 0)
						fo.Write(Encoding.ASCII.GetBytes("\r\n"), 0, 2);
					pos++;

					var sample_int = fi.ReadByte();
					if (sample_int == -1)
						break;

					var sample_value = Convert.ToByte(sample_int);

					var strval = String.Format("0x{0:X2}, ", sample_value);
					var bstrval = Encoding.ASCII.GetBytes(strval);
					fo.Write(bstrval, 0, bstrval.Length);

					length--;
				}
				fo.Flush();
				fo.Close();
			}
		}

		/// <summary>
		/// Generates the sample header info, total of 8 bytes)
		///	0x02	Sample length (4 bytes)
		/// </summary>
		/// <param name="outputFormat"></param>
		/// <param name="length"></param>
		/// <param name="fo"></param>
		private void WriteOutputHeader(OutputFormat outputFormat, uint length, Stream fo)
		{
			// for binary, dump the important sample details to the file
			if (outputFormat != OutputFormat.Binary)
				return;

			// write length to sample
			var buff = BitConverter.GetBytes(length);
			fo.Write(buff, 0, 4);
		}
	}
}
