using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using DataSet_Generator;

namespace serial_communication
{
    public partial class Send_Recieve : Form
    {
        private SerialPort serialPort = new SerialPort();
        // Threads
        Thread t;
        ManualResetEvent runThread = new ManualResetEvent(false);

        // Delegates
        private delegate void DelegateAddToList(string msg);
        private DelegateAddToList m_DelegateAddToList;
        private delegate void DelegateStopPerfmormClick();
        private DelegateStopPerfmormClick m_DelegateStop;

        public Send_Recieve()
        {
            InitializeComponent();
            string[] allSerialPorts = SerialPort.GetPortNames();
            comboBox1.DataSource = allSerialPorts;
            comboBox1.SelectedIndex = 0;
        }

        private void Send_Recieve_Load(object sender, EventArgs e)
        {

            m_DelegateAddToList = new DelegateAddToList(AddToList);
            m_DelegateStop = new DelegateStopPerfmormClick(close_serialport);
            //serialPort.DataReceived += new SerialDataReceivedEventHandler(serialPort_DataReceived);

            t = new Thread(ReceiveThread);
            t.Start();
        }
        private void ReceiveThread()
        {
            while (true)
            {
                runThread.WaitOne(Timeout.Infinite);
                while (true)
                {
                    try
                    {
                        // receive data 
                        string msg = serialPort.ReadLine();
                       // m_DelegateAddToList.DynamicInvoke(new Object[] { "R: " + msg });
                       // m_DelegateAddToList.BeginInvoke(msg, null, this);
                        this.Invoke(this.m_DelegateAddToList, new Object[] { "R: " + msg });
                    }
                    catch
                    {
                        try
                        {
                            this.Invoke(this.m_DelegateStop, new Object[] { });
                        }
                        catch { }
                        runThread.Reset();
                        break;
                    }
                }
            }
        }
        /*
        private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            // blocks until TERM_CHAR is received
            try
            {
                string msg = serialPort.ReadLine();
                this.Invoke(this.m_DelegateAddToList, new Object[] { "R: " + msg });
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                stop_button.PerformClick();
            }
        }
        */
        private void AddToList(string msg)
        {
            int n = msg_listbox.Items.Add(msg);
            msg_listbox.SelectedIndex = n;
            msg_listbox.ClearSelected();
        }

        private void start_button_Click(object sender, EventArgs e)
        {
            serialPort.PortName = "COM13";
            serialPort.BaudRate = 9600;
            serialPort.NewLine = "\n";

            try
            {
                serialPort.Open();
                serialPort.WriteLine("set_on,3,240");
                groupBox2.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            int n = msg_listbox.Items.Add("Connection established...");
            msg_listbox.SelectedIndex = n;
            msg_listbox.ClearSelected();

            runThread.Set();
            
        }
        private void close_serialport()
        {
            try
            {
                if (serialPort.IsOpen == true)
                    serialPort.Close();
            }
            catch (Exception ex)
            { MessageBox.Show(ex.Message, "", MessageBoxButtons.OK, MessageBoxIcon.Error); }

            runThread.Reset();
            int n = msg_listbox.Items.Add("Connection closed.");
            msg_listbox.SelectedIndex = n;
            msg_listbox.ClearSelected();
        }
        private void stop_button_Click(object sender, EventArgs e)
        {
            close_serialport();
        }

        private void button9_Click(object sender, EventArgs e)
        {
            serialPort.WriteLine("fan_on_temph,6,20");
        }

        private void button12_Click(object sender, EventArgs e)
        {
            serialPort.WriteLine("fan_off_temph,6,20");
        }

        private void button10_Click(object sender, EventArgs e)
        {
            serialPort.WriteLine("set_off,6,20");
        }

        private void button13_Click(object sender, EventArgs e)
        {
            serialPort.WriteLine("set_on,6,20");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            about frm = new about();
            frm.ShowDialog();
        }

    }
}
