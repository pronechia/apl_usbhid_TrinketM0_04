/*
 * LCDmanager.cpp v1.0
 */
#include <Wire.h>
#include "LCDManager.h"
//
//public methods ------------------------------
//
boolean LCDManager::begin(uint8_t LCDAddr) {
  LCDDeviceAddr = LCDAddr;
	
	cmd(0b00111000); // function set
  cmd(0b00111001); // function set
  cmd(0b00000100); // EntryModeSet
  cmd(0b00010100); // interval osc
  cmd(0b01110000 | (contrast & 0xF)); // contrast Low
  cmd(0b01011100 | ((contrast >> 4) & 0x3)); // contast High/icon/power
  cmd(0b01101100); // follower control
  delay(200);
  cmd(0b00111000); // function set
  //test => 変化なし lcd_cmd(0b00110000); // function set
  cmd(0b00001100); // Display On
  cmd(0b00000001); // Clear Display

	clear();
	return true;
}
//
void LCDManager::clear() {
	setCursor(0, 0);
	printStr("  ");  //全バイト？
	setCursor(0, 1);
	printStr("  "); //全バイト？
}


// 表示位置の指定
void LCDManager::setCursor(byte x, byte y) {
  cmd(0x80 | (y * 0x40 + x));
}

// 文字の表示
void LCDManager::printStrC0(const char *s) {
  setCursor(0, 0);
  printStr(s);
}
void LCDManager::printStrC1(const char *s) {
  setCursor(0, 1);
  printStr(s);
}
void LCDManager::printStr(const char *s) {
  Wire.beginTransmission(LCDDeviceAddr);
  while (*s) {
    if (*(s + 1)) {
      contdata(*s);
    } else {
      lastdata(*s);
    }
    s++;
  }
  Wire.endTransmission();
}
// 数値の表示
void LCDManager::printInt(int num) {
  char int2str[10];
  sprintf(int2str, "%d", num);
  printStr(int2str);
}
//
// private methods ------------------------------
//
void LCDManager::cmd(byte x) {
  Wire.beginTransmission(LCDDeviceAddr);
  Wire.write(0b00000000); // CO = 0,RS = 0
  Wire.write(x);
  Wire.endTransmission();
}

void LCDManager::contdata(byte x) {
  Wire.write(0b11000000); // CO = 1, RS = 1
  Wire.write(x);
}

void LCDManager::lastdata(byte x) {
  Wire.write(0b01000000); // CO = 0, RS = 1
  Wire.write(x);
}
