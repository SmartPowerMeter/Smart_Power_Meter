#include "LED_SPM.h"
#include "WiFiManager_SPM.h"

Adafruit_NeoPixel led(NUMPIXELS, USR_LED, NEO_RGB + NEO_KHZ800);


volatile LED_status status = STARTING;
volatile LED_status status_prev = STARTING;
struct RGB led1, led2;
unsigned long time1 = 1000000, time2 = 1000000;
extern hw_timer_t * LEDInterrupt;
extern bool temp_led_flag;
bool temp_led_flag_prev = 0;

extern WiFiManager wm;

void initLED(){
    pinMode(USR_LED, OUTPUT);
    digitalWrite(USR_LED, LOW);

    led.begin();
}


void ledLoop(){
    switch (status){
      case CONNECTED:
        led1.RED = 0; led1.GREEN = 250; led1.BLUE = 0;
        led2.RED = 0; led2.GREEN = 0; led2.BLUE = 0;
        time1 = 100000; time2 = 900000;
        break;
      case RECONNECTING:
        led1.RED = 100; led1.GREEN = 0; led1.BLUE = 0;
        led2.RED = 0; led2.GREEN = 0; led2.BLUE = 100;
        time1 = 200000; time2 = 200000;
        break;
      case IN_CONFIGURATION_MODE:
        led1.RED = 0; led1.GREEN = 0; led1.BLUE = 250;
        led2.RED = 0; led2.GREEN = 250; led2.BLUE = 0;
        time1 = 700000; time2 = 300000;
        break;
      case RESTARTING:
        led1.RED = 250; led1.GREEN = 0; led1.BLUE = 0;
        led2.RED = 0; led2.GREEN = 0; led2.BLUE = 0;
        time1 = 100000; time2 = 100000;
        break;
      case STARTING:
        led1.RED = 250; led1.GREEN = 250; led1.BLUE = 250;
        led2.RED = 0; led2.GREEN = 0; led2.BLUE = 0;
        time1 = 1000000; time2 = 1000000;
        break;
      case STARTING_OVER_GSM:
        led1.RED = 160; led1.GREEN = 32; led1.BLUE = 240;
        led2.RED = 0; led2.GREEN = 0; led2.BLUE = 0;
        time1 = 1000000; time2 = 1000000;
        break;
      default:
        led1.RED = 0; led1.GREEN = 0; led1.BLUE = 0;
        led2.RED = 0; led2.GREEN = 0; led2.BLUE = 0;
        time1 = 1000000; time2 = 1000000;
        break;
    }

    if (((temp_led_flag == 1) && (temp_led_flag_prev == 0)) || (status != status_prev)){
        // Serial.println("if");

        status_prev = status;
        temp_led_flag_prev = temp_led_flag;
        led.clear();
        led.setPixelColor(0, led.Color(led1.RED, led1.GREEN, led1.BLUE));
        led.show();
        timerAlarmWrite(LEDInterrupt, time1, true);
    }else if (((temp_led_flag == 0) && (temp_led_flag_prev == 1)) || (status != status_prev)){
        // Serial.println("else");

        status_prev = status;
        temp_led_flag_prev = temp_led_flag;
        led.clear();
        led.setPixelColor(0, led.Color(led2.RED, led2.GREEN, led2.BLUE));
        led.show();
        timerAlarmWrite(LEDInterrupt, time2, true);
    }


    // Serial.printf("last conf:             %d\n", wm.getLastConxResult());
    // Serial.printf("getConfigPortalActive: %d\n", wm.getConfigPortalActive());
    // Serial.printf("getWebPortalActive:    %d\n", wm.getWebPortalActive());
    // Serial.printf("WiFi status:           %d\n", WiFi.status());
    // delay(100);
    
    if (wm.getConfigPortalActive()){
        status = IN_CONFIGURATION_MODE;
    }
}


