using Autofac;
using AutofacExtensions;

namespace EnterpriseIO
{
	public class ApplicationIocRegistrar : IocRegistrar
	{
		public void Build(ContainerBuilder builder)
		{
			builder
				.RegisterType<DefaultRegistryAccessor>()
				.As<RegistryAccessor>()
				.SingleInstance();
		}
	}
}
