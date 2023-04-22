#include "pzem_004t_SPM.h"
#include "Hardware_SPM.h"

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