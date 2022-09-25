#include <EEPROM.h> 

#define PASS_CODE 0xA5  // проверка содержимого памяти
#define MEMO_ChannelsNum 1
#define MEMO_FeedHour1 2
#define MEMO_FeedMin1 3
#define MEMO_FeedHour2 4
#define MEMO_FeedMin2 5
#define MEMO_FeedDelay 6
#define MEMO_RelayUp 7
#define MEMO_BeepOnOff 8
#define MEMO_KeyToneOn 9
#define MEMO_TimeAdj 10
#define MEMO_BLsetDay 11
#define MEMO_BLsetNight 12
#define MEMO_KeysConfig 13
#define TmrAddr 26    // Timers start address in EEPROM

// Written massive to EEPROM from adderess <addr> and size of <dtlng>
void eeWrite(uint8_t val[], uint16_t addr, uint16_t dtlng) {
  for(uint8_t i=0; i<dtlng; i++) {
    EEPROM.write(i+addr, val[i]);
  }
}

void EEwritedef() { // Written default value to EEPROM for all timers
  // for ver.3.04 & above        0     1            2             3            4             5            6         7        8         9        10       11            12              13    14    15    16    17    18 19 20 21
  // default values for EEPROM: (Pass, ChannelsNum, TimeEatHour1, TimeEatMin1, TimeEatHour2, TimeEatMin2, EatDelay, RelayUp, HourBeep, KeyBeep, TimeAdj, BackLightDay, BackLightNight, Key0, Key1, Key2, Key3, Key4, 0, 0, 0, Timers defult setting)
  //                      0          1  2  3   4   5   6  7  8  9  10 11   12  |<--      Keyboard setting      -->| 23 24 25 |<--     Timer1     -->|<--      Timer2      -->|<--     Timer3     -->|<--    Timer4    -->|
  uint8_t ChOnOffDef[] = {PASS_CODE, 4, 9, 25, 19, 45, 3, 0, 1, 1, 0, 255, 55, 0, 100, 1, 24, 1, 144, 2, 88, 3, 82, 0, 0, 0, 0x1F, 0, 7, 20, 21, 0, 0x7F, 1, 16, 40, 19, 20, 0x60, 0, 9, 30, 21, 0, 0x2A, 2, 7, 55, 21, 55};
  ChOnOffDef[MEMO_ChannelsNum] = ChannelsNum;
  ChOnOffDef[MEMO_BeepOnOff] = BeepOnOff;
  ChOnOffDef[MEMO_KeyToneOn] = KeyToneOn;
  ChOnOffDef[MEMO_TimeAdj] = TimeAdj;
  for (uint8_t j = 0; j < 5; j++) {
    ChOnOffDef[MEMO_KeysConfig + 2*j] = adc_key_val[j] / 0x100;
    ChOnOffDef[MEMO_KeysConfig + 2*j + 1] = adc_key_val[j] % 0x100;
  }
  eeWrite(ChOnOffDef, 0, sizeof(ChOnOffDef));     // Write default array
  uint8_t ChOnOffNull[6] = {85, 4, 22, 0, 23, 0}; // Default timer setting
  // write other timers to EEPROM
  for (uint8_t i = 4; i < TimersNum; i++) eeWrite(ChOnOffNull, TmrAddr + i*6, sizeof(ChOnOffNull));
}

void EEreadTimer(uint8_t NumTmr) { // read desired timer
  for (uint8_t i=0; i<6 ; i++) {
    ChOnOff[i] = EEPROM.read(TmrAddr + 6*NumTmr + i);
  }
}

void ReadWriteEEPROM() { // check the contents EEPROM
  if (EEPROM.read(0) == PASS_CODE) {
    ChannelsNum = EEPROM.read(MEMO_ChannelsNum);
    FeedTime1 = EEPROM.read(MEMO_FeedHour1)*60 + EEPROM.read(MEMO_FeedMin1);
    FeedTime2 = EEPROM.read(MEMO_FeedHour2)*60 + EEPROM.read(MEMO_FeedMin2);
    FeedDelay = EEPROM.read(MEMO_FeedDelay);
    RelayUp = HIGH - EEPROM.read(MEMO_RelayUp); //Inverted relay
    BeepOnOff = EEPROM.read(MEMO_BeepOnOff);
    KeyToneOn = EEPROM.read(MEMO_KeyToneOn);
    TimeAdj = EEPROM.read(MEMO_TimeAdj);
    if (EEPROM.read(MEMO_BLsetDay) > 0) {
      BLset[0] = 4*EEPROM.read(MEMO_BLsetDay);
      BLset[1] = 4*EEPROM.read(MEMO_BLsetNight);
    }
    for (uint8_t j = 0; j < 5; j++) 
      adc_key_val[j] = 256*EEPROM.read(MEMO_KeysConfig + 2*j) + EEPROM.read(MEMO_KeysConfig + 2*j + 1);
    snprintf(LCDbuff0, 17, "%02d on/off timers", TimersNum);
    snprintf(LCDbuff1, 17, "for %d/%d channels", ChannelsNum, MaxNumChannels);
    LCDbprint(3);
    if (BeepOnOff) {
      tone(TonePin, NOTE_A6, 200);
      myDelay(220);
      tone(TonePin, NOTE_E7, 100);
    }
    myDelay(1780);
    snprintf(LCDbuff0, 17, " Version %2d.%02db ", TVer/16, TVer%16);
    snprintf(LCDbuff1, 17, "url vk.cc/6Z2GQ7"); // link to https://vk.cc/6Z2GQ7
    LCDbprint(3);
    myDelay(1500);
  } else {
    EEwritedef();
    snprintf(LCDbuff0, 17, "Written time for");
    snprintf(LCDbuff1, 17, "%d channel EEPROM", ChannelsNum);
    LCDbprint(3);
    myDelay(2500);
    ShowChannels();
    tone(TonePin, NOTE_E7, 200);
    myDelay(220);
    tone(TonePin, NOTE_A6, 100);
    myDelay(2000);
  }
}
