String lid1Zero(uint8_t val) {
    if (val<10) return "0" + String(val);
    else return String(val);
}

void HourBeep(uint16_t CurTime) { // makes a beep each hour
    if (BeepOnOff) {
        if (CurTime % 60 == 0) {
            if (!BeepNow) { // if not peak yet
                //int ToneBoy[] = {NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6};
                //int ToneBoy[] = {NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5, NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6};
                uint16_t ToneBoy[] = {NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6, NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7};
                CurTime /= 60;
                CurTime %= 12;
                tone(TonePin, ToneBoy[CurTime], 31);
                BeepNow = true;
                myDelay(35);
                digitalWrite(TonePin, LOW);
            }
        } else BeepNow = false;
    }
}

uint8_t StateChannels(uint16_t CurTime, uint8_t MyDayOfWeek) {
  uint8_t ChState = 0;
  uint8_t bitDay = 1;
  if (MyDayOfWeek == 0) MyDayOfWeek = 7;
  if (MyDayOfWeek > 1) bitDay <<= (MyDayOfWeek-1);
  for (uint8_t i=0 ; i<TimersNum ; i++) {
  	EEreadTimer(i);
  	if (ChOnOff[0] & bitDay) {
  	  uint16_t TimeOn = ChOnOff[2]*60+ChOnOff[3];
      uint16_t TimeOff = ChOnOff[4]*60+ChOnOff[5];
      if (TimeOff > TimeOn) {
      	if((CurTime >= TimeOn) && (CurTime < TimeOff)) ChState |= (1 << ChOnOff[1]);
      } else { 
    	if((CurTime >= TimeOn) || (CurTime < TimeOff)) ChState |= (1 << ChOnOff[1]);
      }
    }
  } 
  return ChState;
}

boolean NotFeeding(uint16_t CurrTime) { // Verification of the need for feeding
    uint8_t ttt = 1;
#ifdef TEST_MODE
    ttt = 5;
#endif
    if (((CurrTime >= FeedTime1) && (CurrTime < (FeedTime1+ttt))) || ((CurrTime >= FeedTime2) && (CurrTime < (FeedTime2+ttt)))) {
        if (FeedOK) return true;
        else {
            FeedOK = true;
            return false;
        }
    } else {
        FeedOK = false;
        return true;
    }
}

