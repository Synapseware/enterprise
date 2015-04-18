using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace IOLib
{
	public interface StitchFile
	{
		void Process(string outputFile, ushort pageSize);
		void Process(string outputFile, IList<string> sourceFiles, ushort pageSize);
	}

	/// <summary>
	/// Stitches all the .wav files in the directory of the data filename and outputs them
	/// to a "stitch.bin" file.
	/// 
	/// Stitch File Format
	/// Byte	Offset	Size	Description
	///		0			2		Number of samples
	///		1			0		
	///		0	0x02	2		Start Page
	///		1
	///		2	0x06	4		Length
	///		3
	///		4
	///		5
	/// 
	///		
	/// Header Size: 6 bytes
	///		256/6 = 42.667
	///		42 * 6 = 252
	///		252 + 2 = 254
	///		2 bytes free
	///		42 total entries possible
	/// </summary>
	public class WaveStitchFile : StitchFile
	{
		private readonly FileSystemAbstractions _fileSystemAbstractions;
		private readonly AudioFileParser _parser;

		public WaveStitchFile(FileSystemAbstractions fileSystemAbstractions, AudioFileParser parser)
		{
			_fileSystemAbstractions = fileSystemAbstractions;
			_parser = parser;
		}

		/// <summary>
		/// </summary>
		/// <param name="outputFile"></param>
		///<param name="pageSize"> </param>
		public void Process(string outputFile, ushort pageSize)
		{
			var sourcePath = Path.GetDirectoryName(outputFile) ?? "";

			var fileNames = _fileSystemAbstractions.GetFiles(sourcePath, "*.wav");
			if (0 == fileNames.Length)
			{
				Log.Write("No .wav files found.");
				return;
			}

			if (fileNames.Length > 255)
			{
				Log.Write("More than 255 files were found.  Can not generate stitch file.");
				return;
			}

			Process(outputFile, fileNames, pageSize);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="sourceFiles"></param>
		/// <param name="outputFile"></param>
		/// <param name="pageSize"></param>
		public void Process(string outputFile, IList<string> sourceFiles, ushort pageSize)
		{
			var samples =
			(
				from fileName
				in sourceFiles
				let waveFile = _parser.Parse(fileName)
				select SampleEntry.ConvertFrom(fileName, waveFile, pageSize)
			).ToList();

			Log.Write("Creating '{0}' with {1} entries", Path.GetFileName(outputFile), sourceFiles.Count());

			// generate the output file
			var totalLength = samples.Sum(s => s.Data.Length) + 4096;
			using (var fo = _fileSystemAbstractions.Create(outputFile, totalLength, FileOptions.RandomAccess))
			{
				// write the sample count to the header
				var bNum = BitConverter.GetBytes((ushort)sourceFiles.Count());
				fo.Write(bNum, 0, 2);
				var headerPos = fo.Position;

				var sampleIndex = 0;

				// current position in data block
				var dataPos = 256;

				foreach (var sample in samples)
				{
					// reset to the header position
					fo.Seek(headerPos, SeekOrigin.Begin);

					// file details
					Log.Write("-----------------------------------------------------------------");
					Log.Write("{0}) {1}", sampleIndex++, Path.GetFileName(sample.Filename));

					// write the start page
					var startPage = (dataPos / pageSize);
					bNum = BitConverter.GetBytes((ushort)startPage);
					fo.Write(bNum, 0, 2);
					Log.Write("0x{0:X4}     Start Page:      0x{1:X4} ({1})", fo.Position, startPage);

					// write the length
					var sampleLen = sample.Data.Length;
					bNum = BitConverter.GetBytes(sampleLen);
					fo.Write(bNum, 0, 4);
					Log.Write("0x{0:X4}     Length:          0x{1:X4} ({1})", fo.Position, sampleLen);

					// save the header position
					headerPos = fo.Position;

					// seek to next data output block
					fo.Seek(dataPos, SeekOrigin.Begin);
					Log.Write("0x{0:X8} Data Start:      0x{1:X8} ({1})", fo.Position, dataPos);

					// write the sample data to the stitch file
					fo.Write(sample.Data, 0, sample.Data.Length);
					Log.Write("0x{0:X8} Sample Length:   0x{1:X8} ({1})", fo.Position, sample.Data.Length);

					// fill the page with 0's
					while (fo.Position % pageSize != 0)
						fo.WriteByte(0);

					// save the current data position
					dataPos = (int)fo.Position;

					fo.Flush();
				}
				var length = fo.Length;
				fo.Flush();
				fo.Close();
				Log.Write("-------------------------------------------");
				Log.Write("Bytes Used: {0}", length);
				Log.Write("Pages Used: {0}", length / pageSize);
				Log.Write("Press <enter> to continue...");
			}
		}
	}
}
