using System;
using System.Collections.Generic;
using System.Reflection;
using System.Windows.Forms;
using Autofac;
using AutofacExtensions;
using IOLib;

namespace EnterpriseIO
{
    public partial class frmMain : Form
    {
	    private readonly IContainer _container;
	    protected RegistryAccessor _registryAccessor;

        public frmMain()
        {
            InitializeComponent();
			SetActiveTimer();

			_container = AutofacApplication.Initialize
			(
				Assembly.GetExecutingAssembly(),
				typeof(Configuration).Assembly
			);

	        _registryAccessor = _container.Resolve<RegistryAccessor>();
        }

		private void btnSaveSettings_Click(object sender, EventArgs e)
		{
			var writer = _container.Resolve<ConfigurationWriter>();

			writer.Save(_container.Resolve<Configuration>(), new Dictionary<string,string>
			{
				{"port", comboPort.Text},
				{"baud", comboBPS.Text},
				{"parity", comboParity.Text},
				{"databits", comboDataBits.Text},
				{"flowcontrol", comboFlowControl.Text},
				{"stopbits", comboStopBits.Text}
			});
		}
    }
}
