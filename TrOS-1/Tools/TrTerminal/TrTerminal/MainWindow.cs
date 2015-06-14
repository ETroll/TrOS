using System;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;
using TrTerminal.Serial;

namespace TrTerminal
{
    public partial class MainWindow : Form
    {
        private string preloadedImagePath;
        private bool isConnected;
        private SerialDevice device;

        delegate void AppendStringCallback(string c);

        public MainWindow()
        {
            InitializeComponent();
            UpdatePreloadedPath();
        }

        private void UpdatePreloadedPath()
        {
            if(string.IsNullOrEmpty(preloadedImagePath))
            {
                lblPreloadedPath.Text = "No image preloaded";
            }
            else
            {
                lblPreloadedPath.Text = Path.GetFileName(preloadedImagePath);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.toolstripLabel1.Text = "";
            isConnected = false;
            device = null;
            string[] data = SerialPort.GetPortNames();

            foreach(string port in data)
            {
                cbPorts.Items.Add(port);
            }

        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if(!isConnected)
            {
                if(cbPorts.SelectedItem == null)
                {
                    MessageBox.Show("No port selected");
                    return;
                }
                if (string.IsNullOrEmpty(preloadedImagePath))
                {
                    if (MessageBox.Show("No kernel image selected.\nDo you really want to continue?", "Warning!", MessageBoxButtons.OKCancel) != System.Windows.Forms.DialogResult.OK)
                    {
                        return;
                    }
                }
                device = new SerialDevice(cbPorts.SelectedItem.ToString());
                device.DataRecieved += device_DataRecieved;
                if(device.Connect())
                {
                    btnConnect.Text = "Disconnect";
                    cbPorts.Enabled = false;
                    isConnected = true;

                    if (!string.IsNullOrEmpty(preloadedImagePath))
                    {
                        device.UseKernel(File.ReadAllBytes(preloadedImagePath));
                    }
                }
                else
                {
                    device.DataRecieved -= device_DataRecieved;
                }
            }
            else
            {
                device.Disconnect();
                cbPorts.Enabled = true;
                isConnected = false;
                btnConnect.Text = "Connect";
                device.DataRecieved -= device_DataRecieved;
            }
        }

        void device_DataRecieved(object sender, DataEventArgs e)
        {
            AppendString(System.Text.Encoding.ASCII.GetString(e.Data));
        }

        private void AppendString(string c)
        {
            if (this.tbTerminal.InvokeRequired)
            {
                AppendStringCallback d = new AppendStringCallback(AppendString);
                this.Invoke(d, new object[] { c });
            }
            else
            {
                this.tbTerminal.Text += c;
                tbTerminal.SelectionStart = tbTerminal.Text.Length;
                tbTerminal.ScrollToCaret();
            }
        }

        private void btnPreload_Click(object sender, EventArgs e)
        {
            using(OpenFileDialog dialog = new OpenFileDialog())
            {
                dialog.Filter = "Image files (*.img)|*.img|All files (*.*)|*.*";
                if(dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    preloadedImagePath = dialog.FileName;
                }
            }
            UpdatePreloadedPath();
        }

        private void tbTerminal_KeyPress(object sender, KeyPressEventArgs e)
        {
            e.Handled = true;
            if(isConnected)
            {
                if(e.KeyChar == '\r')
                {
                    device.Send('\n');
                }
                else
                {
                    device.Send(e.KeyChar);
                }
            }
        }
    }
}
