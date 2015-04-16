using System;
using System.IO;
using System.Windows.Forms;

namespace EnterpriseIO
{
	partial class frmMain
	{
		private void SetActiveTimer()
		{
			timerSingleConvert.Enabled = tabFunctions.SelectedTab.Name == "tabWaveConv";
			timerInterface.Enabled = tabFunctions.SelectedTab.Name == "tabInterface";
		}

		private void tabFunctions_SelectedIndexChanged(object sender, EventArgs e)
		{
			SetActiveTimer();
		}

		private void timerInterface_Tick(object sender, EventArgs e)
		{
			Func<bool> isDownloadValid = () =>
			{
				if (txtBytesToSave.Text.Length == 0 || int.Parse(txtBytesToSave.Text) % 256 != 0)
					return false;
				if (txtDownloadFile.Text.Length == 0)
					return false;
				return true;
			};

			btnDownload.Enabled = isDownloadValid();
		}

		private void timerSingleConvert_Tick(object sender, EventArgs e)
		{
			btnConvertSingle.Enabled = IsSingleConversionValid();

			btnBatchConvert.Enabled = IsBatchConversionValid();
		}

		private bool IsSingleConversionValid()
		{
			if (String.IsNullOrEmpty(txtSingleOutputFilename.Text))
				return false;

			if (!File.Exists(txtSourceFile.Text))
				return false;

			return true;
		}

		private bool IsBatchConversionValid()
		{
			if (String.IsNullOrEmpty(txtBatchOutputFilename.Text))
				return false;

			if (grdWaveFiles.Rows.Count < 1)
				return false;

			foreach (DataGridViewRow row in grdWaveFiles.Rows)
			{
				var filename = (string)row.Cells[1].Value ?? "";
				if (filename != "" && !File.Exists(filename))
					return false;
			}

			return true;
		}
	}
}
