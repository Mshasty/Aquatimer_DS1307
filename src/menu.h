void TimeSetup() {
#ifndef TEST_MODE
#ifndef isDS1302
  uint8_t CurTab[5] = {2, 5, 8, 11, 14};
  uint8_t CurPos = 0;
  DateTime tt = rtc.now();
  uint8_t SetDoW = tt.dayOfTheWeek()+1;
  uint8_t SetDay = tt.day();
  uint8_t SetMonth = tt.month();
  uint16_t SetYear = tt.year() - 2000;
  uint8_t SetHour = tt.hour();
  uint8_t SetMin = tt.minute();
  uint8_t MaxPos = 4;
#else
  uint8_t CurTab[6] = {3, 6, 9, 12, 15, 0};
  uint8_t CurPos = 5;
  Time tt = rtc.getTime();
  uint8_t SetDoW = tt.dow;
  uint8_t SetDay = tt.date;
  uint8_t SetMonth = tt.mon;
  uint16_t SetYear = tt.year - 2000;
  uint8_t SetHour = tt.hour;
  uint8_t SetMin = tt.min;
  uint8_t MaxPos = 5;
#endif
  Timer1.setPwmDuty(BackLightPin, 1023);
  lcd.setCursor(0, 0);
  lcd.print(F("Set Date & Time:"));
  TimeToLCD(&SetDoW, &SetDay, &SetMonth, &SetYear, &SetHour, &SetMin, CurTab[CurPos]);
  //myDelay(1000);
  boolean changeDisp = false;
  lcd.blink();
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1 ; ) {
    //Read_Key();
    if (myButt0.isClick()) { // Right
      if (CurPos < MaxPos) CurPos++; 
      else CurPos = 0;
      changeDisp = true;
    }
    if (myButt1.isClick() || myButt1.isStep()) { // Up
      switch( CurPos ) {
        case 5:
          if (SetDoW < 7) SetDoW++;
          else SetDoW = 1;
          break;
        case 0:
          if (SetDay < 31) SetDay++;
          else SetDay = 1;
          break;
        case 1:
          if (SetMonth < 12) SetMonth++;
          else SetMonth = 1;
          break;
        case 2:
          if (SetYear < 99) SetYear++;
          else SetYear = 0;
          break;
        case 3:
          if (SetHour < 23) SetHour++;
          else SetHour = 0;
          break;
        case 4:
          if (SetMin < 59) SetMin++;
          else {
            SetMin = 0;
            if (SetHour < 23) SetHour++;
            else SetHour = 0;
          }
          break;
      }
      changeDisp = true;
    }
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      switch( CurPos ) {
        case 5:
          if (SetDoW > 1) SetDoW--;
          else SetDoW = 7;
          break;
        case 0:
          if (SetDay > 1) SetDay--;
          else SetDay = 31;
          break;
        case 1:
          if (SetMonth > 1) SetMonth--;
          else SetMonth = 12;
          break;
        case 2:
          if (SetYear > 0) SetYear--;
          else SetYear = 99;
          break;
        case 3:
          if (SetHour > 0) SetHour--;
          else SetHour = 23;
          break;
        case 4:
          if (SetMin > 0) {
            SetMin--;
          } else {
            SetMin = 59;
            if (SetHour > 0) SetHour--;
            else SetHour = 23;
          }
          break;
      }
      changeDisp = true;
    }
    if (myButt3.isClick()) { // Left
      if (CurPos > 0) CurPos--; 
      else CurPos = MaxPos;
      changeDisp = true;
    }
    if (myButt4.isClick()) { // Menu
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      KeyTone();
#ifndef isDS1302      
      rtc.adjust(DateTime((2000 + SetYear), SetMonth, SetDay, SetHour, SetMin, 0));
#else      
      rtc.setDate(SetDay, SetMonth, 2000 + SetYear);
      rtc.setTime(SetHour, SetMin, 0);
      rtc.setDOW(SetDoW);
#endif      
      MenuExit = 1;
    }
    if (changeDisp) {
      TimeStart=millis();
      KeyTone();
      TimeToLCD(&SetDoW, &SetDay, &SetMonth, &SetYear, &SetHour, &SetMin, CurTab[CurPos]);
      changeDisp = false;
    }
    //myDelay(10);
    if((millis()-TimeStart) > 130000) MenuExit = 1;
  }
#endif
  lcd.noBlink();
  ButtonReset();
}

void SetOneTimer(uint8_t CurTimer) { // Setting current timer parametrs
  uint8_t daysofweek[] = {127, 31, 96, 1, 2, 4, 8, 16, 32, 64, 0x15, 0x55, 0x2A, 0}; // States in the week
  uint8_t WeekStateNum; // Total state options in the week
  WeekStateNum = sizeof(daysofweek); 	// Number of states in the week
  uint8_t CurWeekMode;
  uint8_t CurPos = 0;
  uint8_t CurTab[6] = {0, 2, 6, 9, 12, 15};
  for (CurWeekMode=0; CurWeekMode < WeekStateNum; CurWeekMode++) {
    if (ChOnOff[0] == daysofweek[CurWeekMode]) break;
  }
  lcd.blink();
  lcd.setCursor(2, 0);
  lcd.write(SETPIC);
  lcd.setCursor(2, 0);
  myDelay(450);
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1 ; ) {
    //Read_Key();
    if (myButt0.isClick()) { // Right
      TimeStart=millis();
      KeyTone();
      if (CurPos > 4) CurPos = 0;
      else CurPos++;
      if (CurPos > 0) {
        lcd.setCursor(2, 0);
        lcd.write(32);
        lcd.setCursor(CurTab[CurPos], 1);
      } else {
        lcd.setCursor(2, 0);
        lcd.write(SETPIC);
        lcd.setCursor(2, 0);
      }
    } 
    if (myButt1.isClick() || myButt1.isStep()) { //Up
      TimeStart=millis();
      KeyTone();
      if (CurPos > 0) {
        if (CurPos > 1) {
          if (ChOnOff[CurPos]<(23+(CurPos%2)*32)) // 23 or 55
            ChOnOff[CurPos]+=((CurPos%2)*4+1);    // 1 or 5
          else ChOnOff[CurPos]=0;
        } else {
          if (ChOnOff[1] < (ChannelsNum-1)) ChOnOff[1]++;
          else ChOnOff[1]=0;
        }
        TimerDisp();
        lcd.setCursor(CurTab[CurPos], 1);
      } else {
        if (CurWeekMode < (WeekStateNum-1)) CurWeekMode++;
        else CurWeekMode = 0;
        ChOnOff[0] = daysofweek[CurWeekMode];
        TimerDisp();
        lcd.setCursor(2, 0);
        lcd.write(SETPIC);
        lcd.setCursor(2, 0);
      }
    } 
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      TimeStart=millis();
      KeyTone();
      if (CurPos > 0) {
        if (CurPos > 1) {
          if (ChOnOff[CurPos]>0) // 23 or 55
            ChOnOff[CurPos]-=((CurPos%2)*4+1);      // 1 or 5
          else
            ChOnOff[CurPos]=(23+(CurPos%2)*32);
        } else {
          if (ChOnOff[1] > 0) ChOnOff[1]--;
          else ChOnOff[1]=ChannelsNum-1;
        }
        TimerDisp();
        lcd.setCursor(CurTab[CurPos], 1);
      } else {
        if (CurWeekMode > 0) CurWeekMode--;
        else CurWeekMode = (WeekStateNum-1);
        ChOnOff[0] = daysofweek[CurWeekMode];
        TimerDisp();
        lcd.setCursor(2, 0);
        lcd.write(SETPIC);
        lcd.setCursor(2, 0);
      }
    } 
    if (myButt3.isClick()) { // Left
      TimeStart=millis();
      KeyTone();
      if (CurPos > 0) CurPos--;
      else CurPos = 5;
      if (CurPos > 0) {
        lcd.setCursor(2, 0);
        lcd.write(32);
        lcd.setCursor(CurTab[CurPos], 1);
      }
      else {
        lcd.setCursor(2, 0);
        lcd.write(SETPIC);
        lcd.setCursor(2, 0);
      }
    } if (myButt4.isClick()) { // Menu
      KeyTone();
      if (((60*ChOnOff[4]+ChOnOff[5]) < (60*ChOnOff[2]+ChOnOff[3])) && (ChOnOff[0] > 0)) {
        ChOnOff[0] = 127;
        snprintf(LCDbuff0, 17, "Timer night set!");
        snprintf(LCDbuff1, 17, "Full week select");
        LCDbprint(3);
        myDelay(670);
      }
      eeWrite(ChOnOff, TmrAddr + CurTimer*sizeof(ChOnOff), sizeof(ChOnOff));
      MenuExit = 1;
    } // End key read
    if((millis()-TimeStart) > 60000) MenuExit = 1;
  }    
}

void SetTimers() { // Select Timer from Menu
  Timer1.setPwmDuty(BackLightPin, 1023);
  uint8_t CurTimer = 0;
  ShowTimer(CurTimer);
  myDelay(450);
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1 ; ) {
    //Read_Key();
    if (myButt0.isClick()) { // Right
      KeyTone();
      SetOneTimer(CurTimer);
      ShowTimer(CurTimer);
      myDelay(500);
    } 
    if (myButt1.isClick() || myButt1.isStep()) { //Up
      KeyTone();
      if (CurTimer < (TimersNum-1)) CurTimer++;
      else CurTimer=0;
      TimeStart=millis();
      ShowTimer(CurTimer);
    } 
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      KeyTone();
      if (CurTimer > 0) CurTimer--;
      else CurTimer = TimersNum - 1;
      TimeStart=millis();
      ShowTimer(CurTimer);
    } 
    if (myButt3.isClick()) { // Left
      KeyTone();
      MenuExit = 1;
    } 
    if (myButt4.isClick()) { // Menu
      KeyTone();
      MenuExit = 1;
    } // End key read
    //myDelay(33);
    if((millis()-TimeStart) > 30000) MenuExit = 1;
  }    
  lcd.noBlink();
  ButtonReset();
}

void FeedMenu() { // Setting the time and duration of feeding
  Timer1.setPwmDuty(BackLightPin, 1023);
  lcd.blink();
  uint8_t CurPos = 0;
  uint8_t FeedHour1 = EEPROM.read(MEMO_FeedHour1);
  uint8_t FeedMin1 = EEPROM.read(MEMO_FeedMin1);
  uint8_t FeedHour2 = EEPROM.read(MEMO_FeedHour2);
  uint8_t FeedMin2 = EEPROM.read(MEMO_FeedMin2);
  FeedDisp(CurPos);
  myDelay(500);
  boolean changeDisp = false;
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1 ; ) {
    //Read_Key();
    if (myButt0.isClick()) { // Right
      if (CurPos > 3) CurPos = 0;
      else CurPos++;
      changeDisp = true;
    } 
    if (myButt1.isClick() || myButt1.isStep()) { //Up
      TimeStart=millis();
      changeDisp = true;
      switch( CurPos ) {
        case 0:
          if (FeedHour1 < 23) FeedHour1++;
          else FeedHour1 = 0;
          break;
        case 1:
          if (FeedMin1 < 59) FeedMin1++;
          else FeedMin1 = 0;
          break;
        case 2:
          if (FeedHour2 < 24) FeedHour2++;
          else FeedHour2 = 0;
          break;
        case 3:
          if (FeedMin2 < 59) FeedMin2++;
          else FeedMin2 = 0;
          break;
        case 4:
          if (FeedDelay < 120) FeedDelay++;
          else FeedDelay = 0;
          break;
      }
    } 
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      TimeStart=millis();
      changeDisp = true;
      switch( CurPos ) {
        case 0:
          if (FeedHour1 > 0) FeedHour1--;
          else FeedHour1 = 23;
          break;
        case 1:
          if (FeedMin1 > 0) FeedMin1--;
          else FeedMin1 = 59;
          break;
        case 2:
          if (FeedHour2 > 0) FeedHour2--;
          else FeedHour2 = 24;
          break;
        case 3:
          if (FeedMin2 > 0) FeedMin2--;
          else FeedMin2 = 59;
          break;
        case 4:
          if (FeedDelay > 0) FeedDelay--;
          else FeedDelay = 120;
          break;
      }
    } 
    if (myButt3.isClick()) { // Left
      if (CurPos < 1) CurPos = 4;
      else CurPos--;
      changeDisp = true;
    } 
    if (myButt4.isClick()) { // Menu
      KeyTone();
      MenuExit = 1;
    } // End key read
    if (changeDisp) {
      EEPROM.write(MEMO_FeedHour1, FeedHour1);
      EEPROM.write(MEMO_FeedMin1, FeedMin1);
      EEPROM.write(MEMO_FeedHour2, FeedHour2);
      EEPROM.write(MEMO_FeedMin2, FeedMin2);
      EEPROM.write(MEMO_FeedDelay, FeedDelay);
      FeedDisp(CurPos);
      KeyTone();
      changeDisp = false;
    }
    if((millis()-TimeStart) > 30000) MenuExit = 1;
  } 
  FeedTime1 = FeedHour1*60 + FeedMin1;
  FeedTime2 = FeedHour2*60 + FeedMin2;
  lcd.noBlink();
  ButtonReset();
}

void SubChangeMenu(uint8_t SysMenuNum) { // System submenu
  uint8_t LastItem = 0;
  SubMenuDisp(LastItem, SysMenuNum);
  myDelay(500);
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1 ; ) { // Read key
    if (myButt0.isClick()) { // Right
      EEPROM.write(MEMO_RelayUp+SysMenuNum, LastItem);
      myDelay(30);
      RelayUp=HIGH-EEPROM.read(MEMO_RelayUp);
      BeepOnOff=EEPROM.read(MEMO_BeepOnOff);
      KeyToneOn=EEPROM.read(MEMO_KeyToneOn);
      KeyTone();
      SubMenuDisp(LastItem, SysMenuNum);
      myDelay(700);
      MenuExit = 1;
    } 
    if (myButt1.isClick()) { //Up
      if (LastItem == 1) {
        LastItem = 0;
        TimeStart=millis();
        KeyTone();
        SubMenuDisp(LastItem, SysMenuNum);
      } else ErrorBeep();
    } 
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      if (LastItem == 0) {
        LastItem = 1;
        TimeStart=millis();
        KeyTone();
        SubMenuDisp(LastItem, SysMenuNum);
      } else ErrorBeep();
    } 
    if (myButt3.isClick()) { // Left
      KeyTone();
      MenuExit = 1;
    } 
    if (myButt4.isClick()) { // Menu
      KeyTone();
      MenuExit = 1;
    } // End key read
    if((millis()-TimeStart) > 20000) MenuExit = 1;
  }
  lcd.noBlink();
}

void MenuChannelsNum() {
    snprintf(LCDbuff0, 17, " Set the number ");
    snprintf(LCDbuff1, 17, " of channels:   ");
    LCDbprint(3);
    DispNumOfChannels();
    boolean changeDisp = false;
    unsigned long TimeStart=millis();
    for(uint8_t MenuExit = 0; MenuExit < 1 ; ) {
        //Read_Key();
        if (myButt0.isClick()) { // Right
            EEPROM.write(MEMO_ChannelsNum, ChannelsNum);
    	    	KeyTone();
            MenuExit = 1;
        } 
      	if (myButt1.isClick() || myButt1.isStep()) { //Up
            if (ChannelsNum < MaxNumChannels) ChannelsNum++;
            else ChannelsNum = 2;
        		changeDisp = true;
        } 
      	if (myButt2.isClick() || myButt2.isStep()) { // Down
            if (ChannelsNum > 2) ChannelsNum--;
            else ChannelsNum = MaxNumChannels;
    		    changeDisp = true;
        } 
      	if (myButt3.isClick()) { // Left
            ChannelsNum = EEPROM.read(MEMO_ChannelsNum);
        		KeyTone();
            MenuExit = 1;
        } 
      	if (myButt4.isClick()) { // Menu
            EEPROM.write(MEMO_ChannelsNum, ChannelsNum);
        		KeyTone();
                MenuExit = 1;
        } // End key read
      	if (changeDisp) {
        		KeyTone();
        		DispNumOfChannels();
        		changeDisp = false;
        		TimeStart=millis();
      	}
        if((millis()-TimeStart) > 15000) {
            ChannelsNum = EEPROM.read(MEMO_ChannelsNum);
            MenuExit = 1;
        }
    }    
    ShowChannels();
    myDelay(500);
}

void MenuSetTimeAdjust() {
  snprintf(LCDbuff0, 17, "Set the value of");
  snprintf(LCDbuff1, 17, "time adjust: %3d", TimeAdj);
  LCDbprint(3);
  //myDelay(350);
  boolean changeDisp = false;
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1 ; ) {
    //Read_Key();
    if (myButt0.isClick()) { // Right
      EEPROM.write(MEMO_TimeAdj, TimeAdj);
      KeyTone();
      MenuExit = 1;
    } 
    if (myButt1.isClick() || myButt1.isStep()) { //Up
      if (TimeAdj < 29) TimeAdj++;
      else ErrorBeep();
      changeDisp = true;
    } 
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      if (TimeAdj > -30) TimeAdj--;
      else ErrorBeep();
      changeDisp = true;
    } 
    if (myButt3.isClick()) { // Left
      TimeAdj = EEPROM.read(MEMO_TimeAdj);
    KeyTone();
      MenuExit = 1;
    } 
    if (myButt4.isClick()) { // Menu
      EEPROM.write(MEMO_TimeAdj, TimeAdj);
    KeyTone();
      MenuExit = 1;
    } // End key read
    if (changeDisp) {
      KeyTone();
      snprintf(LCDbuff1, 17, "time adjust: %3d", TimeAdj);
      LCDbprint(2);
      changeDisp = false;
      TimeStart=millis();
    }
    if((millis()-TimeStart) > 15000) {
      TimeAdj = EEPROM.read(MEMO_TimeAdj);
      MenuExit = 1;
    }
  }    
}

void MenuBackLightSet() {
  uint8_t LastItem = 0;
  SubMenuDisp(LastItem, 3);
  lcd.noBlink();
  BackLightDisp(LastItem);
  //myDelay(500);
  boolean changeDisp = false;
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1 ; ) {
    //Read_Key();
    if (myButt0.isClick()) { // Right
      if (BLset[LastItem] <= 931) {
        BLset[LastItem] += 92;
        changeDisp = true;
      } else {
        BLset[LastItem] = 1023;
        ErrorBeep();
      }
    } 
    if (myButt1.isClick()) { //Up
      LastItem = 0;
      changeDisp = true;
    } 
    if (myButt2.isClick()) { // Down
      LastItem = 1;
      changeDisp = true;
    } 
    if (myButt3.isClick()) { // Left
      if (BLset[LastItem] >= 103) {
        BLset[LastItem] -= 92;
        changeDisp = true;
      } else {
        BLset[LastItem] = 0;
        ErrorBeep();
      }
    } 
    if (myButt4.isClick()) { // Menu
      KeyTone();
      EEPROM.write(MEMO_BLsetDay, BLset[0]/4);
      EEPROM.write(MEMO_BLsetNight, BLset[1]/4);
      Timer1.setPwmDuty(BackLightPin, 1023);
      MenuExit = 1;
    } // End key read
    if (changeDisp) {
      KeyTone();
      BackLightDisp(LastItem);
      changeDisp = false;
      TimeStart=millis();
    }
    if((millis()-TimeStart) > 15000) {
      if (EEPROM.read(MEMO_BLsetDay) > 0) {
        BLset[0] = 4*EEPROM.read(MEMO_BLsetDay);
        BLset[1] = 4*EEPROM.read(MEMO_BLsetNight);
      } else {
        BLset[0] = 1023;
        BLset[1] = 220;
      }
      Timer1.setPwmDuty(BackLightPin, 1023);
      MenuExit = 1;
    }
  }    
  ButtonReset();
}

void SysMenuSelect() { // System menu
  noInterrupts();
  uint8_t LastItem = 0;
  uint8_t LastDisp = 0;
  uint8_t MaxItem = 8; // 9 items max
  lcd.noCursor();
  lcd.noBlink();
  Timer1.setPwmDuty(BackLightPin, 1023);
  SysMenuDisp(LastItem, LastDisp);
  myDelay(500);
  boolean changeDisp = false;
  unsigned long TimeStart=millis();
  for(uint8_t MenuExit = 0; MenuExit < 1; ) { // Read key
    if (myButt0.isClick()) { // Right
      changeDisp = true;
      KeyTone();
      switch( LastItem ) { 
        case 0: // * >Hour beep on  *
          SubChangeMenu(1); 
          break;
        case 1: // *  Key tone on   *
          SubChangeMenu(2); 
          break;
        case 2: // *  Set relay type *
          SubChangeMenu(0);
          break;
        case 3: // *  Set num channel*
          MenuChannelsNum();
          break;
        case 4: // *  Set time adjust value*
          MenuSetTimeAdjust();
          break;
        case 5: // *  Set backlight value*
          MenuBackLightSet();
          break;
        case 6: // *  Analog key test*
#ifndef KeySetup
          AnalogKeysTest();
#else
          boolean SetKey = AnalogKeySet();
#endif
          break;
        case 7: // *  Default value *
          EEPROM.write(0,1);
          ReadWriteEEPROM();
          MenuExit = 1;
          break;
        case 8: // *  Exit menu     *
          MenuExit = 1;
          break;
      }
    } 
    if (myButt1.isClick() || myButt1.isStep()) { //Up
      if (LastItem > 0) {
        if (LastItem <= LastDisp) LastDisp--;
        LastItem--;
        changeDisp = true;
      } else ErrorBeep();
    } 
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      if (LastItem < MaxItem) {
        if (LastItem > LastDisp) LastDisp++;
        LastItem++;
        changeDisp = true;
      } else ErrorBeep();
    } 
    if (myButt3.isClick()) { // Left
      changeDisp = true;
      LastItem = MaxItem;
      LastDisp = MaxItem - 1;
    }
    if (myButt4.isClick()) { // Menu
      KeyTone();
      MenuExit = 1;
    } // End key read
    if (changeDisp) {
      KeyTone();
      SysMenuDisp(LastItem, LastDisp);
      changeDisp = false;
      TimeStart=millis();
    }
    if((millis()-TimeStart) > 15000) MenuExit = 1;
  } 
  interrupts();
  ButtonReset();
}

void MenuSelect() { // main menu function
  noInterrupts(); 
  unsigned long TimeStart=millis();
  uint8_t LastItem = 0;
  uint8_t LastDisp = 0;
  uint8_t MaxItem = 4; // 5 items max
  Timer1.setPwmDuty(BackLightPin, 1023);
  lcd.noCursor();
  lcd.noBlink();
  boolean changeDisp = 0;
  MenuDisp(LastItem, LastDisp);
  myDelay(500);
  interrupts(); 
  for (uint8_t MenuExit = 0; MenuExit < 1;) {
    //Read_Key();
    if (myButt0.isClick()) { // Right
      switch( LastItem ) {
        case 0: // * >Set Date&Time *
          KeyTone();
          TimeSetup(); 
          MenuExit = 1;
          break;
        case 1: // *  Set Timers    *
          KeyTone();
          SetTimers();
          changeDisp = true;
          break;
        case 2: // *  Set eat time  *
          KeyTone();
          FeedMenu();
          changeDisp = true;
          break;
        case 3: // *  System menu   *
          KeyTone();
          SysMenuSelect();
          changeDisp = true;
          break;
        case 4: // *  Exit menu     *
          KeyTone();
          MenuExit = 1;
          break;
      }
      TimeStart=millis()-15000;
    }
    if (myButt1.isClick() || myButt1.isStep()) { // Up
      if (LastItem > 0) {
        if (LastItem <= LastDisp) LastDisp--;
        LastItem--;
        changeDisp = true;
      } else ErrorBeep();
    }
    if (myButt2.isClick() || myButt2.isStep()) { // Down
      if (LastItem < MaxItem) {
        if (LastItem > LastDisp) LastDisp++;
        LastItem++;
        changeDisp = true;
      } else ErrorBeep();
    }
    if (myButt3.isClick()) { // Left
      LastItem = MaxItem;
      LastDisp = MaxItem - 1;
      changeDisp = true;
    }
    if (myButt4.isClick()) { // Menu
      KeyTone();
      MenuExit = 1;
    }
    if (changeDisp) {
      KeyTone();
      TimeStart=millis();
      MenuDisp(LastItem, LastDisp);
      changeDisp = false;
    }
    if((millis()-TimeStart) > 45000) MenuExit = 1;
  }
}

