namespace IOLib
{
	public static class Commands
	{
		public const byte Hello = (byte) 'A'; // we use A here to let client know we are in Auto mode, so don't give us prompts!
		public const byte StoreFile = (byte) 'W';
		public const byte RetrieveFile = (byte) 'R';
		public const byte Format = (byte) 'F';
	}

	public enum ConversionModes
	{
		Single,
		Stitch,
		Upload,
		Download,
		Format,
		Motion,
		ShowHeader,
		ReadVoltage,
		ReadSOC,
		ReadSOCAuto
	}

	public enum OutputFormat
	{
		Text,
		Binary
	}
}
