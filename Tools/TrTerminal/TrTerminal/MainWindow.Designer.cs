namespace TrTerminal
{
    partial class MainWindow
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
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolstripLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.cbPorts = new System.Windows.Forms.ComboBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.btnPreload = new System.Windows.Forms.Button();
            this.lblPreloadedPath = new System.Windows.Forms.Label();
            this.tbTerminal = new System.Windows.Forms.RichTextBox();
            this.statusStrip1.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolstripLabel1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 409);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(618, 22);
            this.statusStrip1.TabIndex = 0;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolstripLabel1
            // 
            this.toolstripLabel1.Name = "toolstripLabel1";
            this.toolstripLabel1.Size = new System.Drawing.Size(118, 17);
            this.toolstripLabel1.Text = "toolStripStatusLabel1";
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.BackColor = System.Drawing.SystemColors.Control;
            this.tableLayoutPanel1.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Inset;
            this.tableLayoutPanel1.ColumnCount = 4;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 120F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 126F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Controls.Add(this.cbPorts, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.btnConnect, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.btnPreload, 3, 0);
            this.tableLayoutPanel1.Controls.Add(this.lblPreloadedPath, 2, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(618, 31);
            this.tableLayoutPanel1.TabIndex = 1;
            // 
            // cbPorts
            // 
            this.cbPorts.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cbPorts.FormattingEnabled = true;
            this.cbPorts.Location = new System.Drawing.Point(5, 5);
            this.cbPorts.Name = "cbPorts";
            this.cbPorts.Size = new System.Drawing.Size(114, 21);
            this.cbPorts.TabIndex = 0;
            // 
            // btnConnect
            // 
            this.btnConnect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.btnConnect.Location = new System.Drawing.Point(127, 5);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(74, 21);
            this.btnConnect.TabIndex = 2;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // btnPreload
            // 
            this.btnPreload.Dock = System.Windows.Forms.DockStyle.Fill;
            this.btnPreload.Location = new System.Drawing.Point(493, 5);
            this.btnPreload.Name = "btnPreload";
            this.btnPreload.Size = new System.Drawing.Size(120, 21);
            this.btnPreload.TabIndex = 3;
            this.btnPreload.Text = "Preload image";
            this.btnPreload.UseVisualStyleBackColor = true;
            this.btnPreload.Click += new System.EventHandler(this.btnPreload_Click);
            // 
            // lblPreloadedPath
            // 
            this.lblPreloadedPath.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.lblPreloadedPath.AutoSize = true;
            this.lblPreloadedPath.Location = new System.Drawing.Point(383, 9);
            this.lblPreloadedPath.Name = "lblPreloadedPath";
            this.lblPreloadedPath.Size = new System.Drawing.Size(102, 13);
            this.lblPreloadedPath.TabIndex = 4;
            this.lblPreloadedPath.Text = "No image preloaded";
            // 
            // tbTerminal
            // 
            this.tbTerminal.BackColor = System.Drawing.Color.Black;
            this.tbTerminal.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbTerminal.Font = new System.Drawing.Font("Segoe UI Semibold", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbTerminal.ForeColor = System.Drawing.Color.White;
            this.tbTerminal.Location = new System.Drawing.Point(0, 31);
            this.tbTerminal.Name = "tbTerminal";
            this.tbTerminal.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
            this.tbTerminal.Size = new System.Drawing.Size(618, 378);
            this.tbTerminal.TabIndex = 2;
            this.tbTerminal.Text = "";
            this.tbTerminal.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbTerminal_KeyPress);
            // 
            // MainWindow
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(618, 431);
            this.Controls.Add(this.tbTerminal);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.statusStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainWindow";
            this.Text = "TrTerminal";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolstripLabel1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.ComboBox cbPorts;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Label lblPreloadedPath;
        private System.Windows.Forms.RichTextBox tbTerminal;
        private System.Windows.Forms.Button btnPreload;
    }
}

