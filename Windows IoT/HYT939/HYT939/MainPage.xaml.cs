// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Threading;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using Windows.Devices.I2c;

namespace HYT939
{
    struct HumdTemp
    {
        public double RH;
        public double C;
        public double F;
    }; /// <summary>
       /// App that reads data over I2C from an HYT939, Humidity and Temperature Sensor
       /// </summary>

    public sealed partial class MainPage : Page
    {
        private const byte HUMTEMP_I2C_ADDR = 0x28;           //	I2C address of the HYT939
        private const byte HUMTEMP_REG_HUMIDITY = 0x00;              //	Humidity data register


        private I2cDevice I2CHumtemp;
        private Timer periodicTimer;

        public MainPage()
        {
            this.InitializeComponent();

            //	Register for the unloaded event so we can clean up upon exit
            Unloaded += MainPage_Unloaded;

            //	Initialize the I2C bus, Humidity and Temperature Sensor, and timer
            InitI2CHumtemp();
        }

        private async void InitI2CHumtemp()
        {
            string aqs = I2cDevice.GetDeviceSelector();             //	Get a selector string that will return all I2C controllers on the system
            var dis = await DeviceInformation.FindAllAsync(aqs);    //	Find the I2C bus controller device with our selector string
            if (dis.Count == 0)
            {
                Text_Status.Text = "No I2C controllers were found on the system";
                return;
            }

            var settings = new I2cConnectionSettings(HUMTEMP_I2C_ADDR);
            settings.BusSpeed = I2cBusSpeed.FastMode;
            I2CHumtemp = await I2cDevice.FromIdAsync(dis[0].Id, settings);    // Create an I2C Device with our selected bus controller and I2C settings
            if (I2CHumtemp == null)
            {
                Text_Status.Text = string.Format(
                    "Slave address {0} on I2C Controller {1} is currently in use by " +
                    "another application. Please ensure that no other applications are using I2C.",
                    settings.SlaveAddress,
                    dis[0].Id);
                return;
            }

            /*
				Initialize the Humidity and Temperature Sensor
				For this device, we create 1-byte write buffer
				The byte is the content that we want to write to
			*/
            byte[] WriteBuf_ComByte = new byte[] { 0x80 };          //	0x80 Ends Command Mode and transits to Normal Operation Mode

            //	Write the register settings
            try
            {
                I2CHumtemp.Write(WriteBuf_ComByte);
            }
            // If the write fails display the error and stop running
            catch (Exception ex)
            {
                Text_Status.Text = "Failed to communicate with device: " + ex.Message;
                return;
            }

            //	Create a timer to read data every 100ms
            periodicTimer = new Timer(this.TimerCallback, null, 0, 100);
        }

        private void MainPage_Unloaded(object sender, object args)
        {
            //	Cleanup
            I2CHumtemp.Dispose();
        }

        private void TimerCallback(object state)
        {
            string rhText, cText, fText;
            string addressText, statusText;

            //	Read and format Humidity and Temperature data
            try
            {
                HumdTemp HUMTEMP = ReadI2CHumtemp();
                addressText = "I2C Address of the Humidity and Temperature Sensor HYT939: 0x28";
                rhText = String.Format("Relative Humidity (%RH): {0:F2}", HUMTEMP.RH);
                cText = String.Format("Temperature in Celsius (°C): {0:F2}", HUMTEMP.C);
                fText = String.Format("Temperature in Fahrenheit (°F): {0:F2}", HUMTEMP.F);
                statusText = "Status: Running";
            }
            catch (Exception ex)
            {
                rhText = "Relative Humidity: Error";
                cText = "Temperature in Celsius: Error";
                fText = "Temperature in Fahrenheit: Error";
                statusText = "Failed to read from Humidity and Temperature Sensor: " + ex.Message;
            }

            //	UI updates must be invoked on the UI thread
            var task = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                Text_Relative_Humidity.Text = rhText;
                Text_Temperature_in_Celsius.Text = cText;
                Text_Temperature_in_Fahrenheit.Text = fText;
                Text_Status.Text = statusText;
            });
        }

        private HumdTemp ReadI2CHumtemp()
        {
            byte[] RegAddrBuf = new byte[] { HUMTEMP_REG_HUMIDITY };     //	Read data from the register address
            byte[] ReadBuf = new byte[4];                       //	We read 4 bytes sequentially to get all 2 two-byte humidity and temperature registers in one read

            /*
				Read from the Humidity and Temperature Sensor 
				We call WriteRead() so we first write the address of the Relative Humidity I2C register, then read Temperature data
				register
			*/
            I2CHumtemp.WriteRead(RegAddrBuf, ReadBuf);

	    // In order to get the raw 16-bit data values, we need to concatenate two 8-bit bytes from the I2C read for each axis.
            ushort HUMTEMPRawRH = (ushort)((ReadBuf[0] & 0x3F) * 256);
            HUMTEMPRawRH |= (ushort)(ReadBuf[1] & 0xFF);
            ushort HUMTEMPRawC = (ushort)((ReadBuf[2] & 0xFF) * 256);
            HUMTEMPRawRH |= (ushort)(ReadBuf[3] & 0xFC);

            // Conversions using formulas provided
            double humidity = (HUMTEMPRawRH * (100.0 / 16383.0));
            double ctemp = ((HUMTEMPRawC / 4.0) * (165.0 / 16383.0)) - 40.0;
            double ftemp = (ctemp * 1.8) + 32.0;

            HumdTemp humtmp;
            humtmp.RH = humidity;
            humtmp.C = ctemp;
            humtmp.F = ftemp;

            return humtmp;
        }
    }
}
