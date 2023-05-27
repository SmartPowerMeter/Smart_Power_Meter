#include "SIM800L_SPM.h"
#include "Hardware_SPM.h"
# include "Preferences.h"

TinyGsm modem(SerialAT);
Preferences pref_SIM800;

sim800l_status checkSIM800LPrecense(){
    pinMode(SIM800_EN, OUTPUT);
    pinMode(POWER_4_1_EN, OUTPUT);
    digitalWrite(SIM800_EN, HIGH);    // enable SIM800 module
    digitalWrite(POWER_4_1_EN, LOW);  // provide power to it

    SerialAT.begin(GSM_BAUD, SERIAL_8N1, SIM800_TX, SIM800_RX);
    delay(5000); 

    if(modem.testAT()){
        TINY_GSM_DEBUG.println("SIM800L Module available");
        modem.poweroff();
        digitalWrite(POWER_4_1_EN, HIGH);
        return SIM800L_OK;
    }else{
        TINY_GSM_DEBUG.println("SIM800L Module is not available");
        digitalWrite(POWER_4_1_EN, HIGH);
        return SIM800L_ABSENT;
    }
}

sim800l_status checkAndSaveGSMSupport(){
    sim800l_status ret = checkSIM800LPrecense();
    pref_SIM800.begin("infoGSM");

    if (ret != SIM800L_OK){
        pref_SIM800.putBool("support", false);
        TINY_GSM_DEBUG.println("GSM support false is saved into ESP");
    }else{
        pref_SIM800.putBool("support", true);
        TINY_GSM_DEBUG.println("GSM support true is saved into ESP");
    }

    pref_SIM800.end();

    return SIM800L_OK;
}


volatile bool getGSMSupport(){
    pref_SIM800.begin("infoGSM");

    bool prec = pref_SIM800.getBool("support");

    pref_SIM800.end();
    return prec;
}


sim800l_status initGSMSupport(){
    pref_SIM800.begin("infoGSM");

    if (pref_SIM800.isKey("support")){
        bool prec = pref_SIM800.getBool("support");

        if (prec){
            TINY_GSM_DEBUG.println("GSM Support is checked already and is supported");
        }else{
            TINY_GSM_DEBUG.println("GSM Support is checked already and is not supported");
        }

        return SIM800L_OK;
    }else{
        TINY_GSM_DEBUG.println("GSM Support info is absent");
        TINY_GSM_DEBUG.println("Checking GSM Support now ...");
        sim800l_status ret = checkAndSaveGSMSupport();
        if (ret != SIM800L_OK){
            TINY_GSM_DEBUG.println("checkAndSaveGSMSupport() failed");
            return SIM800L_CHECK_AND_SAVE_ERROR;
        }
        return ret;
    }
}