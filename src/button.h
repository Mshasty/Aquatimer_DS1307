#include "GyverButton.h"
#include <GyverTimer.h>

//╒========================╕
//|  Настройка клавиатуры  |
//╘========================╛
#define KeySetup

GButton myButt0(14, HIGH_PULL, NORM_OPEN);
GButton myButt1(14, HIGH_PULL, NORM_OPEN);
GButton myButt2(14, HIGH_PULL, NORM_OPEN);
GButton myButt3(14, HIGH_PULL, NORM_OPEN);
GButton myButt4(14, HIGH_PULL, NORM_OPEN);

GTimer_ms keypadTimer(10);
GTimer_ms clockTimer(333);
GTimer_ms rotateTimer(280);  
#define debounce 45

void timerIsr() {   // timer interrapt
  uint16_t analog = analogRead(0);
  myButt4.tick(analog > adc_key_val[3] && analog < adc_key_val[4]); // Menu 641 - 663
  myButt3.tick(analog > adc_key_val[2] && analog < adc_key_val[3]); // Left 409 - 427
  myButt2.tick(analog > adc_key_val[1] && analog < adc_key_val[2]); // Down 255 - 287
  myButt1.tick(analog > adc_key_val[0] && analog < adc_key_val[1]); // Up   99 - 127
  myButt0.tick(analog >= 0 && analog < adc_key_val[0]);             // Right 0 - 93 
}

void ButtonInit(uint16_t myDebounce) {
  // Right button
  myButt0.setDebounce(myDebounce);  // настройка антидребезга (по умолчанию 80 мс)
  myButt0.setTimeout(600);          // настройка таймаута на удержание (по умолчанию 500 мс)  
  // Up button
  myButt1.setDebounce(myDebounce);
  myButt1.setTimeout(400);
  // Down button
  myButt2.setDebounce(myDebounce);
  myButt2.setTimeout(400);
  // Left button
  myButt3.setDebounce(myDebounce);
  myButt3.setTimeout(600);
  // Menu button
  myButt4.setDebounce(myDebounce);
  myButt4.setTimeout(800);
}


void KeyTone() {
  if (KeyToneOn) tone(TonePin, NOTE_DS6, 15);
}

void ErrorBeep() {
  if (KeyToneOn) {
    tone(TonePin, NOTE_DS5, 50);
    myDelay(100);
    tone(TonePin, NOTE_DS4, 100);
  }
}

void ButtonReset() {
  noInterrupts();
  myButt0.isHolded();
  myButt1.isHolded();
  myButt2.isHolded();
  myButt3.isHolded();
  myButt4.isHolded();
  interrupts();
}

#ifndef KeySetup
void AnalogKeysTest() {
  lcd.noCursor();
  lcd.noBlink();
  snprintf(LCDbuff0, 17, " Press right key");
  snprintf(LCDbuff1, 17, "    for exit    ");
  LCDbprint(3);
  myDelay(1000);
  snprintf(LCDbuff0, 17, "Analog keys test");
  snprintf(LCDbuff1, 17, "Input:          ");
  LCDbprint(3);
  uint16_t analog;
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1; ) { // Read key
    if((millis()-TimeStart) > 45000) MenuExit = 1;
    lcd.setCursor(7, 1);
    analog = analogRead(0);
    lcd.print(analog);
    lcd.print(F("  "));
    myDelay(200);
    if (analog < 1000) TimeStart=millis();
    if (myButt0.isClick()) { // Right
      KeyTone();
      myDelay(300);
      MenuExit = 1;
    }
    if (analog > adc_key_val[3] && analog < adc_key_val[4]) { // Menu 641 - 663
      lcd.setCursor(12, 1);
      lcd.print(F("Menu"));
    }
    if (analog > adc_key_val[2] && analog < adc_key_val[3]) { // Left 409 - 427
      lcd.setCursor(12, 1);
      lcd.print(F("Left"));
    }
    if (analog > adc_key_val[1] && analog < adc_key_val[2]) { // Down 255 - 287
      lcd.setCursor(12, 1);
      lcd.print(F("Down"));
    }
    if (analog > adc_key_val[0] && analog < adc_key_val[1]) { // Up   99 - 127
      lcd.setCursor(14, 1);
      lcd.print(F("Up"));
    }
    if (analog >= 0 && analog < adc_key_val[0]) {             // Right 0 - 93 
      lcd.setCursor(11, 1);
      lcd.print(F("Right"));
    }
    if (analog >= adc_key_val[4]) {             // Right 0 - 93 
      lcd.setCursor(11, 1);
      lcd.print(F("     "));
    }
  }
  ButtonReset();
} 
#else
boolean AnalogKeySet() {
  noInterrupts(); 
  boolean KeySet = false;
  lcd.noCursor();
  lcd.noBlink();
  snprintf(LCDbuff0, 17, "  Analog keys   ");
  snprintf(LCDbuff1, 17, " configuration  ");
  LCDbprint(3);
  myDelay(1000);
  byte KeySetting = 0;
  uint16_t analog;
  String KeyName[4] = {"MENU", "LEFT", "DOWN", "UP  "};
  uint16_t KeyConfig[5] = {1023, 1023, 1023, 1023, 1023};
  unsigned long TimeStart=millis();
  unsigned long KeyTime;
  for(uint8_t MenuExit = 0; MenuExit < 1; ) { // Read key
    if((millis()-TimeStart) > 12000) MenuExit = 1;
    for (uint8_t i=0; i<4; i++) {
      snprintf(LCDbuff0, 17, " Press key      ");
      snprintf(LCDbuff1, 17, "Input:          ");
      LCDbprint(3);
      lcd.setCursor(10, 0);
      lcd.print(KeyName[i]);
      KeyTime = millis();
      while (millis() < KeyTime + 5000) {
        lcd.setCursor(7, 1);
        analog = analogRead(0);
        if (analog < 1020) {
          lcd.print(analog);
          lcd.print(F("  "));
          if (analog < KeyConfig[4 - i]) {
            KeyConfig[4 - i] = analog;
            KeySetting |= (1<<i);
          }
        } else lcd.print(F("    "));
        myDelay(100);
      }
      KeyTone();
    }
    //adc_key_val[0]
    snprintf(LCDbuff0, 17, "Return key input");
    LCDbprint(1);
    lcd.setCursor(0, 1);
    for (uint8_t j=1; j<5; j++) {
      lcd.print(KeyConfig[j]);
      if (j < 4) lcd.print(",");
    }
    myDelay(2000);
    if (KeySetting >= 0xF) {
      snprintf(LCDbuff0, 17, "All key settings");
      LCDbprint(1);
      myDelay(2000);
      MenuExit = 1;
    } else {
      snprintf(LCDbuff0, 17, "%12s %04b", "Key set:  0b", KeySetting);
      LCDbprint(1);
      myDelay(2000);
    }
  }
  return KeySet;
  interrupts(); 
  ButtonReset();
}
#endif