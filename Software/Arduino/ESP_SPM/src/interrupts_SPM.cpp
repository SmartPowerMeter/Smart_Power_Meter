#include "interrupts_SPM.h"

hw_timer_t * timerInterrupt1Sec = NULL;
uint8_t temp;
bool f_1sec_event = 0;

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