using System;
using Autofac;
using Autofac.Core;
using IOLib.Drivers;

namespace IOLib.Operations
{
	public static class OperationParser
	{
		public static Operation Parse(IComponentContext context)
		{
			var parameters = context.Resolve<UserParameters>();

			Func<Operation> parse = () =>
			{
				for (var i = 0; i < parameters.Count; i++)
				{
					switch (parameters[i].ToLower())
					{
					case "/m":
							switch (parameters[i + 1].ToLower())
						{
							case "single":
								// single conversionMode: convert 1 wave file to either bin or hex.
								return context.Resolve<SingleFileConversion>();
							
							case "stitch":
								// stitch conversionMode: combines all .avw files into a final binary data file
								return context.Resolve<StitchFileConversion>();

							case "batch":
								// batch conversion mode: combines all the given wav files into the output file
								return context.Resolve<BatchFileConversion>();
							
							case "upload":
								// upload to eeprom.  get baud rate and block size
								return context.Resolve<UploadOperation>();
							
							case "download":
								// download from eeprom.  get baud rate and block size
								return context.Resolve<DownloadOperation>
								(
									new Parameter[]
									{
										new PositionalParameter(0, context.Resolve<IEEPROMDriver>()),
										new PositionalParameter(1, context.Resolve<Configuration>()),
										new PositionalParameter(2, int.Parse(parameters[i + 2]))
									}
								);

							case "format":
								return context.Resolve<FormatOperation>();
		
							case "motion":
								bool streamData;
								if (parameters.Count > 2)
									streamData = parameters[i + 2].ToLower() == "stream";
								else
									streamData = false;

								return context.Resolve<MotionProcessor>
								(
									new Parameter[]
									{
										new PositionalParameter(0, context.Resolve<Configuration>()),
										new PositionalParameter(1, streamData)
									}
								);

							case "header":
								return context.Resolve<ShowHeaderOperation>();
						}
						break;
					}
				}

				return context.Resolve<NullOperation>();
			};

			return parse();
		}
	}
}
