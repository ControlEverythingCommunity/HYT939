// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// HYT939
// This code is designed to work with the HYT939_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Humidity?sku=HYT939_I2CS#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() 
{
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, HYT939 I2C address is 0x28(40)
	ioctl(file, I2C_SLAVE, 0x28);
	
	// Send normal mode command 
	char config[1] = {0};
	config[0] = 0x80;
	write(file, config, 1);
	usleep(100000);
	
	// Read 4 bytes of data
	// humidity msb, humidity lsb, temp msb, temp lsb
	char data[4] = {0};
	if(read(file, data, 4) != 4)
	{
		printf("Error : Input/output Error \n");
	}
	else
	{
		// Convert the data
		int hum = ((data[0] & 0x3F)* 256 + (data[1] & 0xFF));
		int temp = ((data[2] & 0xFF)* 256 + (data[3] & 0xFC)) / 4;
        	double humidity = (hum) * (100.0 / 16383.0);
		double cTemp = (temp) * (165.0 / 16383.0) - 40;
		double fTemp = (cTemp * 1.8 ) + 32;
		
		// Output data to screen
		printf("Temprature in Celsius : %.2f C \n", cTemp);
		printf("Temprature in Fahrenheit : %.2f F \n", fTemp);
		printf("Relative humidity is : %.2f RH \n", humidity);
	}
}
