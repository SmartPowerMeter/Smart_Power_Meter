#include <Arduino.h>

#include "Hardware_SPM.h"
#include "SD_SPM.h"
#include "pzem_004t_SPM.h"
#include "time_SPM.h"
#include "interrupts_SPM.h"
#include "utils.h"
#include "SIM800L_SPM.h"
#include "WiFiManager_SPM.h"
#include "mqtt_SPM.h"
#include "LED_SPM.h"
#include "rtc_SPM.h"

PZEM004Tv30 pzem = pzemInit();

extern volatile LED_status status;
TaskHandle_t Task1;

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  while(1){

    ledLoop();

  }
}


void setup() {
  status = STARTING;
  Serial.begin(115200);

  pinMode(SIM800_EN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  pinMode(POWER_4_1_EN, OUTPUT);
  digitalWrite(SIM800_EN, HIGH);
  digitalWrite(POWER_4_1_EN, HIGH);


  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    0,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500);

  initLED();
  initLEDInterrupt();

  initRTC();

  Serial.printf("ESP Unique: %lu\n", getUnique());

  sd_status sd_ret = allCardChecks(SD, SD_PRECENCE);
  if(sd_ret != SD_OK){
    handleErrorSD(sd_ret);
  }

  initGSMSupport();

  initWiFiManager();
  getAllConf();


  initMQTT();


  struct tm now = getRTCDateTime();
  setESPTime(now);


  init1SecInterrupt();
  initSDInterrupt();
  initUserButtonInterrupt();
  
 
  setRelayStatus();
}

void loop() {

  MQTTLoop();
  initSDOnInterrupt();
  everySecond();
  usrButtonLoopCheck();

}