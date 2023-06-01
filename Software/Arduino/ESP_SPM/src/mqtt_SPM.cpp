#include "mqtt_SPM.h"
#include "Hardware_SPM.h"
#include "Preferences.h"
#include "interrupts_SPM.h"
#include "LED_SPM.h"

Preferences relayState;
Preferences reconnRebootLimit;

extern TinyGsm modem;
extern bool GSMConf;
extern bool AlertConf;
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

extern volatile LED_status status;

void initMQTT(){
    // if configured to use GSM module
    if (GSMConf){
        Serial.println("--------> Configuring mqtt_client for GSM");
        WiFi.disconnect(true, true);
        WiFi_client.stop();
        overWiFi.~PubSubClient();
        GSM_client.stop();
        GSM_client.init(&modem);
        overGSM.setClient(GSM_client);
        mqtt_client = &overGSM;
        sim800l_status ret = initSequenceSIM800L();
        if (ret != SIM800L_OK){
            handleSIM800LError(ret);
        }
    }else{
        Serial.println("--------> Configuring mqtt_client for WiFi");
        mqtt_client = &overWiFi;

        if (AlertConf){
            sim800l_status ret = initGSMWithoutGPRS();
            if (ret != SIM800L_OK){
                handleSIM800LError(ret);
            }
        }
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

    // delay(2000);

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
        status = RECONNECTING;
        flag_reconn_first = 1;
        reconnect_start_time = millis();
    }
    if (flag_reconn_first && ((millis() - reconnect_start_time) > 30000)){
        Serial.println("Restarting Due to MQTT connection timeout");
        softRestart();
        // usrButtonAction();
        // ESP.restart();
    }
    if (millis() - last_reconnect_attmpt_time > 5000) {
        last_reconnect_attmpt_time = millis();
        mqtt_client->connect(CustomerId_c, MQTT_USER, MQTT_PASS);
        delay(100);
    }
    if (mqtt_client->connected()){
        status = CONNECTED;
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

uint8_t getReconnNumber(){
    reconnRebootLimit.begin("reconn");
    
    uint8_t num = reconnRebootLimit.getInt("number", 0);

    reconnRebootLimit.end();

    return num;
}


void setReconnNumber(uint8_t num){
    reconnRebootLimit.begin("reconn");
    
    reconnRebootLimit.putInt("number", num);

    reconnRebootLimit.end();
}


// read from preferences and set to previously saved value
void setRelayStatus(){
    relayState.begin("relayState");

    if (relayState.getBool("power", false)){
        digitalWrite(RELAY, LOW);
    }else{
        digitalWrite(RELAY, HIGH);
    }

    relayState.end();
}


void softRestart(){
    uint8_t reconn_num = getReconnNumber();
    if (reconn_num > 5){
        setReconnNumber(0);
        usrButtonAction();
    }else{
        setReconnNumber(reconn_num + 1);
        ESP.restart();
    }
}