#ifndef PZEM_004T_SPM
#define PZEM_004T_SPM

#include "PZEM004Tv30.h"

#undef  UPDATE_TIME
#define UPDATE_TIME 1000

typedef enum{
  PZEM_OK = 0,
  PZEM_READ_ERROR
} pzem_status;

struct meas
{
  float voltage;
  float current;
  float power;
  float energy;
  float frequency;
  float pf;
};

PZEM004Tv30 pzemInit();
pzem_status pzemReadValues(PZEM004Tv30& pzem);
char* measToJson(PZEM004Tv30& pzem);
void pzemPrintValues(PZEM004Tv30& pzem, Print& port);

void pzemHandleError(pzem_status status);

#endif