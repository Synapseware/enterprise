using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using IOLib;
using System.Windows.Forms;

namespace EnterpriseIO
{
	partial class frmMain
	{
		private void btnSelectSingleWave_Click(object sender, EventArgs e)
		{
			OpenWithDefault();
			openWavFileDialog.Multiselect = false;
			if (DialogResult.OK != openWavFileDialog.ShowDialog())
				return;
			txtSourceFile.Text = openWavFileDialog.FileName;
			CloseWithDefault();
		}

		private void btnConvertSingle_Click(object sender, EventArgs e)
		{
			MessageBox.Show("Single file conversion should be deprecated, or always forced to convert to HEX output");

			var response = WaveUtilLauncher.SingleFileConversion(txtSingleOutputFilename.Text, txtSourceFile.Text);

			MessageBox.Show(response, "Conversion Response", MessageBoxButtons.OK);
		}

		private void btnSelectWaveFolder_Click(object sender, EventArgs e)
		{
			OpenWithDefault();
			openWavFileDialog.Multiselect = true;
			if (DialogResult.OK != openWavFileDialog.ShowDialog())
				return;

			openWavFileDialog.FileNames
				.ToList()
				.ForEach(f =>
				{
					var row = new DataGridViewRow();
					row.Cells.Add(new DataGridViewCheckBoxCell { Value = true });
					row.Cells.Add(new DataGridViewTextBoxCell { Value = f });

					grdWaveFiles.Rows.Add(row);
				});
			CloseWithDefault();
		}

		private void btnBatchConvert_Click(object sender, EventArgs e)
		{
			var files = new List<string>();
			foreach (DataGridViewRow row in grdWaveFiles.Rows)
			{
				if (true != (bool)(row.Cells[0].Value ?? false))
					continue;
				var filename = (string) row.Cells[1].Value ?? "";
				if (!File.Exists(filename))
					continue;

				files.Add(filename);
			}

			var response = WaveUtilLauncher.BatchFileConversion(txtBatchOutputFilename.Text, files);

			MessageBox.Show(response, "Conversion Response", MessageBoxButtons.OK);
		}

		private void btnClear_Click(object sender, EventArgs e)
		{
			grdWaveFiles.Rows.Clear();
			grdWaveFiles.Update();
		}

		private void grdWaveFiles_DragDrop(object sender, DragEventArgs e)
		{
			MessageBox.Show("Received drop event!");
		}

		private void OpenWithDefault()
		{
			var lastPath = _registryAccessor.GetAppValue<string>("LastSourceFilePath");
			if (!String.IsNullOrEmpty(lastPath))
				openWavFileDialog.InitialDirectory = lastPath;
		}
		private void CloseWithDefault()
		{
			_registryAccessor.SetAppValue("LastSourceFilePath", Path.GetDirectoryName(openWavFileDialog.FileNames[0]));
		}
	}
}
