#include <Arduino.h>

#include "Hardware_SPM.h"
#include "SD_SPM.h"
#include "pzem_004t_SPM.h"
#include "time_SPM.h"

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

  time_status time_ret = setESPTimeUsingWiFi((char*)"SHAKO", (char*)"stereometria2001");
  if (time_ret != TIME_OK){
    timeHandleError(time_ret);
  }

  pinMode(22, OUTPUT);
  init1SecInterrupt();




  // readFileUntilChar(SD, "/data/2023-03-27.txt", '\n');
  // listDir(SD, "/data", 4);
  // Serial.print("Exists or not: ");
  // Serial.println(SD.exists("/data/2023/04/23"));
  // if (!SD.exists("/data/2023/04/23")){
  //   Serial.println(makeDirRecursive(SD, "/data/2023/04/23"));
  // }
  // Serial.print("Exists or not 2: ");
  // Serial.println(SD.exists("/data/2023/04/23"));
  // listDir(SD, "/data", 4);
  // appendFile(SD, "/data/2023/04/23/s.txt", "some text");

  // pinMode(2, OUTPUT);
  // digitalWrite(2, LOW);
}

void loop() {
  everySecond();
  // pzem_status pzem_ret = pzemReadValues(pzem);
  // if (pzem_ret != PZEM_OK) pzemHandleError(pzem_ret);
  // pzemPrintValues(pzem, Serial);
  // delay(1000);
}