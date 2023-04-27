#include "interrupts_SPM.h"
#include "Hardware_SPM.h"

#include "SD.h"

hw_timer_t * timerInterrupt1Sec = NULL;
uint8_t temp;
bool f_1sec_event = 0;
bool f_SD_fall_edge = 0;
unsigned long SD_insert_start = 0;


void IRAM_ATTR every1Sec() {
    if (temp == 1){
        digitalWrite(22, LOW);
        temp = 0;
    }else if (temp == 0){
        digitalWrite(22, HIGH);
        temp = 1;
    }

    if (f_1sec_event == 0) f_1sec_event = 1;
}

void init1SecInterrupt(){
    timerInterrupt1Sec = timerBegin(0, 80, true);
    timerAttachInterrupt(timerInterrupt1Sec, &every1Sec, true);
    timerAlarmWrite(timerInterrupt1Sec, 1000000, true);
    timerAlarmEnable(timerInterrupt1Sec);
}

void onSDInsert(){
    if(f_SD_fall_edge == 0) {
        f_SD_fall_edge = 1;
        SD_insert_start = millis();
    }
}

// void onSDRemove(){
//     if(f_SD_rise_edge == 0) f_SD_rise_edge = 1;
// }

void initSDInterrupt(){
    pinMode(SD_PRECENCE, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SD_PRECENCE), onSDInsert, FALLING);
    // attachInterrupt(digitalPinToInterrupt(SD_PRECENCE), onSDRemove, RISING);
}