#ifndef LED_SPM
#define LED_SPM

#include "Hardware_SPM.h"

#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
 #include "avr/power.h"
 #endif

#define NUMPIXELS 1


typedef enum{
  CONNECTED = 0,
  RECONNECTING,
  IN_CONFIGURATION_MODE,
  RESTARTING,
  STARTING
} LED_status;


struct RGB
{
  uint8_t RED;
  uint8_t GREEN;
  uint8_t BLUE;
};

void initLED();


#endif