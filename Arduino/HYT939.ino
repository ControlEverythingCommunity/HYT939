// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// HYT939
// This code is designed to work with the HYT939_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Humidity?sku=HYT939_I2CS#tabs-0-product_tabset-2

#include<Wire.h>

// HYT939 I2C address is 0x28(40)
#define Addr 0x28

void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial Communication
  Serial.begin(9600);
  delay(300);
}

void loop()
{
  unsigned int data[4];
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Send normal mode command
  Wire.write(0x80);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(300);
  
  // Request 4 bytes of data
  Wire.requestFrom(Addr, 4);

  // Read 4 bytes of data
  // humidity msb, humidity lsb, temp msb, temp lsb
  if(Wire.available() == 4)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    
    // Convert the data to 14-bits
    float humidity = (((data[0] & 0x3F) * 256.0) +  data[1]) * (100.0 / 16383.0);
    float cTemp = (((data[2] * 256.0) + (data[3] & 0xFC)) / 4) * (165.0 / 16383.0) - 40;
    float fTemp = (cTemp * 1.8) + 32;
    
    // Output data to serial monitor
    Serial.print("Relative Humidity : ");
    Serial.print(humidity);
    Serial.println(" %RH");
    Serial.print("Temperature in Celsius : ");
    Serial.print(cTemp);
    Serial.println(" C");
    Serial.print("Temperature in Fahrenheit : ");
    Serial.print(fTemp);
    Serial.println(" F");
  }
  delay(300);
}
