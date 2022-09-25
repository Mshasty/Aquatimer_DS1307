boolean FeedOK = false; // Feeding held
uint8_t ChOnOff[6]; // Buffer for timers
uint8_t oldsec = 65; 	// Previous second value
int8_t TimeAdj = 0; 	// Time correction value

// variables for backlight
uint16_t BLset[2] = {BACKLIGHT_DAY, BACKLIGHT_NIGHT}; 	// 255 - day mode, 55 - night mode
// channel on/off trigger  0x80        0x40       0x20      0x10        0x08       0x04       0x02      0x01
byte NeedOnOff = 0; // Ch2OnOff, Ch2NeedOff, Ch2NeedOn, Ch1OnOff, Ch1NeedOff, Ch1NeedOn, BLNeedOff, BLNeedOn
  // 76543210 -- Back Light Need On
  // ||||||└---- Back Light Need Off
  // |||||└----- Channel 1 Need On
  // ||||└------ Channel 1 Need Off
  // |||└------- Channel 1 On/Off
  // ||└-------- Channel 2 Need On
  // |└--------- Channel 2 Need Off
  // └---------- Channel 2 On/Off
                 
uint8_t BLNightState = 0; 		// 0 - Set day mode, 1 - Set night mode

boolean BeepNow = false;

char LCDbuff0[17];
char LCDbuff1[17];

void EEwritedef();
void EEreadTimer(uint8_t);
void ReadWriteEEPROM();
void LCDbprint(byte StrQty);
void MenuSelect();
void SysMenuSelect();
void ShowChannels();
String lid1Zero(uint8_t val);
void HourBeep(uint16_t CurTime);
uint8_t StateChannels(uint16_t CurTime, uint8_t MyDayOfWeek);
void TimerDisp();
