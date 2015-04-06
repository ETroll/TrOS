using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace TrTerminal.Serial
{
    public delegate void DataRecievedEventHandler(object sender, DataEventArgs e);
    public delegate void KernelRequestEventHandler(object sender, EventArgs e);

    public class SerialDevice
    {
        private volatile bool connected;
        private SerialPort port;
        public event KernelRequestEventHandler KernelRequest;
        public event DataRecievedEventHandler DataRecieved;
        Thread readThread;

        private byte[] kernel;

        public SerialDevice(string portName)
        {
            port = new SerialPort(portName);
            port.BaudRate = 115200;
            port.DataBits = 8;
            port.StopBits = StopBits.One;
            port.Parity = Parity.None;
            port.ReadTimeout = 100;
            port.WriteTimeout = 500;
            connected = false;
            kernel = null;

            readThread = new Thread(Read);
        }

        private void OnKernelRequest(EventArgs e)
        {
            if (KernelRequest != null)
            {
                KernelRequest(this, e);
            }
        }

        private void OnDataRecieved(DataEventArgs e)
        {
            if (DataRecieved != null)
            {
                DataRecieved(this, e);
            }
        }

        public bool Connect()
        {
            if (!connected)
            {
                try
                {
                    if (port.IsOpen)
                    {
                        port.Close();
                    }
                    port.Open();
                    readThread.Start();
                    connected = true;
                }
                catch (UnauthorizedAccessException) { connected = false; }
                catch (IOException) { connected = false; }
                catch (ArgumentException) { connected = false; }
            }
            return connected;
        }

        public void Disconnect()
        {
            connected = false;
            readThread.Join();

        }

        public void UseKernel(byte[] k)
        {
            this.kernel = k;
        }

        public bool Send(char c)
        {
            if(connected)
            {
                try
                {
                    char[] buf = new char[1];
                    buf[0] = c;
                    port.Write(buf, 0, 1);
                    return true;
                }
                catch (TimeoutException) 
                { 
                    return false;
                }        
            }
            else
            {
                return false;
            }
        }

        private void SendKernel()
        {
            if (kernel != null && kernel.Length < 0x200000)
            {
                Console.WriteLine("### sending kernel [{0} byte]", kernel.Length);
                byte[] size = BitConverter.GetBytes(kernel.Length);

                //first send the size
                port.Write(size, 0, size.Length);

                char[] okBuffer = new char[2];
                port.Read(okBuffer, 0, 1);
                port.Read(okBuffer, 1, 1);
                if(okBuffer[0] == 'O' && okBuffer[1] == 'K')
                {
                    try
                    {
                        for (int i = 0; i < kernel.Length; i += 1000 )
                        {
                            int writelength = kernel.Length - i > 1000 ? 1000 : kernel.Length - i;
                            port.Write(kernel, i, writelength);
                        }
                            
                    }
                    catch(Exception e)
                    {
                        Console.WriteLine(e);
                    }
                    
                }
            }
        }

        private void Read()
        {
            int breakCharCount = 0;
            List<byte> buffer = new List<byte>();
            while (connected)
            {
                try
                {
                    byte data = (byte)port.ReadByte();
                    if(data == 0x03)
                    {
                        breakCharCount++;
                        if(breakCharCount == 3)
                        {
                            breakCharCount = 0;
                            OnDataRecieved(new DataEventArgs() { Data = buffer.ToArray() });
                            buffer.Clear();
                            SendKernel();
                        }
                    }
                    else
                    {
                        buffer.Add(data);
                        if(buffer.Count > 100)
                        {
                            OnDataRecieved(new DataEventArgs() { Data = buffer.ToArray() });
                            buffer.Clear();
                        }
                        
                    }                    
                }
                catch (TimeoutException) 
                {
                    if (buffer.Count > 0)
                    {
                        OnDataRecieved(new DataEventArgs() { Data = buffer.ToArray() });
                        buffer.Clear();
                    }
                }
            }
            port.Close();
            Console.WriteLine("Closing port");
        }
    }
}
