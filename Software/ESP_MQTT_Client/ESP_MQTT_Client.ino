#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <PZEM004Tv30.h>
#include "time.h"
#include <Preferences.h>

PZEM004Tv30 pzem(Serial2, 16, 17);

#define RELAY 26

Preferences preferences;

// MQTT Broker
const char *mqtt_broker = "161.35.207.10";
String customerId;
String topic;
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

  String relayState = GetStringValue("Relay", "RelayState");

  if (relayState == "1"){
    digitalWrite(RELAY, LOW);
  }
  else{
    digitalWrite(RELAY, HIGH);
  }
  
  WiFi.mode(WIFI_STA);

  WiFiManager wm;

  //wm.resetSettings();

  WiFiManagerParameter enteredCustomerId("CustomerId", "Customer Id", "", 50);
  wm.addParameter(&enteredCustomerId);

  bool connected = wm.autoConnect("SmartPowerMeter", "password");
  if(!connected) {
      Serial.println("Failed to connect");
      ESP.restart();
  }
  else {
      Serial.println("Connected to the WIFI");
  }

  customerId = GetStringValue("Credentials", "CustomerId");

  String id = String(enteredCustomerId.getValue());
  id.trim();

  if(!id.isEmpty()){
    SaveStringValue("Credentials", "CustomerId", enteredCustomerId.getValue());
    customerId = enteredCustomerId.getValue();
  }
  else{
    if(customerId.isEmpty()){
      wm.resetSettings();
      ESP.restart();
    }
  }

  topic = "measurement-" + customerId;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    if (!client.connect(customerId.c_str(), mqtt_username, mqtt_password)) {
      delay(5000);
    }
    else{
      String relayState = "relay-" + customerId;
      client.subscribe(relayState.c_str());
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY, LOW);
    SaveStringValue("Relay", "RelayState", "1");
  } else {
    digitalWrite(RELAY, HIGH);
    SaveStringValue("Relay", "RelayState", "0");
  }

  String relayStateTopic = "relay-state-" + customerId;

  int state = digitalRead(RELAY);
  String stateStr = String(state);
  client.publish(relayStateTopic.c_str(), stateStr.c_str());
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

    client.publish(topic.c_str(), json);
  }

  delay(1000);
}

///////////////////////////////////////////////////////

void SaveStringValue(String namespaceName, String key, String value){

  preferences.begin(namespaceName.c_str(), false);
  preferences.putString(key.c_str(), value);
  preferences.end();
}

String GetStringValue(String namespaceName, String key){

  preferences.begin(namespaceName.c_str(), false);
  String value = preferences.getString(key.c_str(), "");
  preferences.end();

  return value;
}

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
