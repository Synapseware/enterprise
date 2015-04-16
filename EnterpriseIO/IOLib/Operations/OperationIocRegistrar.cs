using System.Linq;
using System.Reflection;
using Autofac;
using IOLib.Drivers;
using AutofacExtensions;

namespace IOLib.Operations
{
	public class OperationIocRegistrar : IocRegistrar
	{
		public void Build(ContainerBuilder builder)
		{
			builder
				.Register(c => EEPROMDriverFactory.CreateInstance("at24c1024b"))
				.As<IEEPROMDriver>();

			var operations = Assembly
				.GetExecutingAssembly()
				.GetTypes()
				.Where(t => t.IsClass && !t.IsAbstract && t.IsAssignableTo<Operation>())
				.Select(t => t);

			operations
				.ToList()
				.ForEach(o => builder.RegisterType(o));
		}
	}
}
