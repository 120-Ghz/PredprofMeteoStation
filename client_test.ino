#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#ifndef STASSID
#define STASSID "iPhone (Константин)"
#define STAPSK  "123456789"
#endif

#define SensorPin D7

const char* ssid = STASSID;
const char* password = STAPSK;
const String ServerIP = "http://172.20.10.10/send-data-light";

int volt;

WiFiClient client;
HTTPClient http;

void setup(void) {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
      //Declare object of class HTTPClient
 
  http.begin(client, "http://172.20.10.10/send-data-light");      //Specify request destination
  http.addHeader("Content-Type", "text/plain");
  
    pinMode(SensorPin, INPUT);
}

void loop(void) {
  int volt = analogRead(SensorPin);
  int httpCode = http.POST(String(volt));   //Send the request
    String payload = http.getString();                  //Get the response payload
 
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);
    delay(100);
}
