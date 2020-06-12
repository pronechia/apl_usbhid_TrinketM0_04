/*
 * LCDmanager.h  v1.0
 */
#ifndef _LCDMANAGER_H_
#define _LCDMANAGER_H_
#include <Arduino.h>

class LCDManager {
#define LCD_DEFAULT_LEN 8
#define LCD_DEFAULT_THRESH 50
#define CDL_VALCOUNT 8
public:
  uint8_t LCDDeviceAddr;        //0x3e  //LCDデバイス
  byte contrast = 35;  // コントラスト(0～63)

  boolean begin(uint8_t LCDAddr);
  void clear();
	void setCursor(byte x, byte y);
  void printStr(const char *s);
  void printStrC0(const char *s); //上段
  void printStrC1(const char *s); //下段
  void printInt(int num);
   
private:
  void cmd(byte x);
  void contdata(byte x);
  void lastdata(byte x);
};
#endif
// END OF FILE
