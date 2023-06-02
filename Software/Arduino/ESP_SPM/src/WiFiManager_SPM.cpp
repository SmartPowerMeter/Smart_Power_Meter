#include "WiFiManager_SPM.h"
#include "utils.h"
#include "SIM800L_SPM.h"
#include "Preferences.h"
#include "LED_SPM.h"
#include "time_SPM.h"
#include "rtc_SPM.h"

WiFiManager wm;
Preferences initSetupParams;

char CustomerIDInitChar[200];

char html[] = "<html><body><h2>GPRS Settings</h2><p>Following parameters will be necessary to open GPRS context</p></body></html>";
WiFiManagerParameter enteredCustomerId("CustomerId", CustomerIDInitChar, "", 50);
WiFiManagerParameter enteredSDConf("SDConf", "Do you want to save data on SD Card? (Yes/No)", "No", 3);
WiFiManagerParameter enteredGSMConfAlert("GSMConfAlert", "Enter your phone number in case you want to receive alert messages(It will charge SIM card owner). Test message will be sent to this number for validation", "No", 15);
WiFiManagerParameter enteredGSMConf("GSMConf", "Do you want to use GSM instead of WiFi? (Yes/No) (It will charge SIM card owner)", "No", 3, html, WFM_LABEL_BEFORE);
WiFiManagerParameter enteredGSMConfAPN("GSMConfAPN", "Provide APN for GPRS context", "", 30);
WiFiManagerParameter enteredGSMConfPIN("GSMConfPIN", "Provide SIM Card PIN if SIM is locked", "", 6);
WiFiManagerParameter enteredGSMConfGPRSUser("GSMConfGPRSUser", "Provide GPRS User if present", "", 30);
WiFiManagerParameter enteredGSMConfGPRSPass("GSMConfGPRSPass", "Provide GPRS Password if present", "", 30);


bool SDConf = 0;
bool GSMConf = 0;
bool AlertConf = 0;
String GSMConfAlertNum;
String CustomerId;
String GSMConfAPN;
String GSMConfPIN;
String GSMConfGPRSUser;
String GSMConfGPRSPass;

const char* CustomerId_c;
const char* GSMConfAPN_c;
const char* GSMConfPIN_c;
const char* GSMConfGPRSUser_c;
const char* GSMConfGPRSPass_c;

extern volatile LED_status status;

extern uint8_t SD_CARD_WRITE_ENABLE;

volatile void initWiFiManager(){
    // WiFi.mode(WIFI_STA);
    // wm.resetSettings();
    
    const char * menu[] = {"wifi"};
    wm.setMenu(menu, 1);
    wm.setBreakAfterConfig(true);
    wm.setSaveParamsCallback(saveParamCallback);
    wm.setConnectTimeout(20);
    wm.setSaveConnectTimeout(10);
    wm.setConnectRetries(5);
    wm.setCaptivePortalEnable(true);
    wm.setCustomMenuHTML("<h1>Smart Meter</h1><h3>menu html</h3>");
    // wm.setSaveConnect(false);

    unsigned long unique = getUnique();
    String pre_name = "SPM_";
    String unique_str = String(unique);
    String AP_name = pre_name + unique_str;

    char headElement[200];
    sprintf(headElement, "<h1>Smart Power Meter</h1><h4>Initial Setup for %s</h4>", AP_name.c_str());
    String CustomerIDInitString = "Enter Provided CustomerId (Previously configured for " + getCustomerId() + ")";
    strcpy(CustomerIDInitChar, CustomerIDInitString.c_str());

    wm.setCustomHeadElement(headElement);

    wm.addParameter(&enteredCustomerId);
    wm.addParameter(&enteredSDConf);

    if (getGSMSupport()){
        wm.addParameter(&enteredGSMConfAlert);
        wm.addParameter(&enteredGSMConf);
        wm.addParameter(&enteredGSMConfAPN);
        wm.addParameter(&enteredGSMConfPIN);
        wm.addParameter(&enteredGSMConfGPRSUser);
        wm.addParameter(&enteredGSMConfGPRSPass);
    }

    // unsigned long unique = getUnique();
    // String pre_name = "SPM_";
    // String unique_str = String(unique);
    // String AP_name = pre_name + unique_str;
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
            struct tm currtm;
            time_status ret = getCurrTimeUsingWiFi(&currtm);

            if (ret == TIME_OK){
                Serial.println("Now synchronizing time");
                rtcSetCurrDateTime(currtm.tm_year+1900,
                                   currtm.tm_mon+1,
                                   currtm.tm_mday,
                                   currtm.tm_hour,
                                   currtm.tm_min,
                                   currtm.tm_sec);
            }else{
                timeHandleError(ret);
            }
        }
    }

}


void saveParamCallback(){
    Serial.println("----------------------------->Callback");

    String customerId = String(enteredCustomerId.getValue());
    String SDConf = String(enteredSDConf.getValue());
    String GSMConfAlert = String(enteredGSMConfAlert.getValue());
    String GSMConf = String(enteredGSMConf.getValue());
    String GSMConfAPN = String(enteredGSMConfAPN.getValue());
    String GSMConfPIN = String(enteredGSMConfPIN.getValue());
    String GSMConfGPRSUser = String(enteredGSMConfGPRSUser.getValue());
    String GSMConfGPRSPass = String(enteredGSMConfGPRSPass.getValue());

    Serial.printf("CustomerId:      %s\n", customerId);
    Serial.printf("SDConf:          %s\n", SDConf);
    Serial.printf("GSMConfAlert:    %s\n", GSMConfAlert);
    Serial.printf("GSMConf:         %s\n", GSMConf);
    Serial.printf("GSMConfAPN:      %s\n", GSMConfAPN);
    Serial.printf("GSMConfPIN:      %s\n", GSMConfPIN);
    Serial.printf("GSMConfGPRSUser: %s\n", GSMConfGPRSUser);
    Serial.printf("GSMConfGPRSPass: %s\n", GSMConfGPRSPass);

    initSetupParams.begin("initParams");

    SDConf.toLowerCase();
    GSMConf.toLowerCase();
    GSMConfAlert.toLowerCase();

    bool sd_conf = false;
    if ((SDConf == String("yes")) || (SDConf == "y")){
        sd_conf = true;
    }

    bool gsm_conf = false;
    if ((GSMConf == String("yes")) || (GSMConf == "y")){
        gsm_conf = true;
    }

    if (!GSMConfAlert.isEmpty() && (GSMConfAlert != "no") && (GSMConfAlert != "n")){
        initSetupParams.putString("GSMAlertNum", GSMConfAlert);
        initSetupParams.putBool("GSMAlertConf", true);
    }else{
        initSetupParams.putBool("GSMAlertConf", false);
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

bool getSDConf(){
    initSetupParams.begin("initParams");

    bool param = initSetupParams.getBool("SDConf");

    initSetupParams.end();
    return param;
}

String getCustomerId(){
    initSetupParams.begin("initParams");
    String id = initSetupParams.getString("CustomerId");
    initSetupParams.end();

    return id;
}

String getGSMConfAPN(){
    initSetupParams.begin("initParams");
    String id = initSetupParams.getString("GSMConfAPN");
    initSetupParams.end();

    return id;
}

String getGSMConfPIN(){
    initSetupParams.begin("initParams");
    String id = initSetupParams.getString("GSMConfPIN");
    initSetupParams.end();

    return id;
}

String getGSMConfGPRSUser(){
    initSetupParams.begin("initParams");
    String id = initSetupParams.getString("GSMConfGPRSU");
    initSetupParams.end();

    return id;
}

String getGSMConfGPRSPass(){
    initSetupParams.begin("initParams");
    String id = initSetupParams.getString("GSMConfGPRSP");
    initSetupParams.end();

    return id;
}


void getAllConf(){
    initSetupParams.begin("initParams");

    SDConf          = initSetupParams.getBool("SDConf");
    GSMConf         = initSetupParams.getBool("GSMConf");
    AlertConf       = initSetupParams.getBool("GSMAlertConf");
    GSMConfAlertNum = initSetupParams.getString("GSMAlertNum");
    CustomerId      = initSetupParams.getString("CustomerId");
    GSMConfAPN      = initSetupParams.getString("GSMConfAPN");
    GSMConfPIN      = initSetupParams.getString("GSMConfPIN");
    GSMConfGPRSUser = initSetupParams.getString("GSMConfGPRSU");
    GSMConfGPRSPass = initSetupParams.getString("GSMConfGPRSP");

    Serial.println("---------> Reading All Configuration Parameters");
    Serial.printf("CustomerId:      %s\n", CustomerId);
    Serial.printf("SDConf:          %d\n", SDConf);
    Serial.printf("GSMConf:         %d\n", GSMConf);
    Serial.printf("GSMConfAlert:    %d\n", AlertConf);
    Serial.printf("GSMConfAlertNum: %s\n", GSMConfAlertNum);
    Serial.printf("GSMConfAPN:      %s\n", GSMConfAPN);
    Serial.printf("GSMConfPIN:      %s\n", GSMConfPIN);
    Serial.printf("GSMConfGPRSUser: %s\n", GSMConfGPRSUser);
    Serial.printf("GSMConfGPRSPass: %s\n", GSMConfGPRSPass);

    initSetupParams.end();

    CustomerId_c = CustomerId.c_str();
    GSMConfAPN_c = GSMConfAPN.c_str();
    GSMConfPIN_c = GSMConfPIN.c_str();
    GSMConfGPRSUser_c = GSMConfGPRSUser.c_str();
    GSMConfGPRSPass_c = GSMConfGPRSPass.c_str();

    if (SDConf) SD_CARD_WRITE_ENABLE = 1;
}

void setGSMConf(bool val){
    initSetupParams.begin("initParams");

    initSetupParams.putBool("GSMConf", val);

    initSetupParams.end();
}