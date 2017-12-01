
/*  This code was written by Larry Bonnette.
 *  with help from code gleaned from many sources on the internet.
 *   
 *   
 *   This section explains how charaters are positioned on the OLED display
 *   This is the code used to display text on the OLED
 *   
 *   //Example Display on OLED for "Current Time"
 *   display.setTextAlignment(TEXT_ALIGN_LEFT);------AA
 *   display.setFont(ArialMT_Plain_16);--------------BB
 *   display.drawString(0,0,"Current Time");---------CC
 *   display.setFont(ArialMT_Plain_16);--------------DD
 *   display.drawString(15,20,ODtimd);---------------EE
 *   display.drawString(15,40,ODtimt);---------------FF
 *   display.display();------------------------------GG
 *   delay(5000);
 *   display.clear();--------------------------------HH
 *   
 *   AA Sets up the alignment IE left to right in the example
 *   BB Sets the font and the size of font available font is 10,16 and 24 (Other fonts can be added to the .h file) the font is set to Arial 16
 *   CC Puts a string in the OLED buffer. In the example, "Current Time" is the string and is positioned in the first pixel on the first line 
 *      (0,0,"String") the first "0" is the first pixel. The second "0" is how many pixels down. So, "Current Time" shows up in the upper left hand corner.
 *   DD Changes the font. In the example, the font stays the same at "16". It is in the code to remind me that fonts can be changed at anytime.
 *   EE Puts a string in the OLED buffer. In this case, the date retrieved from the weather station is placed at the 15th pixel from the left of the screen and 20 pixels down.
 *   FF Puts a string in the OLED buffer. In this case, the time retrieved from the weather station is placed at the 15th pixel from the left of the screen and 40 pixels down.
 *   GG Tells the OLED to display the data in the buffer onto the OLED screen.
 *   HH Clears the screen
 *   
 *  ********************************************************************************************************************************* 
 *   This section explains how data is extracted from information sent from the WeatherPlus board.
 *   
 *   Data from the WeatherPlus board looks like this:
 *   
 *   {"variables": {"EnglishOrMetric": 0, "AirQualitySensor": 0, "ESP8266HeapSize": 31160, "OurWeatherTime": "09/10/2017 10:16:20", "FullDataString": 
 *   "25.10,51.30,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,09/10/2017 10:16:20,,0,-1,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,V:1,WXLMB ,", 
 *   "FirmwareVersion": "023", "IndoorTemperature":  0.00, "BarometricPressure":  0.00, "Altitude":  0.00, "OutdoorTemperature": 25.10, "OutdoorHumidity": 51.30, "CurrentWindSpeed":  0.00, "CurrentWindGust":  
 *   0.00, "CurrentWindDirection":  0.00, "RainTotal":  0.00, "WindSpeedMin":  0.00, "WindSpeedMax":  0.00, "WindGustMin":  0.00, "WindGustMax":  0.00, "WindDirectionMin":  0.00, 
 *   "WindDirectionMax":  0.00}, "id": "1", "name": "OurWeather", "connected": true}<--------------------------------------This is the final charater sent from the WeatherPlus board.
 *    
 *   This is the code used to extract components like "Temperature" and "Wind Speed" etc. The example
 *   below extracts the time and date.
 *    
 *   // Extract and display the current time from the recieved data
 *   String line = client.readStringUntil('}');----------------------------------------AA
 *   int tim = line.indexOf(String("OurWeatherTime"));---------------------------------BB
 *   String ODtimd = line.substring(tim + 18,tim + 28);--------------------------------CC
 *   String ODtimt = line.substring(tim + 29,tim + 37);--------------------------------DD
 *   Serial.println("The Date is: " + ODtimd);-----------------------------------------EE
 *   Serial.println("The Time is: " + ODtimt);-----------------------------------------FF
 *   
 *   AA reads the data comming from the WeatherPlus board using the function call "client.readStringUntil". The function reads all of the text
 *      comming from the WeatherPlus board and places it into a variable "line" until it gets to a "}" charater. At this charater the function stops reading. 
 *      Now, "line" contains all of the data drom the WeatherPlus board (as shown above). 
 *   BB Now we will get the "time" infromation from this data. This is done by searching the data in the variable "line" and getting an index number that represents the position
 *      in the data of the word "OurWeatherTime" We save that index number into a variable called "tim"
 *   CC Now that we have a number that represents the position of the word "OurWeatherTime". (look at the data above) you can see that if you count charaters starting 
 *      from zero at the "O" in "OurWeatherTime" that the date starts at the 18th charater. so we start saving data from tim + 18 to tim + 28 into a variable "ODtimd"
 *      "ODtimd" now contains the date extracted from the data send from the WeatherPlus board.
 *   DD We do the same as above to get the "time". The time starts a tim + 29 until tim + 37. We store the time in "ODtimt"
 *   EE Prints the date onto the serial monitor
 *   FF Prints the date onto the serial monitor.
 *   
 *   The OLED example (at the top of the page) uses the ODtimd and ODtimt variables and displays them on the OLED
 *   
 * ******************************************************************************************************************************************
 */
    
//------------------------------------------------------------------------------------
// Libraries Needed For This Project
//------------------------------------------------------------------------------------
  #include <Wire.h>           // To Be Able To Use Sensors Like IMU9050
  #include <EEPROM.h>         // To Be Able To Store & Read Data Between Power Off
  #include <ESP8266WiFi.h>    // The Basic Function Of The ESP NOD MCU
  #include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
//  #include "SH1106.h" alis for `#include "SH1106Wire.h"`
// Initialize the OLED display using Wire library (I2C address, Data pin, Clock pin)
    SSD1306  display(0x3c, 4, 5);
//    SH1106 display(0x3c, 4, 5);
//------------------------------------------------------------------------------------
// WIFI Module Role & Port
//------------------------------------------------------------------------------------
// This WIFI Module Code Works As A Client that Will Connect To A WeatherPlus (server) board With an TCP/IP address of 192.168.0.220

  int ServerPort  = 80;
  //const char* host = "192.168.1.77"; // WeatherPlus board address
  const char* host = "192.168.0.196"; // WeatherPlus board address
  
  WiFiClient      client; //initiate Wi-Fi client as "client"
//====================================================================================

  void setup() 
  {

  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

    // Setting The Serial Port ----------------------------------------------
    delay(1000);
    Serial.begin(115200);           // Serial port set to 115200 baud
    Serial.println("Begin initialization of WeatherClient");

    // Starting To Connect --------------------------------------------------
    if(WiFi.status() == WL_CONNECTED)
    {
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      delay(50);
    }

    // The WeatherClient loads the ssid and password for the wireless connection
    
    WiFi.mode(WIFI_STA);            // To Avoid Broadcasting An SSID

    WiFi.begin("Your SSID", "Your WiFi password");      // The wireless SSID That We Want To Connect to and a password
    WiFi.config(IPAddress(192,168,0,221), IPAddress(192,168,0,1), IPAddress(255,255,255,0)); //Define a static client ip address (not using DHCP)


    // Printing Message for the user that a connetion is in progress
    Serial.println("**** Connecting To " + WiFi.SSID() + " ****");
    
    // OLED display connection details
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Connecting To");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 25,WiFi.SSID());
    display.display();
    delay(5000);


    Serial.println("**** WeatherClient Connected ****");

    // Printing IP Address --------------------------------------------------
    Serial.println("Connected To      : " + String(WiFi.SSID()));
    Serial.println("Signal Strenght   : " + String(WiFi.RSSI()) + " dBm");
    Serial.print  ("Server IP Address : ");
    Serial.println(host);
    Serial.print  ("Server Port Num   : ");
    Serial.println(ServerPort);
    // Printing MAC Address
    Serial.print  ("Device MC Address : ");
    Serial.println(String(WiFi.macAddress()));
    // Printing IP Address
    Serial.print  ("Device IP Address : ");
    Serial.println(WiFi.localIP());

  }
    


//====================================================================================
  
  void loop()
  {
  // clear the display
  display.clear();
  delay(1000); //Wait 1 Seconds 
  Serial.println("In Loop");

// Make a connection to the weather server
   if (client.connect(host, ServerPort)) {

// Send a query for weather information
client.print(String("GET /") + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             "Connection: close\r\n" +
             "\r\n"
            );

  }
  else {
    Serial.println("Connection Failed");
    // OLED display connection details
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Connection Fail");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 25,"Retrying Connection");
    display.display();
    delay(5000); // If connection to server failed give it a few seconds
    display.clear();
  }
// If connection succeded then read the weather information into a buffer. (called line)
while (client.connected())
{
  if (client.available())
  {
    String line = client.readStringUntil('}'); // the last charater in the data is a "}" so stop reading when we get this.
    Serial.println(line); // for testing this will print everything
// Display Information on ther serial terminal
// Extract and display the current time from the recieved data
    int tim = line.indexOf(String("OurWeatherTime")); // search for the time
    String ODtimd = line.substring(tim + 18,tim + 28); //when found extract the 18,37 date
    String ODtimt = line.substring(tim + 29,tim + 37); //when found extract the 29,37 time
    Serial.println("The Date is: " + ODtimd); //Display the date
    Serial.println("The Time is: " + ODtimt); //Display the time

// Extract and display the current outdoor temperature.
    int temp = line.indexOf(String("OutdoorTemperature")); // search for temp
    String ODtemp = line.substring(temp + 21,temp + 26); //when found extract the temp
    int tempint = ODtemp.toInt(); // convert to int so we can convert "C" to Faranheight 
    tempint = tempint*1.8+32; // convert "C" to "F"
    Serial.println("The Outdoor Temp is: " + String(tempint)); // Display the temp in "F"


// Extract and display the current outdoor humidity.
    int humi = line.indexOf(String("OutdoorHumidity")); // Search for Humidity
    String ODhumi = line.substring(humi + 18,humi + 23); // when found extract the humidity
    Serial.println("The Outdoor Humidity is: " + ODhumi); // Display  the Humidity

// Extract and display the current indoor temperature.
    int tempi = line.indexOf(String("IndoorTemperature")); // Search for indoor Temp
    String IDtemp = line.substring(tempi + 20,tempi + 25); // when found extract the indoor Temp
    int idtempint = IDtemp.toInt(); // convert to int so we can convert "C" to Faranheight 
    idtempint = idtempint*1.8+32; // convert "C" to "F"
    Serial.println("The Indoor Temperature is: " + String(idtempint)); // Display  the indoor Temp

// Extract and display the current barometric pressure.
    int prsur = line.indexOf(String("BarometricPressure")); // Search for pressure
    String IDpress = line.substring(prsur + 21,prsur + 30); // when found extract the pressure
    float pressint = IDpress.toInt(); // convert to int so we can convert to inch mercury 
    pressint = (pressint/33.863)/100; // convert hga to inches mercury
    Serial.println("The Barometric Pressure is: " + IDpress); // Display  the pressure in hGa
    Serial.println("The Barometric Pressure is: " + String(pressint)); // Display  the pressure in inches of mercury

// Extract and display the current wind speed
    int wspeed = line.indexOf(String("CurrentWindSpeed")); // Search for Wind Speed
    int wdir = line.indexOf(String("CurrentWindDirection")); // Search for Wind direction
    String ODspeed = line.substring(wspeed + 19,wspeed + 24); // when found extract the Wind Speed
    int ODspeedint = ODspeed.toInt();
    ODspeedint = ODspeedint * 0.62137119223733;
    ODspeed = String(ODspeedint);
    String ODdir = line.substring(wdir + 23,wdir + 28); // when found extract the Wind direction
    int INTdir = ODdir.toInt();
    ODdir = getWdir(INTdir);
    Serial.println("The Wind Speed is: " + ODspeed + " MPH"); // Display  the Wind Speed
    Serial.println("The Wind Direction is: " + ODdir); // Display  the Wind Direction
    
    client.stop(); // disconnect from server so we don't keep displaying the info.

//    int t = 5;
//    while(t>0){   // Display information on OLED 5 (1 minute 15 seconds) times before getting data from WeatherPlus again. 
    
    //Display on OLED current Time
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Current Time");
    display.setFont(ArialMT_Plain_16);
    display.drawString(15,20,ODtimd); // print out the date
    display.drawString(15,40,ODtimt); // print out the time
    display.display();
    delay(5000);
    display.clear();
    
    // Display on OLED Temperature   
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Outdoor Temp");
    display.setFont(ArialMT_Plain_24);
    display.drawString(30, 25,String(tempint) + " F");
    display.display();
    delay(5000);
    display.clear();
    
    // Display on OLED Humidity   
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Humidity");
    display.setFont(ArialMT_Plain_24);
    display.drawString(15,25,ODhumi + " %");
    display.display();
    delay(5000);
    display.clear();

    // Display on OLED indoor Temperature   
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Indoor Temp");
    display.setFont(ArialMT_Plain_24);
    display.drawString(30, 25,String(idtempint) + " F");
    display.display();
    delay(5000);
    display.clear();

    // Display on OLED Barometric Pressure   
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Pressure");
    display.setFont(ArialMT_Plain_24);
    display.drawString(30, 25,String(pressint) + " ");
    display.display();
    delay(5000);
    display.clear();

    // Display on OLED Wind Speed   
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,0,"Wind");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,15,ODdir);
    display.setFont(ArialMT_Plain_24);
    display.drawString(15,35,ODspeed + " MPH");
    display.display();
    delay(5000);
    display.clear();
//    --t;
//    }
  }
}
//delay(30000); // Wait 30 seconds and do it all again
  }

//====================================================================================
// Routine to convert Wind direction from numbers to text (returns text)
//====================================================================================
  String getWdir(int zz)
  {
    if(zz >= 0  && zz < 22) {
        return("North");}
        
        else if (zz >= 22  && zz < 45) {
        return("North North East");}
        
        else if (zz >= 45  && zz < 67) {
        return("North East");}

        else if (zz >= 67  && zz < 90) {
        return("East North East");}

        else if (zz >= 90 && zz < 112)  {
        return("East");}

        else if (zz >= 112 && zz < 135)  {
        return("East South East");}

        else if (zz >= 135 && zz < 157)  {
        return("South East");}

        else if (zz >= 157 && zz < 180)  {
        return("South South East");}

        else if (zz >= 180 && zz < 202)  {
        return("South");}

        else if (zz >= 202 && zz < 225)  {
        return("South South West");}

        else if (zz >= 225 && zz < 247)  {
        return("South West");}

        else if (zz >= 247 && zz < 270)  {
        return("West South West");}

        else if (zz >= 270 && zz < 292)  {
        return("West");}

        else if (zz >= 292 && zz < 315)  {
        return("West North West");}

        else if (zz >= 315 && zz < 337)  {
        return("North West");}

        else if (zz >= 337 && zz < 359)  {
        return("North North West");}
        return("Unknown");
    }

//====================================================================================


