#include "rtc_SPM.h"
#include "time_SPM.h"

RtcDS3231<TwoWire> Rtc(Wire);

bool wasError(const char* errorTopic = "")
{
    uint8_t error = Rtc.LastError();
    if (error != 0)
    {
        // we have a communications error
        // see https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
        // for what the number means
        Serial.print("[");
        Serial.print(errorTopic);
        Serial.print("] WIRE communications error (");
        Serial.print(error);
        Serial.print(") : ");

        switch (error)
        {
        case Rtc_Wire_Error_None:
            Serial.println("(none?!)");
            break;
        case Rtc_Wire_Error_TxBufferOverflow:
            Serial.println("transmit buffer overflow");
            break;
        case Rtc_Wire_Error_NoAddressableDevice:
            Serial.println("no device responded");
            break;
        case Rtc_Wire_Error_UnsupportedRequest:
            Serial.println("device doesn't support request");
            break;
        case Rtc_Wire_Error_Unspecific:
            Serial.println("unspecified error");
            break;
        case Rtc_Wire_Error_CommunicationTimeout:
            Serial.println("communications timed out");
            break;
        }
        return true;
    }
    return false;
}


void initRTC(){
    Rtc.Begin();

    if (!Rtc.GetIsRunning())
    {
        if (!wasError("setup GetIsRunning"))
        {
            Serial.println("RTC was not actively running, starting now");
            Rtc.SetIsRunning(true);
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (!wasError("setup GetDateTime"))
    {
        Serial.print("RTC Time: ");
        printDateTime(now);
        Serial.println();
    }

    Rtc.Enable32kHzPin(false);
    wasError("setup Enable32kHzPin");
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
    wasError("setup SetSquareWavePin");
}


void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}


void rtcSetCurrDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
    RtcDateTime curr_time = RtcDateTime(year, month, day, hour, minute, second);

    Serial.print("Setting RTC time to: ");
    printDateTime(curr_time);
    Serial.println();
    
    Rtc.SetDateTime(curr_time);
}


struct tm getRTCDateTime(){
    struct tm now_tm;
    RtcDateTime now = Rtc.GetDateTime();
    
    now_tm.tm_year = now.Year() - 1900;
    now_tm.tm_mon  = now.Month() - 1;
    now_tm.tm_mday = now.Day();
    now_tm.tm_hour = now.Hour();
    now_tm.tm_min  = now.Minute();
    now_tm.tm_sec  = now.Second();
    
    return now_tm;
}