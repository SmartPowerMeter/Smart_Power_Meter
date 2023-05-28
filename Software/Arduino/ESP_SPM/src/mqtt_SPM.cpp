#include "mqtt_SPM.h"
#include "Hardware_SPM.h"
#include "Preferences.h"
#include "interrupts_SPM.h"

Preferences relayState;

extern TinyGsm modem;
extern bool GSMConf;
extern String CustomerId;
extern String GSMConfAPN;
extern String GSMConfPIN;
extern String GSMConfGPRSUser;
extern String GSMConfGPRSPass;

extern const char* CustomerId_c;
extern const char* GSMConfAPN_c;
extern const char* GSMConfPIN_c;
extern const char* GSMConfGPRSUser_c;
extern const char* GSMConfGPRSPass_c;

extern WiFiManager wm;

String topic;
const char* topic_c;

TinyGsmClient GSM_client(modem);
WiFiClient WiFi_client;
PubSubClient overWiFi(WiFi_client);
PubSubClient overGSM(GSM_client);
PubSubClient* mqtt_client;

bool flag_reconn_first = 0;
unsigned long reconnect_start_time = 0;
unsigned long last_reconnect_attmpt_time = 0;


void initMQTT(){
    // if configured to use GSM module
    if (GSMConf){
        WiFi.disconnect(true, true);
        Serial.println("--------> Configuring mqtt_client for GSM");
        mqtt_client = &overGSM;
        sim800l_status ret = initSequenceSIM800L();
        if (ret != SIM800L_OK){
            handleSIM800LError(ret);
        }
    }else{
        Serial.println("--------> Configuring mqtt_client for WiFi");
        mqtt_client = &overWiFi;
    }
    Serial.println("--------> Setting Server Parameters");
    Serial.printf("Broker: ->%s<-\n", MQTT_BROKER);
    Serial.printf("Port: ->%d<-\n", MQTT_PORT);

    mqtt_client->setServer(MQTT_BROKER, MQTT_PORT);

    Serial.println("--------> Setting Callback");
    mqtt_client->setCallback(mqtt_callback);

    Serial.printf("CustomerID: ->%s<-\n", CustomerId_c);
    topic = "measurement-" + CustomerId;
    topic_c = topic.c_str();
    Serial.printf("Topic: ->%s<-\n", topic_c);

    delay(2000);

    if (mqtt_client->connect(CustomerId_c, MQTT_USER, MQTT_PASS)){
        Serial.println("----------------------> MQTT Success Connect");
        String relayState = "relay-" + CustomerId;
        const char* relayState_c = relayState.c_str();
        mqtt_client->subscribe(relayState_c);
    }
}


void mqtt_callback(char* topic, byte* payload, unsigned int length){
    Serial.println("------------------------->  Into Callback");
    Serial.printf("topic: %s\n", topic);

    Serial.print("Payload: ");
    String messageTemp;
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        messageTemp += (char)payload[i];
    }
    Serial.println();

    relayState.begin("relayState");
    if ((char)payload[0] == '1') {
        digitalWrite(RELAY, LOW);
        relayState.putBool("power", true);
    } else {
        digitalWrite(RELAY, HIGH);
        relayState.putBool("power", false);
    }
    relayState.end();

    String relayStateTopic = "relay-state-" + CustomerId;
    const char* relayStateTopic_c = relayStateTopic.c_str();
    Serial.printf("Relay Topic: ->%s<-\n", relayStateTopic_c);

    int state = digitalRead(RELAY);
    String stateStr = String(state);
    const char* stateStr_c = stateStr.c_str();
    mqtt_client->publish(relayStateTopic_c, stateStr_c);
}


void reconnect(){
    Serial.printf("Reconnecting for: ->%s<-\n", CustomerId_c);
    if (flag_reconn_first == 0){
        flag_reconn_first = 1;
        reconnect_start_time = millis();
    }
    if (flag_reconn_first && ((millis() - reconnect_start_time) > 30000)){
        Serial.println("Restarting Due to MQTT connection timeout");
        usrButtonAction();
    }
    if (millis() - last_reconnect_attmpt_time > 5000) {
        last_reconnect_attmpt_time = millis();
        mqtt_client->connect(CustomerId_c, MQTT_USER, MQTT_PASS);
        delay(100);
    }
    if (mqtt_client->connected()){
        String relayState = "relay-" + CustomerId;
        const char* relayState_c = relayState.c_str();
        Serial.printf("subscribe to relayState: ->%s<-\n", relayState_c);
        mqtt_client->subscribe(relayState_c);
        flag_reconn_first = 0;
    }

}

void MQTTLoop(){
    if (!mqtt_client->connected()){
        Serial.println("----------->   MQTT Reconnecting");
        reconnect();
    }

    mqtt_client->loop();
}

bool getRelayConf(){
    relayState.begin("relayState");

    bool state = relayState.getBool("power");

    relayState.end();
    return state;
}