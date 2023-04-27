#include <Arduino.h>

#include "Hardware_SPM.h"
#include "SD_SPM.h"
#include "pzem_004t_SPM.h"
#include "time_SPM.h"
#include "interrupts_SPM.h"

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
   
  time_t tt = time(nullptr) - 11*60;
  struct timeval tv = {tt, 0};
  settimeofday(&tv, nullptr);
  Serial.print("Time after modification: ");
  Serial.println(ctime(&tt));

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


  // char path[] = "/SPM_DATA/2023/04/26/m.txt";
  // struct tm start, end;


  // start.tm_year = 2023;
  // start.tm_mon = 4;
  // start.tm_mday = 26;
  // start.tm_hour = 20;
  // start.tm_min = 37;
  // start.tm_sec = 0;

  // end.tm_year = 2023;
  // end.tm_mon = 4;
  // end.tm_mday = 26;
  // end.tm_hour = 20;
  // end.tm_min = 41;
  // end.tm_sec = 0;

  // struct meas meas;
  // time_t st = mktime(&start);
  // time_t en = mktime(&end);

  // unsigned long b, e;
  // b = micros();
  // sd_ret = meanLastValsBetween(path, st, en, &meas, 7);
  // e = micros();
  // if(sd_ret != SD_OK){
  //   handleErrorSD(sd_ret);
  // }

  // Serial.println("\n\nfinal result");
  // Serial.printf("voltage: %f\n", meas.voltage);
  // Serial.printf("current: %f\n", meas.current);
  // Serial.printf("power: %f\n", meas.power);
  // Serial.printf("energy: %f\n", meas.energy);
  // Serial.printf("frequency: %f\n", meas.frequency);
  // Serial.printf("pf: %f\n", meas.pf);

  // Serial.printf("\nReading took: %lu microseconds", e-b);  

}


void loop() {
  everySecond();

  // pzem_status pzem_ret = pzemReadValues(pzem);
  // if (pzem_ret != PZEM_OK) pzemHandleError(pzem_ret);
  // pzemPrintValues(pzem, Serial);
  // delay(1000);
}