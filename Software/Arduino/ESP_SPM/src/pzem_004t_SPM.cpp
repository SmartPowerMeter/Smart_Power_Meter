#include "pzem_004t_SPM.h"
#include "Hardware_SPM.h"
#include "time_SPM.h"

char currMeasJson[200];

PZEM004Tv30 pzemInit(){
    PZEM004Tv30 pzem(Serial2, PZEM_TX, PZEM_RX);

    return pzem;
}

pzem_status pzemReadValues(PZEM004Tv30& pzem){

    if ((pzem.voltage()    == NAN) | 
        (pzem.current()    == NAN) | 
        (pzem.power()      == NAN) | 
        (pzem.energy()     == NAN) | 
        (pzem.frequency()  == NAN) | 
        (pzem.pf()         == NAN)  ){
        
        return PZEM_READ_ERROR;
    }

    return PZEM_OK;
}

char* measToJson(PZEM004Tv30& pzem){
    // "%05.1f,%07.3f,%07.1f,%07.2f,%04.1f,%04.2f\n"
    sprintf(currMeasJson, "{\"Voltage\":%.1f,"
                           "\"Current\":%.3f,"
                           "\"Power\":%.1f,"
                           "\"Energy\":%.2f,"
                           "\"Frequency\":%.1f,"
                           "\"PowerFactor\":%.2f,"
                           "\"TimeStamp\":\"%s\"}",   pzem.voltage(),
                                                      pzem.current(),
                                                      pzem.power(),
                                                      pzem.energy(),
                                                      pzem.frequency(),
                                                      pzem.pf(),
                                                      getMQTTFormatedCurrTime());

    return currMeasJson;
}

void pzemPrintValues(PZEM004Tv30& pzem, Print& port){
    port.printf("Voltage: %4.1f  Current: %6.3f  Power: %4.1f  Energy: %6.2f  Frequency %3.1f  PF: %3.2f\n",
                        pzem.voltage(),
                        pzem.current(),
                        pzem.power(),
                        pzem.energy(),
                        pzem.frequency(),
                        pzem.pf());
}

void pzemHandleError(pzem_status status){
    if (status == PZEM_READ_ERROR){
        Serial.println("PZEM read error");
    }
}