#include <TM1650.h>
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
 *  lib: https://github.com/maxint-rd/TM16xx 
 */ 

TM1650 module(13, 12, digits);    // data, clock, 4 digits   D6:GPIO12  D7:GPIO13

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW); //LOW:ON HIGH:OFF

  Serial.begin(74880);
  Serial.println("\nNTP xy-clock");
  
  module.setDisplayToString("GAIA");
  delay(1000);
  int cnt=0;
  module.clearDisplay();
  
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
  module.setupDisplay(true, 5);   // sometimes noise on the line may change the intensity level
  switch (digits) {
    case 4: 
      nTime = timeClient.getHours() *100 + timeClient.getMinutes();
      //Serial.println(nTime);
      module.setDisplayToDecNumber(nTime, _BV(2)|_BV(3)); // display dots
      break;
  }
  delay(500);
  
  module.setDisplayToDecNumber(nTime, 0); // clear the dots
  delay(500);
}
