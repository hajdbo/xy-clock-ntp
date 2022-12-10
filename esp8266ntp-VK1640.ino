// board: 8266 NodeMCU 1.0 (esp-12e module) / COM 115200


#include <TM1640.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "***REMOVED***";
const char *password = "***REMOVED***";

const long utcOffsetInSeconds = 28800;  // GMT+8 SG
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

byte digits = 4; // max:11     hh:mm 4     hh:mm ss 6
int nTime = 0;
String mystr;
/*
 *  11 DIGITS:
 *  16:49 sont les 4 premiers digits
 *  88[%RH] sont les 3 digits suivants (humidity)
 *  0324  sont les 4 digits suivants (date / temp)
 *  
 *  lib: https://github.com/maxint-rd/TM16xx 
 */ 

// Define a 4-digit display module. Pin suggestions:
// ESP8266 (Wemos D1): data pin 5 (D1), clock pin 4 (D2)
TM1640 module(5, 4, digits);    // data, clock, 6 digits


/*
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
const int sclPin = D5;
const int sdaPin = D6;
Adafruit_BME280 bme;
unsigned long bme280delay;
*/



void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW); //LOW:ON HIGH:OFF

  Serial.begin(115200);
  Serial.println("\nNTP + BME280 Wifi");
  
  module.setDisplayToString("GAIA");
  delay(1000);
  int cnt=0;
  module.clearDisplay();
  
  /*Wire.begin(sdaPin, sclPin); // I2C for BME280
  unsigned status;
  status = bme.begin(0x76, &Wire);
  if (!status) {
    while (1) delay(10);
  }*/
  
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    cnt++;
    module.setDisplayToDecNumber(cnt, 0);
  }
  timeClient.begin();
  Serial.println("Setup done");
  digitalWrite(LED_BUILTIN, HIGH); //LOW:ON HIGH:OFF
}

void loop()
{
  timeClient.update();
  /*
  //temp = bme.readTemperature();
  //hum = bme.readHumidity();
  Serial.print("Temperature= ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  Serial.print("Humidity= ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  Serial.print("Pressure= ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
*/
  //digitalWrite(LED_BUILTIN, LOW); 
  module.setupDisplay(true, 5);   // sometimes noise on the line may change the intensity level
  switch (digits) {
    case 4: 
      nTime = timeClient.getHours() *100 + timeClient.getMinutes();
      module.setDisplayToDecNumber(nTime, _BV(2)|_BV(3)); // display dots
      break;
    case 6:
      nTime = timeClient.getHours() *10000 + timeClient.getMinutes()*100 + timeClient.getSeconds();
      module.setDisplayToDecNumber(nTime, _BV(4)|_BV(5)); // display dots
      break;
    case 11:
      //mystr = timeClient.getHours() *100 + timeClient.getMinutes() + int(temp);
      break;
  }
  delay(500);
  
  //digitalWrite(LED_BUILTIN, HIGH); 
  module.setDisplayToDecNumber(nTime, 0); // clear the dots
  delay(500);
}

/*
void loop()
{
  module.clearDisplay();
  for(byte nPos=0; nPos<11; nPos++)     // 16 positions (columns) on TM1640, most others have less
  {
    for(int nVal=0; nVal<=0xFF; nVal=nVal*2+1)     // max 8 segments on TM1638QYF module.
    {
      module.setSegments(nVal, nPos);   // note: you can use setSegments16() when module class supports > 8 segments (TM1638, TM1668)
      delay(100);
    }
  }
  delay(1000);
}
*/
