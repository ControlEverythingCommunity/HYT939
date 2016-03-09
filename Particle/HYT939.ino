// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// HYT939
// This code is designed to work with the HYT939_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Humidity?sku=HYT939_I2CS#tabs-0-product_tabset-2

#include <application.h>
#include <spark_wiring_i2c.h>

// HYT939 I2C address is 0x28(40)
#define Addr 0x28

float humidity = 0, cTemp = 0, fTemp = 0;
void setup() 
{
    // Set variable
    Particle.variable("i2cdevice", "SHT31");
    Particle.variable("cTemp", cTemp);
  Particle.variable("humidity", humidity);
  
    // Initialise I2C communication as MASTER 
    Wire.begin();
    // Initialise Serial Communication
    Serial.begin(9600);
    delay(300);
}

void loop() 
{
    // Begin transmission with given device on I2C bus
    Wire.beginTransmission(Addr);
    // Send normal mode command  
    Wire.write(0x80);
    // Stop I2C transmission on the device
    Wire.endTransmission();
    delay(300);
  
    // Request 4 bytes of data from the device
    Wire.requestFrom(Addr, 4);

    // Read 4 bytes of data
    // humidity msb, humidity lsb, temp msb, temp lsb
    if(Wire.available() == 4)
    {
        int hMSB = Wire.read();
        int hLSB = Wire.read();
        int tMSB = Wire.read();
        int tLSB = Wire.read();
        
        delay(300);
        // Convert the data to 14-bits
        humidity = (((hMSB & 0x3F)  256.0) +  hLSB)  (100.0 / 16383.0);
        cTemp = (((tMSB  256.0) + (tLSB & 0xFC)) / 4)  (165.0 / 16383.0) - 40;
        fTemp = (cTemp * 1.8) + 32;
        
        // Output data to dashboard
        Particle.publish("Relative Humidity is      :  ", humidity);
        Particle.publish("Temperature in Celsius    :  ", cTemp);
        Particle.publish("Temperature in Farhenheit :  ", fTemp);
    }
}

