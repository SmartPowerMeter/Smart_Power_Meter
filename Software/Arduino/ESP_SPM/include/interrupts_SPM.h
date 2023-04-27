#ifndef INTERRUPTS_SPM
#define INTERRUPTS_SPM

#include "Arduino.h"
#include "time.h"

void init1SecInterrupt();
void IRAM_ATTR every1Sec();
void onSDInsert();
void initSDInterrupt();

#endif