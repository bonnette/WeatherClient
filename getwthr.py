
#!/usr/bin/env python
# a routine that tests the Pi with the Switchdoc weather board and rack.
# it reads wind speed, direction (off of the weather rack) it reads temperature from
# the BMP280 on the weather board  and time and date from the DS3231 on the weather board
# it reads outdoor temperature from the AM2315 on the I2C buss
# the drivers for each component (BMP280, DS3231, WeatherRack) shoud be in 
# the same directory as the executable.
#
# Additionally the routine collects the data and transfers it into a  file (wthrdata.dat)
# and copies the file into the html file directory on the pi so it can be used on a web page.

import sys
import shutil
import time
import datetime
import random
import Adafruit_BMP.BMP280 as BMP280 # Indoor temperature sensor on the weather board
import SDL_DS3231 # Real Time clock on the weather board
import SDL_Pi_WeatherRack # Sensors on the weather rack (wind speed, direction, and rain guage)

anenometerPin = 26 # this is physical pin 37 on the pi
rainPin = 21 # This is physical pin 40 on the pi
SDL_MODE_INTERNAL_AD = 0
SDL_MODE_I2C_ADS1015 = 1

#sample mode means return immediately.  THe wind speed is averaged at sampleTime or when you ask, whichever is longer
SDL_MODE_SAMPLE = 0
#Delay mode means to wait for sampleTime and the average after that time.
SDL_MODE_DELAY = 1
totalRain = 0

from tentacle_pi.AM2315 import AM2315 # driver for Outdoor temperature and humidity guage
am = AM2315(0x5c,"/dev/i2c-1")
sensor = BMP280.BMP280()
temperature, humidity, crc_check = am.sense()
ds3231 = SDL_DS3231.SDL_DS3231(1, 0x68)
weatherStation = SDL_Pi_WeatherRack.SDL_Pi_WeatherRack(anenometerPin, rainPin, 0,0,SDL_MODE_I2C_ADS1015)

weatherStation.setWindMode(SDL_MODE_SAMPLE, 5.0)
#weatherStation.setWindMode(SDL_MODE_DELAY, 5.0)

currentWindSpeed = weatherStation.current_wind_speed()/1.609

tm = ds3231.read_datetime()   
it = sensor.read_temperature()
ot = temperature
hum = humidity
# The AM2315 sometimes does not answer up even after being asked tiwce. So we use humidity as
# an indicator to determine if the AM2315 answered. If it did. The humidity should not be 0
# If the humidity is 0 we ask again and again (not to exceed 10 times) We give up when zz = 9  
zz=0
while ((hum == 0) or (ot < -100.0)):
#while hum == 0:
	if zz < 9:
		time.sleep(1.5)
		zz=zz+1
		temperature, humidity, crc_check = am.sense() #wake up the AM2315 again
		ot = temperature
		hum = humidity
		print "humidity still 0"
	else:
		break

ps = sensor.read_pressure()/1000
ws = weatherStation.current_wind_speed()/1.609 # test wind speed
wg = weatherStation.get_wind_gust()/1.609
wd = weatherStation.current_wind_direction()
time.sleep(5.0) # must be five or above 
ws = weatherStation.current_wind_speed()/1.609 # test wind speed again
wg = weatherStation.get_wind_gust()/1.609 # test wind gust again
totalRain = weatherStation.get_current_rain_total()/25.4
print "Time = %s" % tm
print "Indoor Temp = %0.1f *C" % it
print "Outdoor Temp = %0.1f *C" % ot
print "Humidity = %0.1f" % hum
print "Pressure = %0.2f KPa" % ps
print "Wind Speed= %0.2f MPH" % ws
print "Wind Gust= %0.2f MPH" % wg
print "Wind Direction= %0.2f Degrees" % wd
print "Total Rain= %0.2f in" % totalRain
print "crc: %i" % crc_check
print " "



# Open the weather data file and populate with data 
file = open("wthrdata.dat","w")
file.write('{"FullDataString": "%04.1f,%0.1f,%04.1f,%s,50.63,%04.1f,%04.1f,%05.1f,%0.2f,3.33,6.98,6.30,11.21,90.00,135.00,0,%s,,0,-1,4.04,-54.00,4.92,93.20,5.15,24.00,0.00,0.00,0.00,0.00,0.00,0.00,V:1,WXLMB ,", "id": "1", "name": "OurWeather", "connected": true}' % (ot,hum,it,sensor.read_pressure(),ws,wg,wd,totalRain,tm))
file.close() # close the weather data file

# Print the weather data file contents on the terminal screen
file = open("wthrdata.dat","r")
print file.read()
file.close()

# Copy the Weather data file to the apache HTML directory
shutil.copy2('./wthrdata.dat', '/var/www/html/wthrdata.dat')
