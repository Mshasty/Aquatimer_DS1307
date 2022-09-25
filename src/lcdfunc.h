// State for LCD i2c mode
//#define LCD_I2C_MODE 

#ifndef LCD_I2C_MODE
#include <LiquidCrystal.h>
#else
#include <LiquidCrystal_I2C.h>
#define DS1307_ADDRESS 0x68
#endif

#ifndef LCD_I2C_MODE
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // for LCD 16*2 with 6*key
#else
LiquidCrystal_I2C lcd(0x27);
//LiquidCrystal_I2C lcd(0x27, 16, 2);
// SDA pin   -> Arduino Analog 4 (A4)
// SCL pin   -> Arduino Analog 5 (A5)
#endif

const uint8_t RotaryChar[4] = {0x2D, 3, 0x7C, 0x2F}; // Rotary symbol codes
const char dn[8][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char mn[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dec"};

uint8_t RotaryNum = 0; 
#define RotaryMaxNum 3

#define SETPIC 2 	// Set-symbol number on table

void LCDsetup() {
  // My symbols
  uint8_t triang[8]  = {0x0,0x8,0xc,0xe,0xc,0x8,0x0};   // >
  uint8_t cosaya[8] = {0x0,0x10,0x8,0x4,0x2,0x1,0x0};   // reverse slash
  uint8_t mybell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};  // bell
  uint8_t nobell[8]  = {0x5,0xf,0xe,0xe,0x1f,0x8,0x10}; // no bell
  uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};   // V
  uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};   // x
  uint8_t smboff[8] = {0x0,0x0,0xe,0xa,0xe,0x0,0x0};    // o

  lcd.createChar(1, nobell);
  lcd.createChar(2, triang);
  lcd.createChar(3, cosaya);
  lcd.createChar(4, mybell);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);
  lcd.createChar(7, smboff);
  lcd.home();
  lcd.noCursor();
}

void LCDbprint(byte StrQty) { // print buffer to LCD
  noInterrupts(); 
  if ((StrQty & 1)) { // print upper string
    lcd.setCursor(0, 0); 
    lcd.print(LCDbuff0); 
  }
  if (StrQty & 2) { // print low string
    lcd.setCursor(0, 1); 
    lcd.print(LCDbuff1); 
  }
  interrupts(); 
}

void ShowChannels() {
  snprintf(LCDbuff0, 17, "Out-channels hex"); 
  snprintf(LCDbuff1, 17, "                ");
  LCDbprint(3);
  lcd.setCursor(0, 1);
  for (uint8_t ij = 0 ; ij < MaxNumChannels ; ij++) {
    lcd.print(ChPin[ij], HEX);
    if ((ij+1) < MaxNumChannels) lcd.write(32);
    else if (((ij+1) == MaxNumChannels) && (ChannelsNum < MaxNumChannels)) lcd.write(41);
    if (((ij+1) == ChannelsNum) && (ChannelsNum < MaxNumChannels)) lcd.write(40);
  }
}

void ShowFeedingTime() {
  snprintf(LCDbuff0, 17, "Feed %02d:%02d %02d:%02d", EEPROM.read(MEMO_FeedHour1), EEPROM.read(MEMO_FeedMin1), EEPROM.read(MEMO_FeedHour2), EEPROM.read(MEMO_FeedMin2));
  snprintf(LCDbuff1, 17, "\4 duration%2d.%d s", FeedDelay/10, FeedDelay%10);
  LCDbprint(3);
}

void ShowTimer(uint8_t i) {
  lcd.setCursor(0, 0);
  lcd.print(lid1Zero(i+1));
  EEreadTimer(i);
  TimerDisp();
  lcd.setCursor(1, 0);
  lcd.blink();
}

String TimerOnOffDisp(uint8_t ChState) {
  uint8_t kk = 1;
  String MyOnOffDisp = "       ";
  if (NeedOnOff & 0b0100) { // Ch1NeedOn
    if (ChState & 1) NeedOnOff &= 0b11111011; // bit3 = 0
    else ChState |= 1;
  }
  if (NeedOnOff & 0b01000) { // Ch1NeedOff
    if (ChState & 1) ChState &= 0b1110; // Ch1 off
    else NeedOnOff &= 0b11110111; // bit4 = 0
  }
  BLNightState = !(ChState & 1);
  if (NeedOnOff & 0x01) {
    if (ChState & 1) NeedOnOff &= 0xfe;
    else BLNightState = 0;
  }
  if (NeedOnOff & 0x02) {
    if (ChState & 1) BLNightState = 1;
    else NeedOnOff &= 0b11111101;;
  }
  if (NeedOnOff & 0x20) {
    if (ChState & 2) NeedOnOff &= 0b11011111; // bit6 = 0
    else ChState |= 2;
  }
  if (NeedOnOff & 0x40) {
    if (ChState & 2) ChState &= 0b1101;
    else NeedOnOff &= 0xbf;
  }
  for(uint8_t i=0;i<MaxNumChannels;i++) {
    if (i < ChannelsNum) { // Only working channels
      if (kk & ChState) {
        if (i==1) { // Pump channel
          //lcd.write(RotaryChar[RotaryNum]);
          MyOnOffDisp[i] = RotaryChar[RotaryNum];
          RotaryNum++;
          if (RotaryNum > RotaryMaxNum) RotaryNum = 0;
        } else {
          //lcd.write(223); // Zaboy
          MyOnOffDisp[i] = 223;
        }
        digitalWrite(ChPin[i], RelayUp);
        if (i==0) {
          //Ch1OnOff = true;
          NeedOnOff |= 0x10;
        }
        if (i==1) {
          //Ch2OnOff = true;
          NeedOnOff |= 0x80;
        }
      } else {
        //lcd.write(161);
        MyOnOffDisp[i] = 161;
        digitalWrite(ChPin[i], HIGH-RelayUp);
        if (i==0) {
          //Ch1OnOff = false;
          NeedOnOff &= 0xEF;
        }
        if (i==1) {
          //Ch2OnOff = false;
          NeedOnOff &= 0x7F;
        }
      }
    } else { // not working channels
      //lcd.write(6); // Cross
      MyOnOffDisp[i] = 6;
    }
    kk <<= 1;
  }
  //for(uint8_t i=MaxNumChannels;i<6;i++) MyOnOffDisp += " "; // for five and less channel configuration
  if (BeepOnOff) MyOnOffDisp[6] = 4; // Bell
  else MyOnOffDisp[6] = 1; // noBell
  //analogWrite(BackLightPin, BLset[BLNightState]);
	Timer1.setPwmDuty(BackLightPin, BLset[BLNightState]);
  return MyOnOffDisp;
}

#ifndef TEST_MODE
void TimeToLCD(uint8_t *DoW, uint8_t *myDay, uint8_t *myMonth, uint16_t *myYear, uint8_t *myHour, uint8_t *myMin, uint8_t PosCur) {
  noInterrupts(); 
#ifndef isDS1302 // for DS1307
  snprintf(LCDbuff1, 17, " %02d.%02d.%02d %02d:%02d ", *myDay, *myMonth, *myYear, *myHour, *myMin); // [ 16.09.17 16:02 ]
#else // for DS1302
  snprintf(LCDbuff1, 17, "%d %02d.%02d.%02d %02d:%02d", *DoW, *myDay, *myMonth, *myYear, *myHour, *myMin); // [2 16.09.17 16:02]
#endif
  LCDbprint(2);
  lcd.setCursor(PosCur, 1);
  interrupts(); 
}
#endif

void TimerDisp() {
  lcd.setCursor(2, 0);
  uint8_t kk = 1;
  for(uint8_t j=0;j<7;j++) {
    lcd.write(32);
    if (kk & ChOnOff[0]) lcd.print(j+1);
    else lcd.write(7); // smboff
    kk <<= 1;
  }
  snprintf(LCDbuff1, 17, "Ch%d: %02d:%02d\375%02d:%02d", ChOnOff[1]+1, ChOnOff[2], ChOnOff[3], ChOnOff[4], ChOnOff[5]);
  LCDbprint(2);
}

void FeedDisp(uint8_t Pos) { // Displaying time and duration of feeding
  uint8_t CurTab[5] = {1, 4, 7, 10, 15};
  snprintf(LCDbuff0, 17, "Feed1 Feed2 \256Dur");
  snprintf(LCDbuff1, 17, "%02d:%02d %02d:%02d %2d.%d", EEPROM.read(MEMO_FeedHour1), EEPROM.read(MEMO_FeedMin1), EEPROM.read(MEMO_FeedHour2), EEPROM.read(MEMO_FeedMin2), FeedDelay/10, FeedDelay%10);
  LCDbprint(3);
  lcd.setCursor(CurTab[Pos], 1);
}

void SubMenuDisp(uint8_t CursPos, uint8_t MenuPos) { // Display system menu items
  noInterrupts(); 
  char MenuItem[2][4][17] = {"Positive relay  ", "Hour Beep Off   ", "Keys Tone Off   ", " Day            ", "Inverted relay  ", "Hour Beep On    ", "Keys Tone On    ", " Ngt            "};
  snprintf(LCDbuff0, 17, MenuItem[0][MenuPos]);
  snprintf(LCDbuff1, 17, MenuItem[1][MenuPos]);
  LCDbprint(3);
  switch( MenuPos ) {
    case 0: // Relay type
      lcd.setCursor(15, HIGH-RelayUp);
      break;
    case 1: // Hour beep
      lcd.setCursor(15, BeepOnOff);
      break;
    case 2: // Keys tone
      lcd.setCursor(15, KeyToneOn);
      break;
  }
  lcd.write(5);
  lcd.setCursor(15, CursPos);
  lcd.blink();
  interrupts(); 
}

void DispNumOfChannels() {
  lcd.setCursor(14, 1);
  lcd.print(ChannelsNum);
}

void BackLightDisp(uint8_t CursPos) {
  Timer1.setPwmDuty(BackLightPin, BLset[CursPos]);
  lcd.setCursor(0, CursPos);
  lcd.write(SETPIC);
  lcd.setCursor(0, 1 - CursPos);
  lcd.write(32);
  for (uint8_t i=0;i<2;i++) {
    lcd.setCursor(5, i);
    for (uint16_t j=0; j<12; j++) {
      if (92*j+11<=BLset[i]) lcd.write(255);
      else lcd.write(219); 
    }
  }    
}

void MenuDisp(uint8_t CursPos, uint8_t DispPos) { // Display main menu
  char MenuItem[5][17] = {" Set date & time", " Setting timers ", " Feeding task   ", " System menu    ", " Menu exit      "};
  snprintf(LCDbuff0, 17, MenuItem[DispPos]);
  snprintf(LCDbuff1, 17, MenuItem[DispPos+1]);
  LCDbprint(3);
  lcd.setCursor(0, (CursPos-DispPos));
  lcd.write(SETPIC);
}

void SysMenuDisp(uint8_t CursPos, uint8_t DispPos) { // Demonstration of the additional menu
  char MenuItem[9][17] = {" Set hour beep  ", " Set key tone   ", " Set relay type ", " Set num channel", " Set time adjust", " Set backlight  ", " Analog key set ", " Default value  ", " Main menu      "};
  snprintf(LCDbuff0, 17, MenuItem[DispPos]);
  snprintf(LCDbuff1, 17, MenuItem[DispPos+1]);
  LCDbprint(3);
  lcd.setCursor(0, (CursPos-DispPos));
  lcd.write(SETPIC);
}

void FeedStart() {
  interrupts(); 
  lcd.print(F("Feeding"));
  if (BeepOnOff) {
    tone(TonePin, NOTE_A5, 100);
    myDelay(120);
    tone(TonePin, NOTE_A5, 500);
  }
  digitalWrite(VibroPin, HIGH);
  myDelay(FeedDelay*100);
  digitalWrite(VibroPin, LOW);
  myDelay(500);
}

void TimeAdjusting() { // Time adjusting
#ifdef isDS1302
  if (TimeAdj < 0) {
    lcd.print(F("Adjusting"));
    lcd.setCursor(0, 1);
    rtc.halt(true);
    myDelay(1000*abs(TimeAdj));
    rtc.halt(false);
    myDelay(1000);
  } else rtc.setTime(3, 30, TimeAdj);
#else
  DateTime Now = rtc.now();
  //DateTime future DateTime(Now.year(), Now.month(), Now.day(), 3, 30, 30 + TimeAdj);
  rtc.adjust(DateTime(Now.year(), Now.month(), Now.day(), 3, 30, 30 + TimeAdj));
  if (TimeAdj < 0) {
    lcd.print(F("Adjusting"));
    lcd.setCursor(0, 1);
    myDelay((1+abs(TimeAdj))*1000);
  }
#endif
}
