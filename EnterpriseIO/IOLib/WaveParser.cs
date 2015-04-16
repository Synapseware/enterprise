using System;
using System.IO;
using System.Text;

namespace IOLib
{
	public interface AudioFile
	{
		uint Length { get; }
		uint Rate { get; }
		byte[] Data { get; }
	}
	public class WaveFile : AudioFile
	{
		public WaveFile(uint length, uint rate, byte[] data)
		{
			Length = length;
			Rate = rate;
			Data = data;
		}

		public uint Length { get; private set; }
		public uint Rate { get; private set; }
		public byte[] Data { get; private set; }
	}

	public interface AudioFileParser
	{
		AudioFile Parse(string filePath);
		AudioFile Parse(Stream fileData);
	}

	public class WaveParser : AudioFileParser
	{
		public AudioFile Parse(string filePath)
		{
			using (var fileData = (Stream)File.OpenRead(filePath))
				return Parse(fileData);
		}

		public AudioFile Parse(Stream fileData)
		{
			var b_str = new byte[5];
			var b_short = new byte[2];
			var b_int = new byte[4];

			// WAV header format:
			// https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
			/*
			The canonical WAVE format starts with the RIFF header:

			0         4   ChunkID          Contains the letters "RIFF" in ASCII form
											(0x52494646 big-endian form).
			4         4   ChunkSize        36 + SubChunk2Size, or more precisely:
											4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
											This is the size of the rest of the chunk 
											following this number.  This is the size of the 
											entire file in bytes minus 8 bytes for the
											two fields not included in this count:
											ChunkID and ChunkSize.
			8         4   Format           Contains the letters "WAVE"
											(0x57415645 big-endian form).

			The "WAVE" format consists of two subchunks: "fmt " and "data":
			The "fmt " subchunk describes the sound data's format:

			12        4   Subchunk1ID      Contains the letters "fmt "
											(0x666d7420 big-endian form).
			16        4   Subchunk1Size    16 for PCM.  This is the size of the
											rest of the Subchunk which follows this number.
			20        2   AudioFormat      PCM = 1 (i.e. Linear quantization)
											Values other than 1 indicate some 
											form of compression.
			22        2   NumChannels      Mono = 1, Stereo = 2, etc.
			24        4   SampleRate       8000, 44100, etc.
			28        4   ByteRate         == SampleRate * NumChannels * BitsPerSample/8
			32        2   BlockAlign       == NumChannels * BitsPerSample/8
											The number of bytes for one sample including
											all channels. I wonder what happens when
											this number isn't an integer?
			34        2   BitsPerSample    8 bits = 8, 16 bits = 16, etc.
						2   ExtraParamSize   if PCM, then doesn't exist
						X   ExtraParams      space for extra parameters

			The "data" subchunk contains the size of the data and the actual sound:

			36        4   Subchunk2ID      Contains the letters "data"
											(0x64617461 big-endian form).
			40        4   Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
											This is the number of bytes in the data.
											You can also think of this as the size
											of the read of the subchunk following this 
											number.
			44        *   Data             The actual sound data.
			*/
			// look for RIFF bytes
			fileData.Seek(0, SeekOrigin.Begin);
			fileData.Read(b_str, 0, 4);
			if (Encoding.ASCII.GetString(b_str, 0, 4) != "RIFF")
				throw new Exception("File is not in RIFF format.");

			// look for WAVE bytes
			fileData.Seek(8, SeekOrigin.Begin);
			fileData.Read(b_str, 0, 4);
			if (Encoding.ASCII.GetString(b_str, 0, 4) != "WAVE")
				throw new Exception("File is not in WAVE format.");

			// find "fmt " chunk length
			fileData.Seek(16, SeekOrigin.Begin);
			fileData.Read(b_int, 0, 4);
			var fmtChunkSize = BitConverter.ToUInt32(b_int, 0);
			var fmtChunkEnd = fmtChunkSize + fileData.Position;

			// get compression code
			fileData.Read(b_short, 0, 2);
			var compressionCode = BitConverter.ToUInt16(b_short, 0);
			if (compressionCode != 1)
				throw new Exception("Compression format is invalid.  Can only read uncompressed WAVE data.");

			// get channel count
			fileData.Read(b_short, 0, 2);
			var channelCount = BitConverter.ToUInt16(b_short, 0);
			if (channelCount != 1)
				throw new Exception("Channel count is invalid.  Can only read mono WAVE data.");

			// get rate
			fileData.Read(b_int, 0, 4);
			var sampleRate = BitConverter.ToUInt16(b_int, 0);
			if (sampleRate > 22000)
				throw new Exception("Sample rate is greater than 22kHz.");

			// skip past bytes per second & block align values (int & short)
			fileData.Seek(6, SeekOrigin.Current);

			// read bits per sample
			fileData.Read(b_short, 0, 2);
			var resolution = BitConverter.ToUInt16(b_short, 0);
			if (resolution != 8)
				throw new Exception(String.Format("Can not use {0} bit wave file.  Only 8 bit samples are supported.", resolution));

			// skip to next chunk (current position - chunk length + chunk start)
			//fi.Seek(fi.Position - fmt_len + 12, SeekOrigin.Current);
			fileData.Position = fmtChunkEnd;

			// move to data chunk
			while (true)
			{
				fileData.Read(b_str, 0, 4);
				if (Encoding.ASCII.GetString(b_str, 0, 4) == "data")
					break;

				// read chunk size
				fileData.Read(b_int, 0, 4);
				var len = BitConverter.ToUInt32(b_int, 0);
				fileData.Seek(len, SeekOrigin.Current);
			}

			// read length of sample data
			fileData.Read(b_int, 0, 4);
			var length = BitConverter.ToUInt32(b_int, 0);
			if (length > 16777216)
				throw new Exception("Sound data is greater than 16MB (24bit address).  Can not convert.");

			// read in the sample data
			var buff = new byte[length];
			fileData.Read(buff, 0, buff.Length);

			fileData.Close();

			// create WaveFile object
			return new WaveFile
			(
				length,
				sampleRate,
				buff
			);
		}
	}
}
