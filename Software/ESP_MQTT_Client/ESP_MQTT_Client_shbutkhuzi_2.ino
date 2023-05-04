#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <PZEM004Tv30.h>
#include "time.h"

PZEM004Tv30 pzem(Serial2, 16, 17);

#define RELAY 26

// MQTT Broker
const char *mqtt_broker = "smartpowermeter.ge";
const char *customerId = "54321";
const char *topic = "measurement-54321";
const char *mqtt_username = "client";
const char *mqtt_password = "pass";
const int mqtt_port = 707;

// NTP Server
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 10800;
const int   daylightOffset_sec = 3600;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  WiFi.mode(WIFI_STA);

  WiFiManager wm;

  // reset settings - wipe stored credentials for testing, these are stored by the esp library
  //wm.resetSettings();

  WiFiManagerParameter data("CustomerId", "Enter your string here", "default string", 50);
  wm.addParameter(&data);
    
  bool connected = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  if(!connected) {
      Serial.println("Failed to connect");
      ESP.restart();
  }
  else {
      Serial.println("Connected to the WIFI");
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    if (!client.connect(customerId, mqtt_username, mqtt_password)) {
      delay(5000);
    }
    else{
      client.subscribe("relay-54321");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY, LOW);
  } else {
    digitalWrite(RELAY, HIGH);
  }

int state = digitalRead(RELAY);
String stateStr = String(state);
client.publish("relay-state-54321", stateStr.c_str());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  double voltage = pzem.voltage();
  double current = pzem.current();
  double power = pzem.power();
  double energy = pzem.energy();
  double frequency = pzem.frequency();
  double pf = pzem.pf();
  String timeStamp = GetLocalTime();
  Serial.println(timeStamp);

  if(!isnan(voltage) && !isnan(current) && !isnan(power) && !isnan(energy) && !isnan(frequency) && !isnan(pf))
  {
    StaticJsonDocument<200> measurement;

    measurement["Voltage"] = voltage;
    measurement["Current"] = current;
    measurement["Power"] = power;
    measurement["Energy"] = energy;
    measurement["Frequency"] = frequency;
    measurement["PowerFactor"] = pf;
    measurement["TimeStamp"] = timeStamp;

    char json[200];
    serializeJson(measurement, json);

    Serial.println(json);

    client.publish(topic, json);
  }

  delay(1000);
}

///////////////////////////////////////////////////////

String GetLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }

  return FormatDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

String FormatDateTime(int year, int month, int day, int hour, int minute, int second){
  String formattedDateTime = "";
  formattedDateTime = formattedDateTime + year + "-" + GetTwoDigitNumber(month) + "-" + GetTwoDigitNumber(day) + "T" + GetTwoDigitNumber(hour) + ":" + GetTwoDigitNumber(minute) + ":" + GetTwoDigitNumber(second);

  return formattedDateTime;
}

String GetTwoDigitNumber(int num){
  String res = "";

  if(num > 9){
    return res + num;
  }
  else{
    return res + "0" + num;
  }
}
