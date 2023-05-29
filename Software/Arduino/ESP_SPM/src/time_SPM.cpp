#include "time_SPM.h"
#include "pzem_004t_SPM.h"
#include "SD_SPM.h"
#include "mqtt_SPM.h"
#include "LED_SPM.h"

extern bool f_1sec_event;
char formatedDateTime_c[30];

extern char currMeasJson[200];
extern PZEM004Tv30 pzem;
extern PubSubClient* mqtt_client;
extern String topic;
extern const char* topic_c;

extern volatile LED_status status;


uint16_t getESPYear(){
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_year + 1900;
}

uint8_t getESPMonth(){
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_mon + 1;
}

uint8_t getESPDay(){
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_mday;
}

uint8_t getESPHour24(){
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_hour;
}

uint8_t getESPMin(){
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_min;
}

uint8_t getESPSec(){
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_sec;
}

char* getMQTTFormatedCurrTime(){
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    sprintf(formatedDateTime_c, "%04d-%02d-%02dT%02d:%02d:%02d", timeinfo.tm_year + 1900,
                                                                 timeinfo.tm_mon + 1,
                                                                 timeinfo.tm_mday,
                                                                 timeinfo.tm_hour,
                                                                 timeinfo.tm_min,
                                                                 timeinfo.tm_sec);

    return formatedDateTime_c;
}

time_status setESPTimeUsingWiFi(char* ssid, char *pass){
    if (WiFi.isConnected() != WL_CONNECTED){
        Serial.println("WiFi Connection for updating Time in progress");
        // WiFi.begin(ssid, pass);
        uint8_t cnt = 0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(10);
            cnt++;
            if (cnt > 200) return TIME_WIFI_CONN_TIMEOUT_ERROR;
        }
    }
    // config time for Georgia
    configTime(3600*4, 0, "time.nist.gov", "pool.ntp.org", "time.google.com");
    struct tm tmstruct;
    if (!getLocalTime(&tmstruct, 10000)) return TIME_GETLOCALTIME_TIMEOUT_ERROR;
    Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",
                    tmstruct.tm_year+1900,
                    tmstruct.tm_mon+1, 
                    tmstruct.tm_mday,
                    tmstruct.tm_hour, 
                    tmstruct.tm_min, 
                    tmstruct.tm_sec);
    return TIME_OK;
}

void everySecond(){
    if(f_1sec_event == 0) return;

    unsigned long start, end;

    f_1sec_event = 0;
    // start = micros();
    pzem_status pzem_ret = pzemReadValues(pzem);
    // end = micros();
    if (pzem_ret != PZEM_OK){
        pzemHandleError(pzem_ret);
    }
    // start = micros();
    measToJson(pzem);
    // end = micros();
    // Serial.printf("MeasToJson() took: %lu microseconds\n", end-start);
    // Serial.printf("Reading measurements took: %lu microseconds\n", end-start);

    // publish measurement
    if (mqtt_client->connected() && mqtt_client->publish(topic_c, currMeasJson)){
        status = CONNECTED;
        Serial.printf("%s: %s\n", topic_c, currMeasJson);
    }

    start = micros();
    sd_status sd_ret = SDRoutineEverySec();
    end = micros();
    if (sd_ret != SD_OK){
        handleErrorSD(sd_ret);
    }
    // Serial.printf("SDRoutineEverySec() took: %lu microseconds\n", end-start);
}

void timeHandleError(time_status status){
    if (status == TIME_WIFI_CONN_TIMEOUT_ERROR){
        Serial.println("WiFi connection timeout for updating ESP time");
    }else if (status == TIME_GETLOCALTIME_TIMEOUT_ERROR){
        Serial.println("Timeout getting time from NTP server");
    }
    
}