#ifndef MQTT_SPM
#define MQTT_SPM

#include "SIM800L_SPM.h"
#include "PubSubClient.h"
#include "time_SPM.h"
#include "WiFiManager_SPM.h"

#define MQTT_BROKER "smartpowermeter.ge"
#define MQTT_PORT    707
#define MQTT_USER    "shbutkhuzi_2"
#define MQTT_PASS    "shako12321"


void initMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void MQTTLoop();
bool getRelayConf();

#endif