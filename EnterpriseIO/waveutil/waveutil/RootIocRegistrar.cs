using Autofac;
using AutofacExtensions;
using IOLib;

namespace waveutil
{
	public class RootIocRegistrar : IocRegistrar
	{
		public void Build(ContainerBuilder builder)
		{
			builder
				.Register(c => new CommandLineUserParameters(Program.UserInput))
				.As<UserParameters>()
				.SingleInstance();

			builder
				.RegisterType<DefaultConfiguration>()
				.As<Configuration>()
				.SingleInstance();
		}
	}
}
