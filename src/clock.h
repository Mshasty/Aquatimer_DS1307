#ifndef isDS1302
#include "RTClib.h"

DS1307 rtc; // Init the DS1307
// Init a Time-data structure
DateTime t;
#else
#include "DS1302.h"
DS1302 rtc(2, 3, 12); // Init the DS1302
// DS1302:  CE pin    -> Arduino Digital 2
//          I/O pin   -> Arduino Digital 3
//          SCLK pin  -> Arduino Digital 12

// Init a Time-data structure
Time t;
#endif

uint16_t TimeNow;

void myDelay(uint16_t deltm) {
  unsigned long TimeStop = deltm + millis();
  while (millis() < TimeStop);      
}


