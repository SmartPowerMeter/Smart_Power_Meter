#ifndef RTC_SPM
#define RTC_SPM

#include "Wire.h"
#include "RtcDS3231.h"

#define countof(a) (sizeof(a) / sizeof(a[0]))

bool wasError(const char* errorTopic);
void initRTC();
void printDateTime(const RtcDateTime& dt);
void rtcSetCurrDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
struct tm getRTCDateTime();

#endif