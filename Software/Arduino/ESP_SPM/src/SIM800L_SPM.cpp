#include "SIM800L_SPM.h"
#include "Hardware_SPM.h"
#include "Preferences.h"
#include "interrupts_SPM.h"


#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, TINY_GSM_DEBUG);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif


Preferences pref_SIM800;

extern bool GSMConf;
extern String CustomerId;
extern String GSMConfAPN;
extern String GSMConfPIN;
extern String GSMConfGPRSUser;
extern String GSMConfGPRSPass;

extern const char* CustomerId_c;
extern const char* GSMConfAPN_c;
extern const char* GSMConfPIN_c;
extern const char* GSMConfGPRSUser_c;
extern const char* GSMConfGPRSPass_c;

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


sim800l_status initSequenceSIM800L(){
    TINY_GSM_DEBUG.println("Configuring SIM Enable and power pins");
    pinMode(SIM800_EN, OUTPUT);
    pinMode(POWER_4_1_EN, OUTPUT);
    digitalWrite(SIM800_EN, HIGH);    // enable SIM800 module
    digitalWrite(POWER_4_1_EN, LOW);  // provide power to it

    TINY_GSM_DEBUG.println("Starting Serial communication");
    SerialAT.begin(GSM_BAUD, SERIAL_8N1, SIM800_TX, SIM800_RX);
    delay(8000);

    TINY_GSM_DEBUG.println("Initializing modem");
    modem.restart();

    String modemInfo = modem.getModemInfo();
    TINY_GSM_DEBUG.print("Modem Info: ");
    TINY_GSM_DEBUG.println(modemInfo);

    uint8_t sim_status = modem.getSimStatus();
    TINY_GSM_DEBUG.printf("SIM Status: %d\n", sim_status);
    if ((!GSMConfPIN.isEmpty()) && (sim_status != 3)){
        TINY_GSM_DEBUG.println("Unlocking SIM Card");
        if (!modem.simUnlock(GSMConfPIN_c)){
            return SIM800L_SIM_UNLOCK_ERROR;
        }
    }

    TINY_GSM_DEBUG.print("Connecting to APN: ");
    TINY_GSM_DEBUG.println(GSMConfAPN_c);
    TINY_GSM_DEBUG.printf("GPRS User: ->%s<-\n", GSMConfGPRSUser_c);
    TINY_GSM_DEBUG.printf("GPRS Pass: ->%s<-\n", GSMConfGPRSPass_c);

    if (!modem.gprsConnect(GSMConfAPN_c, GSMConfGPRSUser_c, GSMConfGPRSPass_c)){
        return SIM800L_GPRS_CONNECTION_ERROR;
    }

    if (modem.isGprsConnected()){
        TINY_GSM_DEBUG.println("GPRS context opened");
    }

    int   year3    = 0;
    int   month3   = 0;
    int   day3     = 0;
    int   hour3    = 0;
    int   min3     = 0;
    int   sec3     = 0;
    float timezone = 0;

    if (modem.getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3, &timezone)) {
        TINY_GSM_DEBUG.print("--------------------------------------------------------------->     GSM time: ");
        TINY_GSM_DEBUG.print("Year: ");     TINY_GSM_DEBUG.println(year3);
        TINY_GSM_DEBUG.print("Month: ");    TINY_GSM_DEBUG.println(month3);
        TINY_GSM_DEBUG.print("Day: ");      TINY_GSM_DEBUG.println(day3);
        TINY_GSM_DEBUG.print("Hour: ");     TINY_GSM_DEBUG.println(hour3);
        TINY_GSM_DEBUG.print("Min: ");      TINY_GSM_DEBUG.println(min3);
        TINY_GSM_DEBUG.print("Sec: ");      TINY_GSM_DEBUG.println(sec3);
        TINY_GSM_DEBUG.print("Timezone: "); TINY_GSM_DEBUG.println(timezone);
    }

    return SIM800L_OK;
}


void handleSIM800LError(sim800l_status status){
    if (status == SIM800L_SIM_UNLOCK_ERROR){
        TINY_GSM_DEBUG.println("SIM800L SIM Unlock Error");
        usrButtonAction();
    }else if (status == SIM800L_GPRS_CONNECTION_ERROR)
    {
        TINY_GSM_DEBUG.println("GSM Failed to open GPRS context");
        usrButtonAction();
    }
    

}
