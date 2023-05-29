#include "LED_SPM.h"

Adafruit_NeoPixel led(NUMPIXELS, USR_LED, NEO_RGB + NEO_KHZ800);


volatile LED_status status;


void initLED(){
    pinMode(USR_LED, OUTPUT);
    digitalWrite(USR_LED, LOW);

    led.begin();
}


