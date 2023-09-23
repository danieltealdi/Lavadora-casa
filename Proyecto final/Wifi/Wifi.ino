#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>   // Include the SPIFFS library

#include "config.h"  // Sustituir con datos de vuestra red
#include "Server.hpp"
#include "ESP8266_Utils.hpp"

#define RELAY 0 // relay connected to GPIO0

void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT );
  pinMode(RELAY,OUTPUT);
  pinMode(2,INPUT);
  digitalWrite(RELAY, HIGH);
	Serial.begin(115200);
	SPIFFS.begin();

	ConnectWiFi_STA();

	InitServer();

  
}

void loop(void)
{
  if(digitalRead(2)==HIGH){
    digitalWrite(RELAY, HIGH);
    }
}
