namespace EnterpriseIO
{
    partial class frmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			this.tabFunctions = new System.Windows.Forms.TabControl();
			this.tabInterface = new System.Windows.Forms.TabPage();
			this.groupBox5 = new System.Windows.Forms.GroupBox();
			this.txtStatus = new System.Windows.Forms.TextBox();
			this.groupBox4 = new System.Windows.Forms.GroupBox();
			this.btnDownloadBrowse = new System.Windows.Forms.Button();
			this.txtBytesToSave = new System.Windows.Forms.TextBox();
			this.label11 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.txtDownloadFile = new System.Windows.Forms.TextBox();
			this.btnDownload = new System.Windows.Forms.Button();
			this.tabWaveConv = new System.Windows.Forms.TabPage();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.txtBatchOutputFilename = new System.Windows.Forms.TextBox();
			this.label12 = new System.Windows.Forms.Label();
			this.btnClear = new System.Windows.Forms.Button();
			this.grdWaveFiles = new System.Windows.Forms.DataGridView();
			this.chkInclude = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.txtFilename = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.btnBatchConvert = new System.Windows.Forms.Button();
			this.btnSelectWaveFolder = new System.Windows.Forms.Button();
			this.label8 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label9 = new System.Windows.Forms.Label();
			this.txtSingleOutputFilename = new System.Windows.Forms.TextBox();
			this.btnSelectSingleWave = new System.Windows.Forms.Button();
			this.btnConvertSingle = new System.Windows.Forms.Button();
			this.txtSourceFile = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.tabSettings = new System.Windows.Forms.TabPage();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.btnSaveSettings = new System.Windows.Forms.Button();
			this.label6 = new System.Windows.Forms.Label();
			this.comboPort = new System.Windows.Forms.ComboBox();
			this.label5 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.comboFlowControl = new System.Windows.Forms.ComboBox();
			this.comboStopBits = new System.Windows.Forms.ComboBox();
			this.comboParity = new System.Windows.Forms.ComboBox();
			this.comboDataBits = new System.Windows.Forms.ComboBox();
			this.label1 = new System.Windows.Forms.Label();
			this.comboBPS = new System.Windows.Forms.ComboBox();
			this.openWavFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.dlgWaveFolder = new System.Windows.Forms.FolderBrowserDialog();
			this.timerSingleConvert = new System.Windows.Forms.Timer(this.components);
			this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
			this.timerInterface = new System.Windows.Forms.Timer(this.components);
			this.backgroundWorker = new System.ComponentModel.BackgroundWorker();
			this.tabFunctions.SuspendLayout();
			this.tabInterface.SuspendLayout();
			this.groupBox5.SuspendLayout();
			this.groupBox4.SuspendLayout();
			this.tabWaveConv.SuspendLayout();
			this.groupBox3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.grdWaveFiles)).BeginInit();
			this.groupBox2.SuspendLayout();
			this.tabSettings.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// tabFunctions
			// 
			this.tabFunctions.Controls.Add(this.tabInterface);
			this.tabFunctions.Controls.Add(this.tabWaveConv);
			this.tabFunctions.Controls.Add(this.tabSettings);
			this.tabFunctions.Location = new System.Drawing.Point(12, 12);
			this.tabFunctions.Name = "tabFunctions";
			this.tabFunctions.SelectedIndex = 0;
			this.tabFunctions.Size = new System.Drawing.Size(690, 501);
			this.tabFunctions.TabIndex = 0;
			this.tabFunctions.Tag = "";
			this.tabFunctions.SelectedIndexChanged += new System.EventHandler(this.tabFunctions_SelectedIndexChanged);
			// 
			// tabInterface
			// 
			this.tabInterface.Controls.Add(this.groupBox4);
			this.tabInterface.Location = new System.Drawing.Point(4, 22);
			this.tabInterface.Name = "tabInterface";
			this.tabInterface.Padding = new System.Windows.Forms.Padding(3);
			this.tabInterface.Size = new System.Drawing.Size(682, 475);
			this.tabInterface.TabIndex = 0;
			this.tabInterface.Text = "Interface";
			this.tabInterface.UseVisualStyleBackColor = true;
			// 
			// groupBox5
			// 
			this.groupBox5.Controls.Add(this.txtStatus);
			this.groupBox5.Location = new System.Drawing.Point(12, 519);
			this.groupBox5.Name = "groupBox5";
			this.groupBox5.Size = new System.Drawing.Size(690, 147);
			this.groupBox5.TabIndex = 2;
			this.groupBox5.TabStop = false;
			this.groupBox5.Text = "Status";
			// 
			// txtStatus
			// 
			this.txtStatus.BackColor = System.Drawing.Color.DimGray;
			this.txtStatus.ForeColor = System.Drawing.Color.Yellow;
			this.txtStatus.Location = new System.Drawing.Point(10, 19);
			this.txtStatus.Multiline = true;
			this.txtStatus.Name = "txtStatus";
			this.txtStatus.ReadOnly = true;
			this.txtStatus.Size = new System.Drawing.Size(674, 122);
			this.txtStatus.TabIndex = 1;
			// 
			// groupBox4
			// 
			this.groupBox4.Controls.Add(this.btnDownloadBrowse);
			this.groupBox4.Controls.Add(this.txtBytesToSave);
			this.groupBox4.Controls.Add(this.label11);
			this.groupBox4.Controls.Add(this.label10);
			this.groupBox4.Controls.Add(this.txtDownloadFile);
			this.groupBox4.Controls.Add(this.btnDownload);
			this.groupBox4.Location = new System.Drawing.Point(7, 7);
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new System.Drawing.Size(675, 88);
			this.groupBox4.TabIndex = 0;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "Download";
			// 
			// btnDownloadBrowse
			// 
			this.btnDownloadBrowse.Location = new System.Drawing.Point(559, 23);
			this.btnDownloadBrowse.Name = "btnDownloadBrowse";
			this.btnDownloadBrowse.Size = new System.Drawing.Size(110, 23);
			this.btnDownloadBrowse.TabIndex = 8;
			this.btnDownloadBrowse.Text = "&Browse";
			this.btnDownloadBrowse.UseVisualStyleBackColor = true;
			this.btnDownloadBrowse.Click += new System.EventHandler(this.btnDownloadBrowse_Click);
			// 
			// txtBytesToSave
			// 
			this.txtBytesToSave.Location = new System.Drawing.Point(102, 52);
			this.txtBytesToSave.MaxLength = 8;
			this.txtBytesToSave.Name = "txtBytesToSave";
			this.txtBytesToSave.Size = new System.Drawing.Size(113, 20);
			this.txtBytesToSave.TabIndex = 7;
			this.txtBytesToSave.Tag = "Download:BytesToSave";
			this.txtBytesToSave.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtBytesToSave_KeyPress);
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Location = new System.Drawing.Point(28, 56);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(74, 13);
			this.label11.TabIndex = 6;
			this.label11.Text = "Bytes to save:";
			this.label11.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(49, 27);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(47, 13);
			this.label10.TabIndex = 5;
			this.label10.Text = "Save to:";
			this.label10.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// txtDownloadFile
			// 
			this.txtDownloadFile.Location = new System.Drawing.Point(102, 25);
			this.txtDownloadFile.MaxLength = 512;
			this.txtDownloadFile.Name = "txtDownloadFile";
			this.txtDownloadFile.Size = new System.Drawing.Size(451, 20);
			this.txtDownloadFile.TabIndex = 4;
			this.txtDownloadFile.Tag = "Download:SaveTo";
			// 
			// btnDownload
			// 
			this.btnDownload.Enabled = false;
			this.btnDownload.Location = new System.Drawing.Point(559, 51);
			this.btnDownload.Name = "btnDownload";
			this.btnDownload.Size = new System.Drawing.Size(110, 23);
			this.btnDownload.TabIndex = 3;
			this.btnDownload.Text = "&Download";
			this.btnDownload.UseVisualStyleBackColor = true;
			this.btnDownload.Click += new System.EventHandler(this.btnDownload_Click);
			// 
			// tabWaveConv
			// 
			this.tabWaveConv.Controls.Add(this.groupBox3);
			this.tabWaveConv.Controls.Add(this.groupBox2);
			this.tabWaveConv.Location = new System.Drawing.Point(4, 22);
			this.tabWaveConv.Name = "tabWaveConv";
			this.tabWaveConv.Size = new System.Drawing.Size(682, 475);
			this.tabWaveConv.TabIndex = 2;
			this.tabWaveConv.Text = "Wave Conversion";
			this.tabWaveConv.UseVisualStyleBackColor = true;
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.txtBatchOutputFilename);
			this.groupBox3.Controls.Add(this.label12);
			this.groupBox3.Controls.Add(this.btnClear);
			this.groupBox3.Controls.Add(this.grdWaveFiles);
			this.groupBox3.Controls.Add(this.btnBatchConvert);
			this.groupBox3.Controls.Add(this.btnSelectWaveFolder);
			this.groupBox3.Controls.Add(this.label8);
			this.groupBox3.Location = new System.Drawing.Point(7, 148);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(678, 315);
			this.groupBox3.TabIndex = 6;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Batch Conversion";
			// 
			// txtBatchOutputFilename
			// 
			this.txtBatchOutputFilename.Location = new System.Drawing.Point(102, 239);
			this.txtBatchOutputFilename.Name = "txtBatchOutputFilename";
			this.txtBatchOutputFilename.Size = new System.Drawing.Size(563, 20);
			this.txtBatchOutputFilename.TabIndex = 7;
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.Location = new System.Drawing.Point(36, 242);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(58, 13);
			this.label12.TabIndex = 7;
			this.label12.Text = "Output file:";
			this.label12.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// btnClear
			// 
			this.btnClear.Location = new System.Drawing.Point(320, 278);
			this.btnClear.Name = "btnClear";
			this.btnClear.Size = new System.Drawing.Size(110, 23);
			this.btnClear.TabIndex = 7;
			this.btnClear.Text = "C&lear";
			this.btnClear.UseVisualStyleBackColor = true;
			this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
			// 
			// grdWaveFiles
			// 
			this.grdWaveFiles.AllowDrop = true;
			this.grdWaveFiles.AllowUserToResizeColumns = false;
			this.grdWaveFiles.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.grdWaveFiles.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.chkInclude,
            this.txtFilename});
			this.grdWaveFiles.Location = new System.Drawing.Point(102, 19);
			this.grdWaveFiles.Name = "grdWaveFiles";
			this.grdWaveFiles.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.grdWaveFiles.Size = new System.Drawing.Size(563, 214);
			this.grdWaveFiles.TabIndex = 6;
			this.grdWaveFiles.DragDrop += new System.Windows.Forms.DragEventHandler(this.grdWaveFiles_DragDrop);
			// 
			// chkInclude
			// 
			this.chkInclude.HeaderText = "Include";
			this.chkInclude.Name = "chkInclude";
			this.chkInclude.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.chkInclude.Width = 50;
			// 
			// txtFilename
			// 
			this.txtFilename.HeaderText = "Filename";
			this.txtFilename.Name = "txtFilename";
			this.txtFilename.Resizable = System.Windows.Forms.DataGridViewTriState.False;
			this.txtFilename.Width = 460;
			// 
			// btnBatchConvert
			// 
			this.btnBatchConvert.Location = new System.Drawing.Point(555, 278);
			this.btnBatchConvert.Name = "btnBatchConvert";
			this.btnBatchConvert.Size = new System.Drawing.Size(110, 23);
			this.btnBatchConvert.TabIndex = 5;
			this.btnBatchConvert.Text = "&Convert";
			this.btnBatchConvert.UseVisualStyleBackColor = true;
			this.btnBatchConvert.Click += new System.EventHandler(this.btnBatchConvert_Click);
			// 
			// btnSelectWaveFolder
			// 
			this.btnSelectWaveFolder.Location = new System.Drawing.Point(436, 278);
			this.btnSelectWaveFolder.Name = "btnSelectWaveFolder";
			this.btnSelectWaveFolder.Size = new System.Drawing.Size(110, 23);
			this.btnSelectWaveFolder.TabIndex = 5;
			this.btnSelectWaveFolder.Text = "&Select";
			this.btnSelectWaveFolder.UseVisualStyleBackColor = true;
			this.btnSelectWaveFolder.Click += new System.EventHandler(this.btnSelectWaveFolder_Click);
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(23, 19);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(73, 13);
			this.label8.TabIndex = 1;
			this.label8.Text = "Selected files:";
			this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.label9);
			this.groupBox2.Controls.Add(this.txtSingleOutputFilename);
			this.groupBox2.Controls.Add(this.btnSelectSingleWave);
			this.groupBox2.Controls.Add(this.btnConvertSingle);
			this.groupBox2.Controls.Add(this.txtSourceFile);
			this.groupBox2.Controls.Add(this.label7);
			this.groupBox2.Location = new System.Drawing.Point(7, 7);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(675, 135);
			this.groupBox2.TabIndex = 5;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Single File Conversion";
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(38, 56);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(58, 13);
			this.label9.TabIndex = 6;
			this.label9.Text = "Output file:";
			this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// txtSingleOutputFilename
			// 
			this.txtSingleOutputFilename.Location = new System.Drawing.Point(102, 53);
			this.txtSingleOutputFilename.Name = "txtSingleOutputFilename";
			this.txtSingleOutputFilename.Size = new System.Drawing.Size(451, 20);
			this.txtSingleOutputFilename.TabIndex = 5;
			// 
			// btnSelectSingleWave
			// 
			this.btnSelectSingleWave.Location = new System.Drawing.Point(559, 23);
			this.btnSelectSingleWave.Name = "btnSelectSingleWave";
			this.btnSelectSingleWave.Size = new System.Drawing.Size(110, 23);
			this.btnSelectSingleWave.TabIndex = 0;
			this.btnSelectSingleWave.Text = "&Select";
			this.btnSelectSingleWave.UseVisualStyleBackColor = true;
			this.btnSelectSingleWave.Click += new System.EventHandler(this.btnSelectSingleWave_Click);
			// 
			// btnConvertSingle
			// 
			this.btnConvertSingle.Enabled = false;
			this.btnConvertSingle.Location = new System.Drawing.Point(559, 50);
			this.btnConvertSingle.Name = "btnConvertSingle";
			this.btnConvertSingle.Size = new System.Drawing.Size(110, 23);
			this.btnConvertSingle.TabIndex = 3;
			this.btnConvertSingle.Text = "&Convert";
			this.btnConvertSingle.UseVisualStyleBackColor = true;
			this.btnConvertSingle.Click += new System.EventHandler(this.btnConvertSingle_Click);
			// 
			// txtSourceFile
			// 
			this.txtSourceFile.Location = new System.Drawing.Point(102, 25);
			this.txtSourceFile.Name = "txtSourceFile";
			this.txtSourceFile.Size = new System.Drawing.Size(451, 20);
			this.txtSourceFile.TabIndex = 1;
			this.txtSourceFile.Tag = "SingleConversion:SourceFile";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(36, 28);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(60, 13);
			this.label7.TabIndex = 2;
			this.label7.Text = "Source file:";
			this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tabSettings
			// 
			this.tabSettings.Controls.Add(this.groupBox1);
			this.tabSettings.Location = new System.Drawing.Point(4, 22);
			this.tabSettings.Name = "tabSettings";
			this.tabSettings.Padding = new System.Windows.Forms.Padding(3);
			this.tabSettings.Size = new System.Drawing.Size(682, 408);
			this.tabSettings.TabIndex = 1;
			this.tabSettings.Text = "Settings";
			this.tabSettings.UseVisualStyleBackColor = true;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.btnSaveSettings);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.comboPort);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.comboFlowControl);
			this.groupBox1.Controls.Add(this.comboStopBits);
			this.groupBox1.Controls.Add(this.comboParity);
			this.groupBox1.Controls.Add(this.comboDataBits);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.comboBPS);
			this.groupBox1.Location = new System.Drawing.Point(7, 7);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(200, 274);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "COM Port Settings";
			// 
			// btnSaveSettings
			// 
			this.btnSaveSettings.Location = new System.Drawing.Point(108, 245);
			this.btnSaveSettings.Name = "btnSaveSettings";
			this.btnSaveSettings.Size = new System.Drawing.Size(86, 23);
			this.btnSaveSettings.TabIndex = 12;
			this.btnSaveSettings.Text = "&Save";
			this.btnSaveSettings.UseVisualStyleBackColor = true;
			this.btnSaveSettings.Click += new System.EventHandler(this.btnSaveSettings_Click);
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(73, 22);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(29, 13);
			this.label6.TabIndex = 11;
			this.label6.Text = "Port:";
			this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// comboPort
			// 
			this.comboPort.FormattingEnabled = true;
			this.comboPort.Location = new System.Drawing.Point(108, 19);
			this.comboPort.Name = "comboPort";
			this.comboPort.Size = new System.Drawing.Size(86, 21);
			this.comboPort.TabIndex = 10;
			this.comboPort.Tag = "Settings:Port";
			this.comboPort.Text = "COM3";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(35, 158);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(67, 13);
			this.label5.TabIndex = 9;
			this.label5.Text = "Flow control:";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(51, 131);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(51, 13);
			this.label4.TabIndex = 8;
			this.label4.Text = "Stop bits:";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(66, 104);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(36, 13);
			this.label3.TabIndex = 7;
			this.label3.Text = "Parity:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(50, 76);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(52, 13);
			this.label2.TabIndex = 6;
			this.label2.Text = "Data bits:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// comboFlowControl
			// 
			this.comboFlowControl.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboFlowControl.FormattingEnabled = true;
			this.comboFlowControl.Items.AddRange(new object[] {
            "None",
            "Xon / Xoff",
            "Hardware"});
			this.comboFlowControl.Location = new System.Drawing.Point(108, 155);
			this.comboFlowControl.Name = "comboFlowControl";
			this.comboFlowControl.Size = new System.Drawing.Size(86, 21);
			this.comboFlowControl.TabIndex = 5;
			this.comboFlowControl.Tag = "Settings:FlowControl";
			// 
			// comboStopBits
			// 
			this.comboStopBits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboStopBits.FormattingEnabled = true;
			this.comboStopBits.Items.AddRange(new object[] {
            "1",
            "2"});
			this.comboStopBits.Location = new System.Drawing.Point(108, 128);
			this.comboStopBits.Name = "comboStopBits";
			this.comboStopBits.Size = new System.Drawing.Size(86, 21);
			this.comboStopBits.TabIndex = 4;
			this.comboStopBits.Tag = "Settings:StopBits";
			// 
			// comboParity
			// 
			this.comboParity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboParity.FormattingEnabled = true;
			this.comboParity.Items.AddRange(new object[] {
            "None",
            "Even",
            "Odd"});
			this.comboParity.Location = new System.Drawing.Point(108, 101);
			this.comboParity.Name = "comboParity";
			this.comboParity.Size = new System.Drawing.Size(86, 21);
			this.comboParity.TabIndex = 3;
			this.comboParity.Tag = "Settings:Parity";
			// 
			// comboDataBits
			// 
			this.comboDataBits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboDataBits.FormattingEnabled = true;
			this.comboDataBits.Items.AddRange(new object[] {
            "8",
            "7"});
			this.comboDataBits.Location = new System.Drawing.Point(108, 73);
			this.comboDataBits.Name = "comboDataBits";
			this.comboDataBits.Size = new System.Drawing.Size(86, 21);
			this.comboDataBits.TabIndex = 2;
			this.comboDataBits.Tag = "Settings:FrameSize";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(19, 49);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(83, 13);
			this.label1.TabIndex = 1;
			this.label1.Text = "Bits per second:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// comboBPS
			// 
			this.comboBPS.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBPS.FormatString = "N0";
			this.comboBPS.FormattingEnabled = true;
			this.comboBPS.Items.AddRange(new object[] {
            "300",
            "600",
            "1200",
            "2400",
            "4800",
            "9600",
            "14400",
            "19200",
            "28800",
            "33600",
            "57600",
            "115200",
            "230400",
            "460800",
            "921600"});
			this.comboBPS.Location = new System.Drawing.Point(107, 46);
			this.comboBPS.Name = "comboBPS";
			this.comboBPS.Size = new System.Drawing.Size(87, 21);
			this.comboBPS.TabIndex = 0;
			this.comboBPS.Tag = "Settings:Baud";
			// 
			// openWavFileDialog
			// 
			this.openWavFileDialog.Filter = "Wav files|*.wav|All files|*.*";
			// 
			// dlgWaveFolder
			// 
			this.dlgWaveFolder.RootFolder = System.Environment.SpecialFolder.MyComputer;
			this.dlgWaveFolder.ShowNewFolderButton = false;
			// 
			// timerSingleConvert
			// 
			this.timerSingleConvert.Tick += new System.EventHandler(this.timerSingleConvert_Tick);
			// 
			// saveFileDialog
			// 
			this.saveFileDialog.Filter = "BIN files|*.bin|All files|*.*";
			// 
			// timerInterface
			// 
			this.timerInterface.Tick += new System.EventHandler(this.timerInterface_Tick);
			// 
			// frmMain
			// 
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(715, 674);
			this.Controls.Add(this.groupBox5);
			this.Controls.Add(this.tabFunctions);
			this.MinimumSize = new System.Drawing.Size(731, 712);
			this.Name = "frmMain";
			this.Text = "Form1";
			this.tabFunctions.ResumeLayout(false);
			this.tabInterface.ResumeLayout(false);
			this.groupBox5.ResumeLayout(false);
			this.groupBox5.PerformLayout();
			this.groupBox4.ResumeLayout(false);
			this.groupBox4.PerformLayout();
			this.tabWaveConv.ResumeLayout(false);
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.grdWaveFiles)).EndInit();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.tabSettings.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabFunctions;
        private System.Windows.Forms.TabPage tabInterface;
        private System.Windows.Forms.TabPage tabSettings;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox comboBPS;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox comboFlowControl;
        private System.Windows.Forms.ComboBox comboStopBits;
        private System.Windows.Forms.ComboBox comboParity;
        private System.Windows.Forms.ComboBox comboDataBits;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox comboPort;
        private System.Windows.Forms.Button btnSaveSettings;
        private System.Windows.Forms.TabPage tabWaveConv;
        private System.Windows.Forms.Button btnConvertSingle;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtSourceFile;
        private System.Windows.Forms.Button btnSelectSingleWave;
		private System.Windows.Forms.OpenFileDialog openWavFileDialog;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.FolderBrowserDialog dlgWaveFolder;
		private System.Windows.Forms.Button btnSelectWaveFolder;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Button btnBatchConvert;
		private System.Windows.Forms.Timer timerSingleConvert;
		private System.Windows.Forms.GroupBox groupBox4;
		private System.Windows.Forms.SaveFileDialog saveFileDialog;
		private System.Windows.Forms.Button btnDownload;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.TextBox txtDownloadFile;
		private System.Windows.Forms.TextBox txtStatus;
		private System.Windows.Forms.GroupBox groupBox5;
		private System.Windows.Forms.TextBox txtBytesToSave;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.Button btnDownloadBrowse;
		private System.Windows.Forms.Timer timerInterface;
		private System.ComponentModel.BackgroundWorker backgroundWorker;
		private System.Windows.Forms.DataGridView grdWaveFiles;
		private System.Windows.Forms.Button btnClear;
		private System.Windows.Forms.DataGridViewCheckBoxColumn chkInclude;
		private System.Windows.Forms.DataGridViewTextBoxColumn txtFilename;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.TextBox txtSingleOutputFilename;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.TextBox txtBatchOutputFilename;
    }
}

