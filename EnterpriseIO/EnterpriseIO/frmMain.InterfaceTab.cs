using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using IOLib;
using System.Windows.Forms;
using System.Threading;

namespace EnterpriseIO
{
	partial class frmMain
	{
		private void btnDownload_Click(object sender, EventArgs e)
		{
			ClearStatus();
			AppendStatus("Downloading {0} bytes from EEPROM...", txtBytesToSave.Text);

			var response = WaveUtilLauncher.Download
			(
				status => AppendStatus(status),
				saveFileDialog.FileName,
				int.Parse(txtBytesToSave.Text)
			);

			//AppendStatus(response);
		}

		private void txtBytesToSave_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar < '0' || e.KeyChar > '9')
			{
				e.Handled = true;
				return;
			}
		}

		private void btnDownloadBrowse_Click(object sender, EventArgs e)
		{
			if (DialogResult.OK != saveFileDialog.ShowDialog())
				return;

			txtDownloadFile.Text = saveFileDialog.FileName;
		}

		private void ClearStatus()
		{
			Action clear = () => txtStatus.Text = "";
			if (txtStatus.InvokeRequired)
				txtStatus.Invoke(clear);
			else
				clear();
		}

		private void AppendStatus(string msg, params object[] args)
		{
			Action append = () =>
			{
				txtStatus.AppendText(string.Format(msg, args));
				txtStatus.AppendText(Environment.NewLine);
				txtStatus.Refresh();
			};

			if (txtStatus.InvokeRequired)
				txtStatus.Invoke(append);
			else
				append();
		}
	}
}
