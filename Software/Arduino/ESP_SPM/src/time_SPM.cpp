#include "time_SPM.h"
#include "pzem_004t_SPM.h"
#include "SD_SPM.h"

extern bool f_1sec_event;

extern PZEM004Tv30 pzem;


uint16_t getESPYear(){
    time_t now; time(&now);
    struct tm *timeinfo = gmtime(&now);
    return timeinfo->tm_year + 1900;
}

uint8_t getESPMonth(){
    time_t now; time(&now);
    struct tm *timeinfo = gmtime(&now);
    return timeinfo->tm_mon + 1;
}

uint8_t getESPDay(){
    time_t now; time(&now);
    struct tm *timeinfo = gmtime(&now);
    return timeinfo->tm_mday;
}

uint8_t getESPHour24(){
    time_t now; time(&now);
    struct tm *timeinfo = gmtime(&now);
    return timeinfo->tm_hour;
}

uint8_t getESPMin(){
    time_t now; time(&now);
    struct tm *timeinfo = gmtime(&now);
    return timeinfo->tm_min;
}

uint8_t getESPSec(){
    time_t now; time(&now);
    struct tm *timeinfo = gmtime(&now);
    return timeinfo->tm_sec;
}

time_status setESPTimeUsingWiFi(char* ssid, char *pass){
    if (WiFi.isConnected() != WL_CONNECTED){
        WiFi.begin(ssid, pass);
        uint8_t cnt = 0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(10);
            cnt++;
            if (cnt > 200) return TIME_WIFI_CONN_TIMEOUT_ERROR;
        }
    }

    configTime(0, 0, "time.nist.gov", "pool.ntp.org", "time.google.com");
    struct tm tmstruct;
    if (!getLocalTime(&tmstruct, 10000)) return TIME_GETLOCALTIME_TIMEOUT_ERROR;
    Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d UTC\n",
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
    // Serial.printf("Reading measurements took: %lu microseconds\n", end-start);

    start = micros();
    sd_status sd_ret = SDRoutineEverySec();
    end = micros();
    if (sd_ret != SD_OK){
        handleErrorSD(sd_ret);
    }
    Serial.printf("SDRoutineEverySec() took: %lu microseconds\n", end-start);
}

void timeHandleError(time_status status){
    if (status == TIME_WIFI_CONN_TIMEOUT_ERROR){
        Serial.println("WiFi connection timeout for updating ESP time");
    }else if (status == TIME_GETLOCALTIME_TIMEOUT_ERROR){
        Serial.println("Timeout getting time from NTP server");
    }
    
}