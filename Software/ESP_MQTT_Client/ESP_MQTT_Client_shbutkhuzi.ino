#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <PZEM004Tv30.h>

PZEM004Tv30 pzem(Serial2, 16, 17);

#define RELAY 26

// MQTT Broker
const char *mqtt_broker = "161.35.207.10";
const char *clientId = "54321";
const char *topic = "measurement-54321";
const char *mqtt_username = "shbutkhuzi";
const char *mqtt_password = "shako123";
const int mqtt_port = 707;

WiFiClient espClient;
PubSubClient client(espClient);
StaticJsonDocument<200> measurement;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  WiFi.mode(WIFI_STA);

  WiFiManager wm;

  // reset settings - wipe stored credentials for testing, these are stored by the esp library
  //wm.resetSettings();

  WiFiManagerParameter data("ClientId", "Enter your string here", "default string", 50);
  wm.addParameter(&data);
    
  bool connected = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  if(!connected) {
      Serial.println("Failed to connect");
      ESP.restart();
  }
  else {
      Serial.println("Connected to the WIFI");
  }
  Serial.println("after connect\n");
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  client.subscribe("admin-12345");
  Serial.println("after broker\n");
}

void reconnect() {
  while (!client.connected()) {
    if (!client.connect(clientId, mqtt_username, mqtt_password)) {
      Serial.println("reconnect loop");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY, LOW);
  } else {
    digitalWrite(RELAY, HIGH);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
    Serial.println("reconnect\n");
  }
  client.loop();
  Serial.println("ki\n");

  double voltage = pzem.voltage();
  double current = pzem.current();
  double power = pzem.power();
  double energy = pzem.energy();
  double frequency = pzem.frequency();
  double pf = pzem.pf();


  if(!isnan(voltage) && !isnan(current) && !isnan(power) && !isnan(energy) && !isnan(frequency) && !isnan(pf))
  {
    measurement["Voltage"] = voltage;
    measurement["Current"] = current;
    measurement["Power"] = power;
    measurement["Energy"] = energy;
    measurement["Frequency"] = frequency;
    measurement["PowerFactor"] = pf;

    char json[200];
    serializeJson(measurement, json);

    client.publish(topic, json);
  }
  

  delay(1000);
}
