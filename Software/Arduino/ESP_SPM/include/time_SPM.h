#ifndef TIME_SPM
#define TIME_SPM

#include "time.h"
#include "WiFi.h"

typedef enum{
  TIME_OK,
  TIME_WIFI_CONN_TIMEOUT_ERROR,
  TIME_GETLOCALTIME_TIMEOUT_ERROR
} time_status;

uint16_t getESPYear();
uint8_t getESPMonth();
uint8_t getESPDay();
uint8_t getESPHour24();
uint8_t getESPMin();
uint8_t getESPSec();
char* getMQTTFormatedCurrTime();
time_status setESPTimeUsingWiFi(char* ssid, char *pass);
void everySecond();

void timeHandleError(time_status status);

#endif