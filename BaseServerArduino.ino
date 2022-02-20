#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#ifndef STASSID
#define STASSID "iPhone (Константин)"
#define STAPSK  "123456789"
#endif

#define T1pin D12
#define T2pin D13
#define T3pin D14
#define T4pin D10
#define T5pin D11
int profile;
const char* ssid = STASSID;
const char* password = STAPSK;
bool T1 = false;
bool T2 = false;
bool T3 = false;
bool T4 = false;
bool T5 = false;

int LightSensorVoltage = 1024;
int WindSpeed = 1;

bool changed = false;

int defaultSensorVoltage = 300;
int defaultWindSpeed = 0;

ESP8266WebServer server(80);

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/get-data", []() {
    Serial.println(server.arg("plain"));
    String panel = "\"panel\":"+ String(profile == 2) + ",";
    String battery = "\"battery\":" + String(profile != 4) + ",";
    String wind = "\"wind\":" + String(profile == 1 || profile == 3) + ",";
    String sun = "\"sun\":" + String(profile == 1 || profile == 2) + ",";
    String bulb = "\"bulb\":" + String(profile == 4 || profile == 3);
    
    String dat = "{" + panel + battery + wind + sun + bulb + "}";
    server.send(200, "text/plain", dat);
  });

  server.on("/send-data-light", []() {
    Serial.println("nice");
    Serial.println(server.arg("plain"));
    LightSensorVoltage = server.arg("plain").toInt();
    changed = true;
  });

  server.begin();
  pinMode(T1pin, OUTPUT);
  pinMode(T2pin, OUTPUT);
  pinMode(T3pin, OUTPUT);
  pinMode(T4pin, OUTPUT);
  pinMode(T5pin, OUTPUT);
}

void setState(int case_num) {
  profile = case_num;
  Serial.println(case_num);
  switch (case_num) {
    case 1:
      T1 = true;
      T2 = false;
      T3 = false;
      T4 = false; 
      T5 = true;
      break;
    case 2:
      T1 = false;
      T2 = true;
      T3 = false;
      T4 = false; 
      T5 = true;
      break;
    case 3:
      T1 = true;
      T2 = false;
      T3 = false;
      T4 = true; 
      T5 = false;
      break;
    case 4:
      T1 = false;
      T2 = false;
      T3 = true;
      T4 = true; 
      T5 = false;
      break;
  }
  digitalWrite(T1pin, T1 ? HIGH : LOW);
  digitalWrite(T2pin, T2 ? HIGH : LOW);
  digitalWrite(T3pin, T3 ? HIGH : LOW);
  digitalWrite(T4pin, T4 ? HIGH : LOW);
  digitalWrite(T5pin, T5 ? HIGH : LOW);
}

void loop(void) {
  server.handleClient();
  MDNS.update();
if (changed) {
  if (LightSensorVoltage < defaultSensorVoltage) {
    // svetlo
    setState(WindSpeed >= defaultWindSpeed ? 1 : 2);
  } else {
    setState(WindSpeed >= defaultWindSpeed ? 3 : 4);
  }
  changed = false;
}

 
}
