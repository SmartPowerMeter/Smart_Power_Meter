#include "WiFiManager_SPM.h"
#include "utils.h"
#include "SIM800L_SPM.h"
#include "Preferences.h"

WiFiManager wm;
Preferences initSetupParams;

char html[] = "<html><body><h2>GSM Settings</h2><p>Following parameters will be necessary to open GPRS context</p></body></html>";
WiFiManagerParameter enteredCustomerId("CustomerId", "Enter Provided CustomerId", "", 50);
WiFiManagerParameter enteredSDConf("SDConf", "Do you want to save data on SD Card? (Yes/No)", "No", 3);
WiFiManagerParameter enteredGSMConf("GSMConf", "Do you want to use GSM instead of WiFi? (Yes/No)", "No", 3, html, WFM_LABEL_BEFORE);
WiFiManagerParameter enteredGSMConfAPN("GSMConfAPN", "Provide APN for GPRS context", "", 30);
WiFiManagerParameter enteredGSMConfPIN("GSMConfPIN", "Provide SIM Card PIN if SIM is locked", "", 6);
WiFiManagerParameter enteredGSMConfGPRSUser("GSMConfGPRSUser", "Provide GPRS User if present", "", 30);
WiFiManagerParameter enteredGSMConfGPRSPass("GSMConfGPRSPass", "Provide GPRS Password if present", "", 30);


volatile void initWiFiManager(){
    // wm.resetSettings();
    const char * menu[] = {"wifi"};
    wm.setMenu(menu, 1);
    wm.setBreakAfterConfig(true);
    wm.setSaveParamsCallback(saveParamCallback);
    wm.setConnectTimeout(5);
    wm.setSaveConnectTimeout(5);
    wm.setConnectRetries(1);
    // wm.setSaveConnect(false);

    wm.setCustomHeadElement("<h1>Smart Power Meter</h1><h3>Initial Setup</h3>");

    wm.addParameter(&enteredCustomerId);
    wm.addParameter(&enteredSDConf);

    if (getGSMSupport()){
        wm.addParameter(&enteredGSMConf);
        wm.addParameter(&enteredGSMConfAPN);
        wm.addParameter(&enteredGSMConfPIN);
        wm.addParameter(&enteredGSMConfGPRSUser);
        wm.addParameter(&enteredGSMConfGPRSPass);
    }

    unsigned long unique = getUnique();
    String pre_name = "SPM_";
    String unique_str = String(unique);
    String AP_name = pre_name + unique_str;
    char AP_name_chr[20];
    char unique_chr[20];
    AP_name.toCharArray(AP_name_chr, sizeof(AP_name_chr));
    unique_str.toCharArray(unique_chr, sizeof(unique_chr));

    // if not configured to use GSM module
    if (!getGSMConf()){
        bool conn = wm.autoConnect(AP_name_chr, unique_chr);
        if (!conn){
            Serial.println("WiFi Manager Connection Failed");
            Serial.println("Now handle wifi not connection");

            // if GSM module use is requested
            if (getGSMConf()){
                Serial.println("Configured to use GSM Module");
            }else{
                ESP.restart();
            }

        }else{
            Serial.println("WiFi Manager Connection Success");
        }
    }

}


void saveParamCallback(){
    Serial.println("----------------------------->Callback");

    String customerId = String(enteredCustomerId.getValue());
    String SDConf = String(enteredSDConf.getValue());
    String GSMConf = String(enteredGSMConf.getValue());
    String GSMConfAPN = String(enteredGSMConfAPN.getValue());
    String GSMConfPIN = String(enteredGSMConfPIN.getValue());
    String GSMConfGPRSUser = String(enteredGSMConfGPRSUser.getValue());
    String GSMConfGPRSPass = String(enteredGSMConfGPRSPass.getValue());

    Serial.printf("CustomerId:      %s\n", customerId);
    Serial.printf("SDConf:          %s\n", SDConf);
    Serial.printf("GSMConf:         %s\n", GSMConf);
    Serial.printf("GSMConfAPN:      %s\n", GSMConfAPN);
    Serial.printf("GSMConfPIN:      %s\n", GSMConfPIN);
    Serial.printf("GSMConfGPRSUser: %s\n", GSMConfGPRSUser);
    Serial.printf("GSMConfGPRSPass: %s\n", GSMConfGPRSPass);

    initSetupParams.begin("initParams");

    SDConf.toLowerCase();
    GSMConf.toLowerCase();

    bool sd_conf = false;
    if ((SDConf == String("yes")) || (SDConf == "y")){
        sd_conf = true;
    }

    bool gsm_conf = false;
    if ((GSMConf == String("yes")) || (GSMConf == "y")){
        gsm_conf = true;
    }

    initSetupParams.putString("CustomerId", customerId);
    initSetupParams.putBool("SDConf", sd_conf);
    initSetupParams.putBool("GSMConf", gsm_conf);
    initSetupParams.putString("GSMConfAPN", GSMConfAPN);
    initSetupParams.putString("GSMConfPIN", GSMConfPIN);
    initSetupParams.putString("GSMConfGPRSU", GSMConfGPRSUser);
    initSetupParams.putString("GSMConfGPRSP", GSMConfGPRSPass);

    initSetupParams.end();
}


bool getGSMConf(){
    initSetupParams.begin("initParams");

    bool param = initSetupParams.getBool("GSMConf");

    initSetupParams.end();
    return param;
}


void setGSMConf(bool val){
    initSetupParams.begin("initParams");

    initSetupParams.putBool("GSMConf", val);

    initSetupParams.end();
}