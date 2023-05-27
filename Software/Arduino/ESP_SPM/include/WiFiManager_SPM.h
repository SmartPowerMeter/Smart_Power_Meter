#ifndef WIFIMANAGER_SPM
#define WIFIMANAGER_SPM

#include "WiFiManager.h"

volatile void initWiFiManager();
void saveParamCallback();
bool getGSMConf();
void setGSMConf(bool val);

#endif