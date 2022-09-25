/********************************************************
* 6-channel timer for Aquarium. Version 3.05 (06.02.20) *
* creator: Owintsowsky Maxim     22 772 / 22 954 (1633) *
* https://vk.com/automation4house  VK group             *
* https://t.me/aquatimer Telegram chat                  *
*********************************************************/

#include <Arduino.h>
#include <Wire.h>
#include "TimerOne.h"
#include "pitches.h"

#define VibroPin 15 //Pins for vibro-motor A1
#define BackLightPin 10 // Pins for LCD backlight
#define BACKLIGHT_DAY 1023  // Backlight for day
#define BACKLIGHT_NIGHT 220 // Backlight for night

#define RESET_CLOCK 0 // возврат часов на время загрузки прошивки. Не забудьте поставить 0 и прошить ещё раз!

//#define isDS1302 // Для DS1307 закомментируйте эту строку

/* Уберите две косые в начале следующей строки для теста без RTC */
//#define TEST_MODE 

const uint8_t TVer = 0x35;
boolean BeepOnOff = true; // Switch on/off beeper every hour
boolean KeyToneOn = true; // Switch on/off keys tone
uint16_t adc_key_val[5] ={100, 280, 400, 600, 850};  // Analog keyboard setting

#ifdef isDS1302 // for RTC DS1302
#define TonePin 18 //Pins for pieze A4 (DS1302)
uint8_t MaxNumChannels = 5; //Max number of channels
uint8_t ChPin[] = {11, 13, 16, 17, 19}; // Pins for channels (D11, D13, A2, A3, A5)
#else // for RTC DS1307
#define TonePin  17 //Pins for pieze A3 (DS1307)
uint8_t MaxNumChannels = 6; //Max number of channels
uint8_t ChPin[] = {11, 13, 2, 3, 16, 12}; // Pins for channels (D11, D13, D2, D3, A2, D12)
#endif
uint8_t ChannelsNum = 4; // Number of relay channels                          Screen:      0123456789ABCDEF 
uint8_t TimersNum = 24; // Number of timers
uint8_t RelayUp = HIGH; // Relay type

uint16_t FeedTime1 = 565; // Start time vibro 9:25
uint16_t FeedTime2 = 1181; // Second time vibro 19:41
uint8_t FeedDelay = 3; // Time for vibro * 100mS

#include "declare.h"
#include "clock.h"
#include "romfunc.h"
#include "lcdfunc.h"
#include "functions.h"
#include "button.h"
#include "menu.h"

void setup() {
  for (uint8_t ij = 0 ; ij < MaxNumChannels ; ij++) {
    pinMode(ChPin[ij], OUTPUT);
    digitalWrite(ChPin[ij], HIGH - RelayUp);
  }
  pinMode(VibroPin, OUTPUT);
  digitalWrite(VibroPin, LOW);
  pinMode(TonePin, OUTPUT);
  digitalWrite(TonePin, LOW);

  Wire.begin();
#ifndef isDS1302
  rtc.begin();
  //if (RESET_CLOCK || rtc.lostPower()) // For Adafrut RTClib
  if (RESET_CLOCK || !rtc.isrunning())
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // For Adafrut RTClib
    rtc.adjust(DateTime(__DATE__, __TIME__));
#else
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
#endif

  Timer1.initialize(5000);           // set timer № 1 every 5000 mks (= 10 мs)
  Timer1.attachInterrupt(timerIsr);		// timer start
  noInterrupts(); 
  //Serial.begin(9600); <-- убьёт таймер!!! 
  ButtonInit(debounce);

#ifdef LCD_I2C_MODE
  //lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
#else
  lcd.begin(16, 2);
#endif
  
  Timer1.pwm(BackLightPin, BACKLIGHT_DAY);
  LCDsetup();
  interrupts(); 
  ReadWriteEEPROM();
  ShowFeedingTime();
  if (myButt0.isHolded())  // Right holded
#ifndef KeySetup
    AnalogKeysTest();
#else
    if (AnalogKeySet()) {}
#endif
  myDelay(2000);
}

void loop() { // Main sketch loop
#ifndef TEST_MODE
  if (clockTimer.isReady()) {
#ifndef isDS1302    
    t = rtc.now();     // Get data from the DS1307
    if (oldsec != t.second()) {
      oldsec = t.second();
      snprintf(LCDbuff0, 17, "%3s, %02d %3s %04d", dn[t.dayOfTheWeek()], t.day(), mn[t.month()-1], t.year());
#else
    t = rtc.getTime(); // Get data from the DS1302
    if (oldsec != t.sec) {
      oldsec = t.sec;
      snprintf(LCDbuff0, 17, "%3s, %02d %3s %04d", dn[t.dow], t.date, mn[t.mon - 1], t.year);
#endif    
      LCDbprint(1);
      lcd.setCursor(0, 1);
#ifndef isDS1302
      TimeNow = t.hour()*60+t.minute();
      if ((TimeNow == 210) && (t.second() == 30) && (TimeAdj != 0)) TimeAdjusting();
      if (NotFeeding(TimeNow)) {
      snprintf(LCDbuff1, 17, "%02d:%02d:%02d \0", t.hour(), t.minute(), t.second(), '\0');
#else
      TimeNow = t.hour*60+t.min;
      if ((TimeNow == 210) && (t.sec == 30) && (TimeAdj != 0)) TimeAdjusting();
      if (NotFeeding(TimeNow)) {
      snprintf(LCDbuff1, 17, "%02d:%02d:%02d \0", t.hour, t.min, t.sec );
#endif
      LCDbprint(2);
      if (!BLNightState) HourBeep(TimeNow);
      } else FeedStart();
    } 
  }  
  if (rotateTimer.isReady()) {
    lcd.setCursor(9, 1);
#ifndef isDS1302
    lcd.print(TimerOnOffDisp(StateChannels(TimeNow, t.dayOfTheWeek())));
#else
    lcd.print(TimerOnOffDisp(StateChannels(TimeNow, t.dow)));
#endif
  }
  if (myButt0.isClick()) { // Right
    if (NeedOnOff & 0x80) {
      if (!(NeedOnOff & 0x20)) NeedOnOff |= 0x40;
      else NeedOnOff &= 0xdf;
    } else {
      if (!(NeedOnOff & 0x40)) NeedOnOff |= 0x20;
      else NeedOnOff &= 0xb7;
    }
    KeyTone();
  }
  if (myButt0.isHolded()) { // Right holded
    KeyTone();
    //ButtonReset();
    SysMenuSelect();
  }
  if (myButt1.isClick()) { // Up
    if (BLNightState == 0) {
      if (!(NeedOnOff & 1)) NeedOnOff |= 2;
      else NeedOnOff &= 0xFE;
    } else {
      if (!(NeedOnOff & 0x02)) NeedOnOff |= 1;
      else NeedOnOff &= 0xFD;
    }
    KeyTone();
  }
  if (myButt1.isHolded()) { // Up holded
    KeyTone();
    MenuBackLightSet();
  }
  if (myButt2.isClick()) { // Down
    KeyTone();
    lcd.setCursor(9, 1);
    FeedStart();
  }
  if (myButt2.isHolded()) { // Down holded
    KeyTone();
    FeedMenu();
  }
  if (myButt3.isClick()) { // Left
    if (NeedOnOff & 0b00010000) { // Ch1 on
      if (!(NeedOnOff & 0b00000100)) NeedOnOff |= 0b00001000; // Ch1 need on
      else NeedOnOff &= 0b11111011;
    } else { // Ch1 off
      if (!(NeedOnOff & 0x08)) NeedOnOff |= 0x04;
      else NeedOnOff &= 0b11110111;
    }
    KeyTone();
  }
  if (myButt3.isHolded()) { // Left holded
    KeyTone();
    SetTimers();
  }
  if (myButt4.isClick()) { // Menu
    KeyTone();
    MenuSelect();
  }
  if (myButt4.isHolded()) { // Menu holded
    KeyTone();
    TimeSetup();
  }
#else // for test mode
  for (uint8_t dw = 1; dw < 8 ; dw++) {
    for (uint16_t ct = 0 ; ct < 12*24 ; ct++) {
      lcd.setCursor(0, 0);
      lcd.print(dn[dw-1]);
      lcd.print(F(", 1"));
      lcd.print(dw+2);           
      lcd.print(F(" Jan 2020"));   
      lcd.setCursor(0, 1);
      lcd.print(lid1Zero(ct/12));
      lcd.write(58); // :
      lcd.print(lid1Zero((ct*5)%60));
      lcd.print(F(":00 "));
      if (NotFeeding(5*ct)) {
        lcd.print(TimerOnOffDisp(StateChannels(5*ct, dw)));
        if (!BLNightState) HourBeep(5*ct);
      } else FeedStart();
      myDelay(100);
      if (myButt0.isClick()) { // Right
        if (NeedOnOff & 0x80) {
          if (!(NeedOnOff & 0x20)) NeedOnOff |= 0x40;
          else NeedOnOff &= 0xdf;
        } else {
          if (!(NeedOnOff & 0x40)) NeedOnOff |= 0x20;
          else NeedOnOff &= 0xb7;
        }
        KeyTone();
        myDelay(500);
      }
      if (myButt1.isClick()) { // Up
        if (BLNightState == 0) {
          if (!(NeedOnOff & 1)) NeedOnOff |= 2;
          else NeedOnOff &= 0xFE;
        } else {
          if (!(NeedOnOff & 0x02)) NeedOnOff |= 1;
          else NeedOnOff &= 0xFD;
        }
        KeyTone();
        myDelay(200);
      }
      if (myButt2.isClick()) { // Down
        KeyTone();
        lcd.setCursor(9, 1);
        FeedStart();
      }
      if (myButt3.isClick()) { // Left
        if (NeedOnOff & 0x10) {
          if (!(NeedOnOff & 0x04)) NeedOnOff |= 0x08;
          else NeedOnOff &= 0xfd;
        } else {
          if (!(NeedOnOff & 0x08)) NeedOnOff |= 0x04;
          else NeedOnOff &= 0xf7;
        }
        KeyTone();
        myDelay(500);
      }
      if (myButt4.isClick()) { // Menu
        KeyTone();
        MenuSelect();
      }
    }
  }
#endif
}
