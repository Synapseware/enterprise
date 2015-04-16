using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IOLib
{
	public static class StringExtensions
	{
		public static string Quote(this string value)
		{
			if (!value.Contains(' '))
				return value;

			return "\"" + value + "\"";
		}
	}
}
