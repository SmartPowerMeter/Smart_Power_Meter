#include <Arduino.h>

#include "Hardware_SPM.h"
#include "SD_SPM.h"
#include "pzem_004t_SPM.h"

PZEM004Tv30 pzem = pzemInit();

void setup() {
  Serial.begin(115200);

  pinMode(SIM800_EN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(SIM800_EN, HIGH);
  digitalWrite(RELAY, LOW);

  sd_status sd_ret = allCardChecks(SD, SD_PRECENCE);
  if(sd_ret != SD_OK){
    handleErrorSD(sd_ret);
  }

  readFileUntilChar(SD, "/data/2023-03-27.txt", '\n');

  // pinMode(2, OUTPUT);
  // digitalWrite(2, LOW);
}

void loop() {
  // pzem_status pzem_ret = pzemReadValues(pzem);
  // if (pzem_ret != PZEM_OK) pzemHandleError(pzem_ret);
  // pzemPrintValues(pzem, Serial);
  // delay(1000);
}