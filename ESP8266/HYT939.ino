// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// HYT939
// This code is designed to work with the HYT939_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Humidity?sku=HYT939_I2CS#tabs-0-product_tabset-2

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include<Wire.h>

// HYT939 I2C address is 0x28(40)
#define Addr 0x28

const char* ssid = "your ssid netwotk";
const char* password = "your password";
float fTemp, cTemp, humidity;

ESP8266WebServer server(80);

void handleroot()
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
  if (Wire.available() == 4)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();

    // Convert the data to 14-bits
    humidity = (((data[0] & 0x3F) * 256.0) +  data[1]) * (100.0 / 16383.0);
    cTemp = (((data[2] * 256.0) + (data[3] & 0xFC)) / 4) * (165.0 / 16383.0) - 40;
    fTemp = (cTemp * 1.8) + 32;

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

    // Output data to web server
    server.sendContent
    ("<html><head><meta http-equiv='refresh' content='5'</meta>"
     "<h1 style=text-align:center;font-size:300%;color:blue;font-family:britannic bold;>CONTROL EVERYTHING</h1>"
     "<h3 style=text-align:center;font-family:courier new;><a href=http://www.controleverything.com/ target=_blank>www.controleverything.com</a></h3><hr>"
     "<h2 style=text-align:center;font-family:tahoma;><a href= https://www.controleverything.com/content/Humidity?sku=HYT939_I2CS#tabs-0-product_tabset-2/ \n"
     "target=_blank>HYT939 Sensor I2C Mini Module</a></h2>");
    server.sendContent
    ("<h3 style=text-align:center;font-family:tahoma;>Relative Humidity = " + String(humidity) + " %RH");
    server.sendContent
    ("<h3 style=text-align:center;font-family:tahoma;>Temperature in Celsius = " + String(cTemp) + " C");
    server.sendContent
    ("<h3 style=text-align:center;font-family:tahoma;>Temperature in Fahrenheit = " + String(fTemp) + " F");
  }
}

void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin(2, 14);
  // Initialise serial communication, set baud rate = 115200
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  // Get the IP address of ESP8266
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleroot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}

