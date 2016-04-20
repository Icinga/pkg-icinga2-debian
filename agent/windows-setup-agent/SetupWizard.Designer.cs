﻿namespace Icinga
{
	partial class SetupWizard
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SetupWizard));
			this.btnBack = new System.Windows.Forms.Button();
			this.btnNext = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.tabFinish = new System.Windows.Forms.TabPage();
			this.lblSetupCompleted = new System.Windows.Forms.Label();
			this.tabConfigure = new System.Windows.Forms.TabPage();
			this.lblConfigStatus = new System.Windows.Forms.Label();
			this.prgConfig = new System.Windows.Forms.ProgressBar();
			this.tabParameters = new System.Windows.Forms.TabPage();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.chkAcceptConfig = new System.Windows.Forms.CheckBox();
			this.chkAcceptCommands = new System.Windows.Forms.CheckBox();
			this.txtTicket = new System.Windows.Forms.TextBox();
			this.lblTicket = new System.Windows.Forms.Label();
			this.txtInstanceName = new System.Windows.Forms.TextBox();
			this.lblInstanceName = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.rdoNoListener = new System.Windows.Forms.RadioButton();
			this.txtListenerPort = new System.Windows.Forms.TextBox();
			this.lblListenerPort = new System.Windows.Forms.Label();
			this.rdoListener = new System.Windows.Forms.RadioButton();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.btnRemoveEndpoint = new System.Windows.Forms.Button();
			this.btnAddEndpoint = new System.Windows.Forms.Button();
			this.lvwEndpoints = new System.Windows.Forms.ListView();
			this.colInstanceName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.colHost = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.colPort = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.rdoNoMaster = new System.Windows.Forms.RadioButton();
			this.rdoNewMaster = new System.Windows.Forms.RadioButton();
			this.tbcPages = new System.Windows.Forms.TabControl();
			this.tabRetrieveCertificate = new System.Windows.Forms.TabPage();
			this.lblRetrieveCertificate = new System.Windows.Forms.Label();
			this.prgRetrieveCertificate = new System.Windows.Forms.ProgressBar();
			this.tabVerifyCertificate = new System.Windows.Forms.TabPage();
			this.grpX509Fields = new System.Windows.Forms.GroupBox();
			this.txtX509Field = new System.Windows.Forms.TextBox();
			this.lvwX509Fields = new System.Windows.Forms.ListView();
			this.colField = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.colValue = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.txtX509Subject = new System.Windows.Forms.TextBox();
			this.txtX509Issuer = new System.Windows.Forms.TextBox();
			this.lblX509Subject = new System.Windows.Forms.Label();
			this.lblX509Issuer = new System.Windows.Forms.Label();
			this.lblX509Prompt = new System.Windows.Forms.Label();
			this.tabError = new System.Windows.Forms.TabPage();
			this.txtError = new System.Windows.Forms.TextBox();
			this.lblError = new System.Windows.Forms.Label();
			this.picBanner = new System.Windows.Forms.PictureBox();
			this.chkInstallNSCP = new System.Windows.Forms.CheckBox();
			this.tabFinish.SuspendLayout();
			this.tabConfigure.SuspendLayout();
			this.tabParameters.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.tbcPages.SuspendLayout();
			this.tabRetrieveCertificate.SuspendLayout();
			this.tabVerifyCertificate.SuspendLayout();
			this.grpX509Fields.SuspendLayout();
			this.tabError.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.picBanner)).BeginInit();
			this.SuspendLayout();
			// 
			// btnBack
			// 
			this.btnBack.Enabled = false;
			this.btnBack.Location = new System.Drawing.Point(367, 592);
			this.btnBack.Name = "btnBack";
			this.btnBack.Size = new System.Drawing.Size(75, 23);
			this.btnBack.TabIndex = 1;
			this.btnBack.Text = "< &Back";
			this.btnBack.UseVisualStyleBackColor = true;
			this.btnBack.Click += new System.EventHandler(this.btnBack_Click);
			// 
			// btnNext
			// 
			this.btnNext.Location = new System.Drawing.Point(448, 592);
			this.btnNext.Name = "btnNext";
			this.btnNext.Size = new System.Drawing.Size(75, 23);
			this.btnNext.TabIndex = 2;
			this.btnNext.Text = "&Next >";
			this.btnNext.UseVisualStyleBackColor = true;
			this.btnNext.Click += new System.EventHandler(this.btnNext_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Location = new System.Drawing.Point(538, 592);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(75, 23);
			this.btnCancel.TabIndex = 3;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// tabFinish
			// 
			this.tabFinish.Controls.Add(this.lblSetupCompleted);
			this.tabFinish.Location = new System.Drawing.Point(4, 5);
			this.tabFinish.Name = "tabFinish";
			this.tabFinish.Padding = new System.Windows.Forms.Padding(3);
			this.tabFinish.Size = new System.Drawing.Size(617, 500);
			this.tabFinish.TabIndex = 5;
			this.tabFinish.Text = "Finish";
			this.tabFinish.UseVisualStyleBackColor = true;
			// 
			// lblSetupCompleted
			// 
			this.lblSetupCompleted.AutoSize = true;
			this.lblSetupCompleted.Location = new System.Drawing.Point(34, 35);
			this.lblSetupCompleted.Name = "lblSetupCompleted";
			this.lblSetupCompleted.Size = new System.Drawing.Size(214, 13);
			this.lblSetupCompleted.TabIndex = 0;
			this.lblSetupCompleted.Text = "The Icinga 2 agent was set up successfully.";
			// 
			// tabConfigure
			// 
			this.tabConfigure.Controls.Add(this.lblConfigStatus);
			this.tabConfigure.Controls.Add(this.prgConfig);
			this.tabConfigure.Location = new System.Drawing.Point(4, 5);
			this.tabConfigure.Name = "tabConfigure";
			this.tabConfigure.Padding = new System.Windows.Forms.Padding(3);
			this.tabConfigure.Size = new System.Drawing.Size(617, 500);
			this.tabConfigure.TabIndex = 4;
			this.tabConfigure.Text = "Configure Icinga 2";
			this.tabConfigure.UseVisualStyleBackColor = true;
			// 
			// lblConfigStatus
			// 
			this.lblConfigStatus.AutoSize = true;
			this.lblConfigStatus.Location = new System.Drawing.Point(184, 204);
			this.lblConfigStatus.Name = "lblConfigStatus";
			this.lblConfigStatus.Size = new System.Drawing.Size(141, 13);
			this.lblConfigStatus.TabIndex = 1;
			this.lblConfigStatus.Text = "Updating the configuration...";
			// 
			// prgConfig
			// 
			this.prgConfig.Location = new System.Drawing.Point(184, 223);
			this.prgConfig.Name = "prgConfig";
			this.prgConfig.Size = new System.Drawing.Size(289, 23);
			this.prgConfig.TabIndex = 0;
			// 
			// tabParameters
			// 
			this.tabParameters.Controls.Add(this.groupBox3);
			this.tabParameters.Controls.Add(this.txtTicket);
			this.tabParameters.Controls.Add(this.lblTicket);
			this.tabParameters.Controls.Add(this.txtInstanceName);
			this.tabParameters.Controls.Add(this.lblInstanceName);
			this.tabParameters.Controls.Add(this.groupBox2);
			this.tabParameters.Controls.Add(this.groupBox1);
			this.tabParameters.Location = new System.Drawing.Point(4, 5);
			this.tabParameters.Name = "tabParameters";
			this.tabParameters.Padding = new System.Windows.Forms.Padding(3);
			this.tabParameters.Size = new System.Drawing.Size(617, 500);
			this.tabParameters.TabIndex = 3;
			this.tabParameters.Text = "Agent Parameters";
			this.tabParameters.UseVisualStyleBackColor = true;
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.chkInstallNSCP);
			this.groupBox3.Controls.Add(this.chkAcceptConfig);
			this.groupBox3.Controls.Add(this.chkAcceptCommands);
			this.groupBox3.Location = new System.Drawing.Point(308, 359);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(301, 111);
			this.groupBox3.TabIndex = 5;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Advanced Options";
			// 
			// chkAcceptConfig
			// 
			this.chkAcceptConfig.AutoSize = true;
			this.chkAcceptConfig.Location = new System.Drawing.Point(9, 47);
			this.chkAcceptConfig.Name = "chkAcceptConfig";
			this.chkAcceptConfig.Size = new System.Drawing.Size(190, 17);
			this.chkAcceptConfig.TabIndex = 1;
			this.chkAcceptConfig.Text = "Accept config updates from master";
			this.chkAcceptConfig.UseVisualStyleBackColor = true;
			// 
			// chkAcceptCommands
			// 
			this.chkAcceptCommands.AutoSize = true;
			this.chkAcceptCommands.Location = new System.Drawing.Point(9, 24);
			this.chkAcceptCommands.Name = "chkAcceptCommands";
			this.chkAcceptCommands.Size = new System.Drawing.Size(171, 17);
			this.chkAcceptCommands.TabIndex = 0;
			this.chkAcceptCommands.Text = "Accept commands from master";
			this.chkAcceptCommands.UseVisualStyleBackColor = true;
			// 
			// txtTicket
			// 
			this.txtTicket.Location = new System.Drawing.Point(98, 45);
			this.txtTicket.Name = "txtTicket";
			this.txtTicket.Size = new System.Drawing.Size(340, 20);
			this.txtTicket.TabIndex = 1;
			// 
			// lblTicket
			// 
			this.lblTicket.AutoSize = true;
			this.lblTicket.Location = new System.Drawing.Point(9, 48);
			this.lblTicket.Name = "lblTicket";
			this.lblTicket.Size = new System.Drawing.Size(71, 13);
			this.lblTicket.TabIndex = 4;
			this.lblTicket.Text = "Setup Ticket:";
			// 
			// txtInstanceName
			// 
			this.txtInstanceName.Location = new System.Drawing.Point(98, 16);
			this.txtInstanceName.Name = "txtInstanceName";
			this.txtInstanceName.Size = new System.Drawing.Size(340, 20);
			this.txtInstanceName.TabIndex = 0;
			// 
			// lblInstanceName
			// 
			this.lblInstanceName.AutoSize = true;
			this.lblInstanceName.Location = new System.Drawing.Point(9, 20);
			this.lblInstanceName.Name = "lblInstanceName";
			this.lblInstanceName.Size = new System.Drawing.Size(82, 13);
			this.lblInstanceName.TabIndex = 3;
			this.lblInstanceName.Text = "Instance Name:";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.rdoNoListener);
			this.groupBox2.Controls.Add(this.txtListenerPort);
			this.groupBox2.Controls.Add(this.lblListenerPort);
			this.groupBox2.Controls.Add(this.rdoListener);
			this.groupBox2.Location = new System.Drawing.Point(8, 359);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(294, 111);
			this.groupBox2.TabIndex = 2;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "TCP Listener";
			// 
			// rdoNoListener
			// 
			this.rdoNoListener.AutoSize = true;
			this.rdoNoListener.Checked = true;
			this.rdoNoListener.Location = new System.Drawing.Point(11, 82);
			this.rdoNoListener.Name = "rdoNoListener";
			this.rdoNoListener.Size = new System.Drawing.Size(163, 17);
			this.rdoNoListener.TabIndex = 9;
			this.rdoNoListener.TabStop = true;
			this.rdoNoListener.Text = "Do not listen for connections.";
			this.rdoNoListener.UseVisualStyleBackColor = true;
			this.rdoNoListener.CheckedChanged += new System.EventHandler(this.RadioListener_CheckedChanged);
			// 
			// txtListenerPort
			// 
			this.txtListenerPort.Enabled = false;
			this.txtListenerPort.Location = new System.Drawing.Point(132, 51);
			this.txtListenerPort.Name = "txtListenerPort";
			this.txtListenerPort.Size = new System.Drawing.Size(84, 20);
			this.txtListenerPort.TabIndex = 8;
			this.txtListenerPort.Text = "5665";
			// 
			// lblListenerPort
			// 
			this.lblListenerPort.AutoSize = true;
			this.lblListenerPort.Location = new System.Drawing.Point(43, 55);
			this.lblListenerPort.Name = "lblListenerPort";
			this.lblListenerPort.Size = new System.Drawing.Size(29, 13);
			this.lblListenerPort.TabIndex = 1;
			this.lblListenerPort.Text = "Port:";
			// 
			// rdoListener
			// 
			this.rdoListener.AutoSize = true;
			this.rdoListener.Location = new System.Drawing.Point(11, 24);
			this.rdoListener.Name = "rdoListener";
			this.rdoListener.Size = new System.Drawing.Size(250, 17);
			this.rdoListener.TabIndex = 7;
			this.rdoListener.Text = "Listen for connections from the master instance:";
			this.rdoListener.UseVisualStyleBackColor = true;
			this.rdoListener.CheckedChanged += new System.EventHandler(this.RadioListener_CheckedChanged);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.btnRemoveEndpoint);
			this.groupBox1.Controls.Add(this.btnAddEndpoint);
			this.groupBox1.Controls.Add(this.lvwEndpoints);
			this.groupBox1.Controls.Add(this.rdoNoMaster);
			this.groupBox1.Controls.Add(this.rdoNewMaster);
			this.groupBox1.Location = new System.Drawing.Point(8, 77);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(601, 276);
			this.groupBox1.TabIndex = 1;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Master Instance";
			// 
			// btnRemoveEndpoint
			// 
			this.btnRemoveEndpoint.Enabled = false;
			this.btnRemoveEndpoint.Location = new System.Drawing.Point(520, 112);
			this.btnRemoveEndpoint.Name = "btnRemoveEndpoint";
			this.btnRemoveEndpoint.Size = new System.Drawing.Size(75, 23);
			this.btnRemoveEndpoint.TabIndex = 6;
			this.btnRemoveEndpoint.Text = "Remove";
			this.btnRemoveEndpoint.UseVisualStyleBackColor = true;
			this.btnRemoveEndpoint.Click += new System.EventHandler(this.btnRemoveEndpoint_Click);
			// 
			// btnAddEndpoint
			// 
			this.btnAddEndpoint.Location = new System.Drawing.Point(520, 83);
			this.btnAddEndpoint.Name = "btnAddEndpoint";
			this.btnAddEndpoint.Size = new System.Drawing.Size(75, 23);
			this.btnAddEndpoint.TabIndex = 5;
			this.btnAddEndpoint.Text = "Add";
			this.btnAddEndpoint.UseVisualStyleBackColor = true;
			this.btnAddEndpoint.Click += new System.EventHandler(this.btnAddEndpoint_Click);
			// 
			// lvwEndpoints
			// 
			this.lvwEndpoints.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colInstanceName,
            this.colHost,
            this.colPort});
			this.lvwEndpoints.FullRowSelect = true;
			this.lvwEndpoints.Location = new System.Drawing.Point(11, 83);
			this.lvwEndpoints.Name = "lvwEndpoints";
			this.lvwEndpoints.Size = new System.Drawing.Size(500, 176);
			this.lvwEndpoints.TabIndex = 4;
			this.lvwEndpoints.UseCompatibleStateImageBehavior = false;
			this.lvwEndpoints.View = System.Windows.Forms.View.Details;
			this.lvwEndpoints.SelectedIndexChanged += new System.EventHandler(this.lvwEndpoints_SelectedIndexChanged);
			// 
			// colInstanceName
			// 
			this.colInstanceName.Text = "Instance Name";
			this.colInstanceName.Width = 200;
			// 
			// colHost
			// 
			this.colHost.Text = "Host";
			this.colHost.Width = 200;
			// 
			// colPort
			// 
			this.colPort.Text = "Port";
			this.colPort.Width = 80;
			// 
			// rdoNoMaster
			// 
			this.rdoNoMaster.AutoSize = true;
			this.rdoNoMaster.Checked = true;
			this.rdoNoMaster.Location = new System.Drawing.Point(11, 50);
			this.rdoNoMaster.Name = "rdoNoMaster";
			this.rdoNoMaster.Size = new System.Drawing.Size(374, 17);
			this.rdoNoMaster.TabIndex = 3;
			this.rdoNoMaster.TabStop = true;
			this.rdoNoMaster.Text = "This instance should report its check results to an existing Icinga 2 master:";
			this.rdoNoMaster.UseVisualStyleBackColor = true;
			this.rdoNoMaster.CheckedChanged += new System.EventHandler(this.RadioMaster_CheckedChanged);
			// 
			// rdoNewMaster
			// 
			this.rdoNewMaster.AutoSize = true;
			this.rdoNewMaster.Enabled = false;
			this.rdoNewMaster.Location = new System.Drawing.Point(11, 22);
			this.rdoNewMaster.Name = "rdoNewMaster";
			this.rdoNewMaster.Size = new System.Drawing.Size(167, 17);
			this.rdoNewMaster.TabIndex = 2;
			this.rdoNewMaster.TabStop = true;
			this.rdoNewMaster.Text = "This is a new master instance.";
			this.rdoNewMaster.UseVisualStyleBackColor = true;
			this.rdoNewMaster.CheckedChanged += new System.EventHandler(this.RadioMaster_CheckedChanged);
			// 
			// tbcPages
			// 
			this.tbcPages.Appearance = System.Windows.Forms.TabAppearance.FlatButtons;
			this.tbcPages.Controls.Add(this.tabParameters);
			this.tbcPages.Controls.Add(this.tabRetrieveCertificate);
			this.tbcPages.Controls.Add(this.tabVerifyCertificate);
			this.tbcPages.Controls.Add(this.tabConfigure);
			this.tbcPages.Controls.Add(this.tabFinish);
			this.tbcPages.Controls.Add(this.tabError);
			this.tbcPages.ItemSize = new System.Drawing.Size(0, 1);
			this.tbcPages.Location = new System.Drawing.Point(0, 80);
			this.tbcPages.Margin = new System.Windows.Forms.Padding(0);
			this.tbcPages.Name = "tbcPages";
			this.tbcPages.SelectedIndex = 0;
			this.tbcPages.Size = new System.Drawing.Size(625, 509);
			this.tbcPages.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
			this.tbcPages.TabIndex = 0;
			this.tbcPages.SelectedIndexChanged += new System.EventHandler(this.tbcPages_SelectedIndexChanged);
			// 
			// tabRetrieveCertificate
			// 
			this.tabRetrieveCertificate.Controls.Add(this.lblRetrieveCertificate);
			this.tabRetrieveCertificate.Controls.Add(this.prgRetrieveCertificate);
			this.tabRetrieveCertificate.Location = new System.Drawing.Point(4, 5);
			this.tabRetrieveCertificate.Name = "tabRetrieveCertificate";
			this.tabRetrieveCertificate.Padding = new System.Windows.Forms.Padding(3);
			this.tabRetrieveCertificate.Size = new System.Drawing.Size(617, 500);
			this.tabRetrieveCertificate.TabIndex = 7;
			this.tabRetrieveCertificate.Text = "Checking Certificate";
			this.tabRetrieveCertificate.UseVisualStyleBackColor = true;
			// 
			// lblRetrieveCertificate
			// 
			this.lblRetrieveCertificate.AutoSize = true;
			this.lblRetrieveCertificate.Location = new System.Drawing.Point(164, 229);
			this.lblRetrieveCertificate.Name = "lblRetrieveCertificate";
			this.lblRetrieveCertificate.Size = new System.Drawing.Size(110, 13);
			this.lblRetrieveCertificate.TabIndex = 3;
			this.lblRetrieveCertificate.Text = "Checking certificate...";
			// 
			// prgRetrieveCertificate
			// 
			this.prgRetrieveCertificate.Location = new System.Drawing.Point(164, 248);
			this.prgRetrieveCertificate.Name = "prgRetrieveCertificate";
			this.prgRetrieveCertificate.Size = new System.Drawing.Size(289, 23);
			this.prgRetrieveCertificate.TabIndex = 2;
			// 
			// tabVerifyCertificate
			// 
			this.tabVerifyCertificate.Controls.Add(this.grpX509Fields);
			this.tabVerifyCertificate.Controls.Add(this.txtX509Subject);
			this.tabVerifyCertificate.Controls.Add(this.txtX509Issuer);
			this.tabVerifyCertificate.Controls.Add(this.lblX509Subject);
			this.tabVerifyCertificate.Controls.Add(this.lblX509Issuer);
			this.tabVerifyCertificate.Controls.Add(this.lblX509Prompt);
			this.tabVerifyCertificate.Location = new System.Drawing.Point(4, 5);
			this.tabVerifyCertificate.Name = "tabVerifyCertificate";
			this.tabVerifyCertificate.Padding = new System.Windows.Forms.Padding(3);
			this.tabVerifyCertificate.Size = new System.Drawing.Size(617, 500);
			this.tabVerifyCertificate.TabIndex = 6;
			this.tabVerifyCertificate.Text = "Verify Certificate";
			this.tabVerifyCertificate.UseVisualStyleBackColor = true;
			// 
			// grpX509Fields
			// 
			this.grpX509Fields.Controls.Add(this.txtX509Field);
			this.grpX509Fields.Controls.Add(this.lvwX509Fields);
			this.grpX509Fields.Location = new System.Drawing.Point(11, 115);
			this.grpX509Fields.Name = "grpX509Fields";
			this.grpX509Fields.Size = new System.Drawing.Size(598, 369);
			this.grpX509Fields.TabIndex = 8;
			this.grpX509Fields.TabStop = false;
			this.grpX509Fields.Text = "X509 Fields";
			// 
			// txtX509Field
			// 
			this.txtX509Field.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtX509Field.Location = new System.Drawing.Point(6, 197);
			this.txtX509Field.Multiline = true;
			this.txtX509Field.Name = "txtX509Field";
			this.txtX509Field.ReadOnly = true;
			this.txtX509Field.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.txtX509Field.Size = new System.Drawing.Size(586, 166);
			this.txtX509Field.TabIndex = 9;
			// 
			// lvwX509Fields
			// 
			this.lvwX509Fields.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colField,
            this.colValue});
			this.lvwX509Fields.Location = new System.Drawing.Point(6, 19);
			this.lvwX509Fields.Name = "lvwX509Fields";
			this.lvwX509Fields.Size = new System.Drawing.Size(586, 172);
			this.lvwX509Fields.TabIndex = 8;
			this.lvwX509Fields.UseCompatibleStateImageBehavior = false;
			this.lvwX509Fields.View = System.Windows.Forms.View.Details;
			this.lvwX509Fields.SelectedIndexChanged += new System.EventHandler(this.lvwX509Fields_SelectedIndexChanged);
			// 
			// colField
			// 
			this.colField.Text = "Field";
			this.colField.Width = 200;
			// 
			// colValue
			// 
			this.colValue.Text = "Value";
			this.colValue.Width = 350;
			// 
			// txtX509Subject
			// 
			this.txtX509Subject.Location = new System.Drawing.Point(71, 73);
			this.txtX509Subject.Name = "txtX509Subject";
			this.txtX509Subject.ReadOnly = true;
			this.txtX509Subject.Size = new System.Drawing.Size(532, 20);
			this.txtX509Subject.TabIndex = 4;
			// 
			// txtX509Issuer
			// 
			this.txtX509Issuer.Location = new System.Drawing.Point(71, 47);
			this.txtX509Issuer.Name = "txtX509Issuer";
			this.txtX509Issuer.ReadOnly = true;
			this.txtX509Issuer.Size = new System.Drawing.Size(532, 20);
			this.txtX509Issuer.TabIndex = 3;
			// 
			// lblX509Subject
			// 
			this.lblX509Subject.AutoSize = true;
			this.lblX509Subject.Location = new System.Drawing.Point(8, 77);
			this.lblX509Subject.Name = "lblX509Subject";
			this.lblX509Subject.Size = new System.Drawing.Size(46, 13);
			this.lblX509Subject.TabIndex = 2;
			this.lblX509Subject.Text = "Subject:";
			// 
			// lblX509Issuer
			// 
			this.lblX509Issuer.AutoSize = true;
			this.lblX509Issuer.Location = new System.Drawing.Point(8, 50);
			this.lblX509Issuer.Name = "lblX509Issuer";
			this.lblX509Issuer.Size = new System.Drawing.Size(38, 13);
			this.lblX509Issuer.TabIndex = 1;
			this.lblX509Issuer.Text = "Issuer:";
			// 
			// lblX509Prompt
			// 
			this.lblX509Prompt.AutoSize = true;
			this.lblX509Prompt.Location = new System.Drawing.Point(8, 15);
			this.lblX509Prompt.Name = "lblX509Prompt";
			this.lblX509Prompt.Size = new System.Drawing.Size(201, 13);
			this.lblX509Prompt.TabIndex = 0;
			this.lblX509Prompt.Text = "Please verify the master\'s SSL certificate:";
			// 
			// tabError
			// 
			this.tabError.Controls.Add(this.txtError);
			this.tabError.Controls.Add(this.lblError);
			this.tabError.Location = new System.Drawing.Point(4, 5);
			this.tabError.Name = "tabError";
			this.tabError.Padding = new System.Windows.Forms.Padding(3);
			this.tabError.Size = new System.Drawing.Size(617, 500);
			this.tabError.TabIndex = 8;
			this.tabError.Text = "Error";
			this.tabError.UseVisualStyleBackColor = true;
			// 
			// txtError
			// 
			this.txtError.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtError.Location = new System.Drawing.Point(11, 38);
			this.txtError.Multiline = true;
			this.txtError.Name = "txtError";
			this.txtError.ReadOnly = true;
			this.txtError.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.txtError.Size = new System.Drawing.Size(598, 397);
			this.txtError.TabIndex = 1;
			// 
			// lblError
			// 
			this.lblError.AutoSize = true;
			this.lblError.Location = new System.Drawing.Point(8, 12);
			this.lblError.Name = "lblError";
			this.lblError.Size = new System.Drawing.Size(209, 13);
			this.lblError.TabIndex = 0;
			this.lblError.Text = "An error occurred while setting up Icinga 2:";
			// 
			// picBanner
			// 
			this.picBanner.Image = global::Icinga.Properties.Resources.icinga_banner;
			this.picBanner.Location = new System.Drawing.Point(0, 0);
			this.picBanner.Name = "picBanner";
			this.picBanner.Size = new System.Drawing.Size(625, 77);
			this.picBanner.TabIndex = 1;
			this.picBanner.TabStop = false;
			// 
			// chkInstallNSCP
			// 
			this.chkInstallNSCP.AutoSize = true;
			this.chkInstallNSCP.Location = new System.Drawing.Point(9, 70);
			this.chkInstallNSCP.Name = "chkInstallNSCP";
			this.chkInstallNSCP.Size = new System.Drawing.Size(149, 17);
			this.chkInstallNSCP.TabIndex = 6;
			this.chkInstallNSCP.Text = "Install/Update NSClient++";
			this.chkInstallNSCP.UseVisualStyleBackColor = true;
			// 
			// SetupWizard
			// 
			this.AcceptButton = this.btnNext;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.btnCancel;
			this.ClientSize = new System.Drawing.Size(625, 624);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.btnNext);
			this.Controls.Add(this.btnBack);
			this.Controls.Add(this.picBanner);
			this.Controls.Add(this.tbcPages);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "SetupWizard";
			this.Text = "Icinga 2 Setup Wizard";
			this.tabFinish.ResumeLayout(false);
			this.tabFinish.PerformLayout();
			this.tabConfigure.ResumeLayout(false);
			this.tabConfigure.PerformLayout();
			this.tabParameters.ResumeLayout(false);
			this.tabParameters.PerformLayout();
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.tbcPages.ResumeLayout(false);
			this.tabRetrieveCertificate.ResumeLayout(false);
			this.tabRetrieveCertificate.PerformLayout();
			this.tabVerifyCertificate.ResumeLayout(false);
			this.tabVerifyCertificate.PerformLayout();
			this.grpX509Fields.ResumeLayout(false);
			this.grpX509Fields.PerformLayout();
			this.tabError.ResumeLayout(false);
			this.tabError.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.picBanner)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.PictureBox picBanner;
		private System.Windows.Forms.Button btnBack;
		private System.Windows.Forms.Button btnNext;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.TabPage tabFinish;
		private System.Windows.Forms.Label lblSetupCompleted;
		private System.Windows.Forms.TabPage tabConfigure;
		private System.Windows.Forms.Label lblConfigStatus;
		private System.Windows.Forms.ProgressBar prgConfig;
		private System.Windows.Forms.TabPage tabParameters;
		private System.Windows.Forms.TextBox txtInstanceName;
		private System.Windows.Forms.Label lblInstanceName;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.RadioButton rdoNoListener;
		private System.Windows.Forms.TextBox txtListenerPort;
		private System.Windows.Forms.Label lblListenerPort;
		private System.Windows.Forms.RadioButton rdoListener;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button btnRemoveEndpoint;
		private System.Windows.Forms.Button btnAddEndpoint;
		private System.Windows.Forms.ListView lvwEndpoints;
		private System.Windows.Forms.ColumnHeader colHost;
		private System.Windows.Forms.ColumnHeader colPort;
		private System.Windows.Forms.RadioButton rdoNoMaster;
		private System.Windows.Forms.RadioButton rdoNewMaster;
		private System.Windows.Forms.TabControl tbcPages;
		private System.Windows.Forms.TabPage tabVerifyCertificate;
		private System.Windows.Forms.Label lblX509Prompt;
		private System.Windows.Forms.TextBox txtX509Subject;
		private System.Windows.Forms.TextBox txtX509Issuer;
		private System.Windows.Forms.Label lblX509Subject;
		private System.Windows.Forms.Label lblX509Issuer;
		private System.Windows.Forms.GroupBox grpX509Fields;
		private System.Windows.Forms.ListView lvwX509Fields;
		private System.Windows.Forms.ColumnHeader colField;
		private System.Windows.Forms.ColumnHeader colValue;
		private System.Windows.Forms.TextBox txtX509Field;
		private System.Windows.Forms.TabPage tabRetrieveCertificate;
		private System.Windows.Forms.Label lblRetrieveCertificate;
		private System.Windows.Forms.ProgressBar prgRetrieveCertificate;
		private System.Windows.Forms.TabPage tabError;
		private System.Windows.Forms.TextBox txtError;
		private System.Windows.Forms.Label lblError;
		private System.Windows.Forms.TextBox txtTicket;
		private System.Windows.Forms.Label lblTicket;
		private System.Windows.Forms.ColumnHeader colInstanceName;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.CheckBox chkAcceptConfig;
        private System.Windows.Forms.CheckBox chkAcceptCommands;
	private System.Windows.Forms.CheckBox chkInstallNSCP;
	}
}

