# Distributed with a free-will license.
# Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
# HYT939
# This code is designed to work with the HYT939_I2CS I2C Mini Module available from ControlEverything.com.
# https://www.controleverything.com/content/Humidity?sku=HYT939_I2CS#tabs-0-product_tabset-2

from OmegaExpansion import onionI2C
import time

# Get I2C bus
i2c = onionI2C.OnionI2C()

# HYT939 address, 0x28(40)
#		0x80(128)	Send normal mode
data = [0x80]
i2c.write(0x28, data)

time.sleep(0.5)

# HYT939 address, 0x28(40)
# Read data back from 0x00(00), 4 bytes
# Humidity MSB, Humidity LSB, Temp MSB, Temp LSB
data = i2c.readBytes(0x28, 0x00, 4)

# Convert the data to 14-bits
humidity = ((data[0] & 0x3F) * 256 + data[1]) * (100 / 16383.0)
cTemp = ((data[2] * 256 + (data[3] & 0xFC)) / 4) * (165 / 16383.0) - 40
fTemp = cTemp * 1.8 + 32

# Output data to screen
print "Relative Humidity is : %.2f %%RH" %humidity
print "Temperature in Celsius is : %.2f C" %cTemp
print "Temperature in Fahrenheit is : %.2f F" %fTemp
