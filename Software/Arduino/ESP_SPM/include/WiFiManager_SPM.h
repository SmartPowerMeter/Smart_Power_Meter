#ifndef WIFIMANAGER_SPM
#define WIFIMANAGER_SPM

#include "WiFiManager.h"

volatile void initWiFiManager();
void saveParamCallback();
bool getGSMConf();
bool getSDConf();
String getCustomerId();
String getGSMConfAPN();
String getGSMConfPIN();
String getGSMConfGPRSUser();
String getGSMConfGPRSPass();
void getAllConf();

void setGSMConf(bool val);

#endif