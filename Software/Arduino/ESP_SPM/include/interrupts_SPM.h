#ifndef INTERRUPTS_SPM
#define INTERRUPTS_SPM

#include "Arduino.h"
#include "time.h"

void init1SecInterrupt();
void initLEDInterrupt();
void IRAM_ATTR every1Sec();
void IRAM_ATTR LEDCallbackRoutine();
void onSDInsert();
void initSDInterrupt();
void initUserButtonInterrupt();
void onFallingUserButton();
void usrButtonLoopCheck();
void usrButtonAction();

#endif