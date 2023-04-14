/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cloud-mqtt-broker-sim800l/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Select your modem:
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800L

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands
#define SerialAT Serial1

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "3g.ge"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "";
const char gprsPass[] = "";

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// MQTT details
const char* broker = "smartpowermeter.ge";                    // Public IP address or domain name
// const char* mqttUsername = "shbutkhuzi_2";  // MQTT username
// const char* mqttPassword = "shako_12321";  // MQTT password

// const char* topicOutput1 = "esp/output1";
// const char* topicOutput2 = "esp/output2";
// const char* topicTemperature = "esp/temperature";
// const char* topicHumidity = "esp/humidity";


const char *clientId = "12321";
const char *topic = "measurement-12321";
const char *mqtt_username = "shbutkhuzi_2";
const char *mqtt_password = "shako12321";
const int mqtt_port = 707;


// Define the serial console for debug prints, if needed
// #define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
// #include "sys/time.h"
// #include "Arduino.h"
// #include <WiFi.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>

TinyGsmClient client(modem);
PubSubClient mqtt(client);
StaticJsonDocument<200> measurement;

// TTGO T-Call pins
#define MODEM_RST            27
// #define MODEM_PWKEY          4
// #define MODEM_POWER_ON       23
#define MODEM_TX             14
#define MODEM_RX             13
// #define I2C_SDA              21
// #define I2C_SCL              22

// BME280 pins
// #define I2C_SDA_2            18
// #define I2C_SCL_2            19

// #define OUTPUT_1             2
// #define OUTPUT_2             15

uint32_t lastReconnectAttempt = 0;

// I2C for SIM800 (to keep it running when powered from battery)
// TwoWire I2CPower = TwoWire(0);

// TwoWire I2CBME = TwoWire(1);
// Adafruit_BME280 bme;

// #define IP5306_ADDR          0x75
// #define IP5306_REG_SYS_CTL0  0x00

float temperature = 0;
float humidity = 0;
long lastMsg = 0;

// bool setPowerBoostKeepOn(int en){
//   I2CPower.beginTransmission(IP5306_ADDR);
//   I2CPower.write(IP5306_REG_SYS_CTL0);
//   if (en) {
//     I2CPower.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
//   } else {
//     I2CPower.write(0x35); // 0x37 is default reg value
//   }
//   return I2CPower.endTransmission() == 0;
// }


void mqttCallback(char* topic, byte* message, unsigned int len) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < len; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp/output1, you check if the message is either "true" or "false". 
  // Changes the output state according to the message
  // if (String(topic) == "esp/output1") {
  //   Serial.print("Changing output to ");
  //   if(messageTemp == "true"){
  //     Serial.println("true");
  //     digitalWrite(OUTPUT_1, HIGH);
  //   }
  //   else if(messageTemp == "false"){
  //     Serial.println("false");
  //     digitalWrite(OUTPUT_1, LOW);
  //   }
  // }
  // else if (String(topic) == "esp/output2") {
  //   Serial.print("Changing output to ");
  //   if(messageTemp == "true"){
  //     Serial.println("true");
  //     digitalWrite(OUTPUT_2, HIGH);
  //   }
  //   else if(messageTemp == "false"){
  //     Serial.println("false");
  //     digitalWrite(OUTPUT_2, LOW);
  //   }
  // }
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker without username and password
  //boolean status = mqtt.connect("GsmClientN");

  // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect(clientId, mqtt_username, mqtt_password);

  if (status == false) {
    SerialMon.println(" fail");
    ESP.restart();
    return false;
  }
  SerialMon.println(" success");
  mqtt.subscribe(topic);
  // mqtt.subscribe(topicOutput2);

  return mqtt.connected();
}


void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);
  
  // Start I2C communication
  // I2CPower.begin(I2C_SDA, I2C_SCL, 400000);
  // I2CBME.begin(I2C_SDA_2, I2C_SCL_2, 400000);
  
  // Keep power when running from battery
  // bool isOk = setPowerBoostKeepOn(1);
  // SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set modem reset, enable, power pins
  // pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  // pinMode(MODEM_POWER_ON, OUTPUT);
  // digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  // digitalWrite(MODEM_POWER_ON, HIGH);
  
  // pinMode(OUTPUT_1, OUTPUT);
  // pinMode(OUTPUT_2, OUTPUT);
  
  SerialMon.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

  // You might need to change the BME280 I2C address, in our case it's 0x76
  // if (!bme.begin(0x76, &I2CBME)) {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //   while (1);
  // }

  SerialMon.print("Connecting to APN: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    ESP.restart();
  }
  else {
    SerialMon.println(" OK");
  }
  
  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }

  // modem.setTimeFromNITZ(true, nitzServerNumber);
  // time_t now = modem.getGSMDateTime();
  // SerialMon.print("------------------------------------------------------------------->   Time from GSM: ");
  // SerialMon.println(ctime(&now));
  int   year3    = 0;
  int   month3   = 0;
  int   day3     = 0;
  int   hour3    = 0;
  int   min3     = 0;
  int   sec3     = 0;
  float timezone = 0;

  if (modem.getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3, &timezone)) {
    SerialMon.print("--------------------------------------------------------------->     GSM time: ");
    SerialMon.print("Year: ");     SerialMon.println(year3);
    SerialMon.print("Month: ");    SerialMon.println(month3);
    SerialMon.print("Day: ");      SerialMon.println(day3);
    SerialMon.print("Hour: ");     SerialMon.println(hour3);
    SerialMon.print("Min: ");      SerialMon.println(min3);
    SerialMon.print("Sec: ");      SerialMon.println(sec3);
    SerialMon.print("Timezone: "); SerialMon.println(timezone);
  }

  randomSeed(year3*month3*day3*hour3*min3*sec3);

  // MQTT Broker setup
  mqtt.setServer(broker, mqtt_port);
  mqtt.setCallback(mqttCallback);
}

double voltage = 232.97;
double current = 2.34;
double power = 29;
double energy = 0.96;
double frequency = 50.01;
double pf = 0.95;

void loop() {
  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

    measurement["Voltage"] = voltage + voltage*random(-1, 2)*random(0, 10)*0.01;
    measurement["Current"] = current + current*random(-1, 2)*random(0, 10)*0.01;
    measurement["Power"] = power + power*random(-1, 2)*random(0, 10)*0.01;
    measurement["Energy"] = energy + energy*random(0, 2)*random(0, 10)*0.01;
    measurement["Frequency"] = frequency + frequency*random(-1, 2)*random(0, 10)*0.008;
    measurement["PowerFactor"] = pf + pf*random(-1, 2)*random(0, 10)*0.008;

    energy = energy + 0.01;

    char json[200];
    serializeJson(measurement, json);

    mqtt.publish(topic, json);
    
    // Temperature in Celsius
    // temperature = bme.readTemperature();   
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    //temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
    
    // Convert the value to a char array
    // char tempString[8];
    // dtostrf(temperature, 1, 2, tempString);
    // Serial.print("Temperature: ");
    // Serial.println(tempString);
    // mqtt.publish(topicTemperature, tempString);

    // humidity = bme.readHumidity();
    
    // // Convert the value to a char array
    // char humString[8];
    // dtostrf(humidity, 1, 2, humString);
    // Serial.print("Humidity: ");
    // Serial.println(humString);
    // mqtt.publish(topicHumidity, humString);
  }

  mqtt.loop();
}