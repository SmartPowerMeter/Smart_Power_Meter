#include "interrupts_SPM.h"
#include "Hardware_SPM.h"
#include "WiFiManager_SPM.h"

#include "SD.h"

extern WiFiManager wm;

hw_timer_t * timerInterrupt1Sec = NULL;
uint8_t temp;
bool f_1sec_event = 0;
bool f_SD_fall_edge = 0;
unsigned long SD_insert_start = 0;
bool f_usr_btn = 0;
unsigned long button_push_time = 0;


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


void initUserButtonInterrupt(){
    pinMode(USR_BTN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(USR_BTN), onFallingUserButton, FALLING);
}


void onFallingUserButton(){
    if (f_usr_btn == 0){
        f_usr_btn = 1;
        button_push_time = millis();
    }
}


void usrButtonLoopCheck(){
    if (digitalRead(USR_BTN) == 1){
        f_usr_btn = 0;
        button_push_time = 0;
    }else if (f_usr_btn && ((millis() - button_push_time) > 3000)){
        usrButtonAction();
    }
    
}


void usrButtonAction(){
    Serial.print("Resetting settings and restarting ESP in 3 ");
    delay(1000);
    Serial.print("2 ");
    delay(1000);
    Serial.println("1");
    delay(1000);

    wm.resetSettings();
    setGSMConf(false);
    ESP.restart();
}