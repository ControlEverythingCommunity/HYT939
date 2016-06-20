// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// HYT939
// This code is designed to work with the HYT939_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Humidity?sku=HYT939_I2CS#tabs-0-product_tabset-2

#include <application.h>
#include <spark_wiring_i2c.h>

// HYT939 I2C address is 0x28(40)
#define Addr 0x28

double humidity = 0.0, cTemp = 0.0, fTemp = 0.0;
void setup() 
{
    // Set variable
    Particle.variable("i2cdevice", "HYT939");
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
    unsigned int data[4];
    // Start I2C transmission
    Wire.beginTransmission(Addr);
    // Send normal mode command  
    Wire.write(0x80);
    // Stop I2C transmission
    Wire.endTransmission();
    delay(300);
  
    // Request 4 bytes of data from the device
    Wire.requestFrom(Addr, 4);

    // Read 4 bytes of data
    // humidity msb, humidity lsb, temp msb, temp lsb
    if(Wire.available() == 4)
    {
        data[0] = Wire.read();
        data[1] = Wire.read();
        data[2] = Wire.read();
        data[3] = Wire.read();
    }    
    
    // Convert the data to 14-bits
    humidity = (((data[0] & 0x3F) * 256.0) +  data[1]) * (100.0 / 16383.0);
    cTemp = (((data[2] * 256.0) + (data[3] & 0xFC)) / 4) * (165.0 / 16383.0) - 40;
    fTemp = (cTemp * 1.8) + 32;
        
    // Output data to dashboard
    Particle.publish("Relative Humidity is      :  ", String(humidity));
    delay(1000);
    Particle.publish("Temperature in Celsius    :  ", String(cTemp));
    delay(1000);
    Particle.publish("Temperature in Fahrenheit :  ", String(fTemp));
    delay(1000);
}
