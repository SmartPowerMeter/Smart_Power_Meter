#ifndef SIM800L_SPM
#define SIM800L_SPM

#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_DEBUG Serial
#define GSM_PIN ""

#define SerialAT Serial1
#define GSM_BAUD 115200

#include "TinyGsmClient.h"

//for debugging
// #define DUMP_AT_COMMANDS

typedef enum{
  SIM800L_OK = 0,
  SIM800L_ABSENT,
  SIM800L_CHECK_AND_SAVE_ERROR,
  SIM800L_SIM_UNLOCK_ERROR,
  SIM800L_GPRS_CONNECTION_ERROR,
  SIM800L_SIM_CARD_NOT_INSERTED
} sim800l_status;


sim800l_status checkSIM800LPrecense();
sim800l_status checkAndSaveGSMSupport();
volatile bool getGSMSupport();
sim800l_status initGSMSupport();
sim800l_status initSequenceSIM800L();

void handleSIM800LError(sim800l_status status);

#endif