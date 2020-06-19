/*
 *公開第四回）radiko操作 2020/06/19
 * radiko OFF時にTabを閉じるが、Windows/Edge の場合は閉じることができない
 */
// WindowsPC用
#define USBHOST_WINPC

//環境に合わせて内容を確認または修正すること　ーーここから -----------------------------
//①LCD I2Cアドレスの設定
#define LCDDEVADDR 0x3e
//②ハード設定
#define LED_PIN 13
#define SW_PIN  1
#define SW2_PIN  3
//環境に合わせて内容を確認または修正すること　ーーここまで -----------------------------

#if defined USBHOST_WINPC
String g_url_string = "http'//radiko.jp/#!/live/";
#elif defined USBHOST_MAC
String g_url_string = "radiko.jp/#!/live/";
#else
/設定不備はコンパイルエラーにする
#endif
#define WCS_DELAY_T1 150 //T1 ブラウザー応答時間150
#define WCS_DELAY_GAMEN 3000  //URLたたいてから画面が表示されるまで待つ
#define WCS_SWWAIT_MAXCOUNT 30  //sw押下待ちのリトライ回数上限 30秒
#include <Wire.h>
#include <ZeroTimer.h>
#include "lib_keyboard.h"
#include "LCDManager.h"

int16_t g_pass;  //HID出力したら後の待ち時間を制御する
boolean g_I2CNormal;
boolean g_LED;
boolean g_radio_on;
volatile int g_i; //timerカウント
LCDManager myLCD;
//チャンネル定義は中部地方の例。必要にあわせて変更してください。
int g_channel; //最大3件
char g_annai[3][9] = {"c1:CBC  " , "c2:TOKAI" , "c3:NHK1 "};
char g_radiko_url[3][11] = {"CBC       " , "TOKAIRADIO" , "JOCK      "};
//
//
//
void setup(){
  char w_buf[9];
  g_pass = 1;
  g_i = 0;
  g_LED = false;
  g_radio_on = false;
  g_channel = 0;
  pinMode(LED_PIN,OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);

  Wire.begin();
  delay(100); //I2Cデバイスの初期化待ち
  //
  if (myLCD.begin(LCDDEVADDR) == false) g_I2CNormal = false;
  else g_I2CNormal = true;
  //
  strcpy(w_buf, " )pushSW");
#if defined USBHOST_WINPC
  sub_kbd_begin(1);  //Windows用に初期化
  w_buf[0] = 'W';
#elif defined USBHOST_MAC
  sub_kbd_begin(2);  //Mac用に初期化
  w_buf[0] = 'M';
#endif
  //
  sub_fw_Blink(LED_PIN, 3, 50); //動き始めたことを知らせる
  delay(100); //HIDデバイスの初期化待ち
  if (g_I2CNormal) myLCD.printStrC1(w_buf);
  if (g_I2CNormal) myLCD.printStrC0(g_annai[g_channel]);
  //
  sub_fw_timerset();  //タイマー起動
}
//
//main loop
//
void loop(){
  // 20x1=20msec毎に処理を行う
  if (sub_fw_event(1)) sub_proc();
}
//
//USB/HID処理
//
void sub_proc() {
  int j;

  //待ち時間がゼロになるまで何もしない。
  if (g_pass-- >= 0 ) return;

  if (sub_fw_SWcheck(0, SW_PIN) == 2) {
    if (g_radio_on) {
      //Enterを出して止める
      sub_apl_key2();
      if (g_I2CNormal) myLCD.printStrC1("Chan.OFF");
    } else {
      //URL+Tabx3を出す。画面が表示されるまで待つのもこの関数内
      if (g_I2CNormal) myLCD.printStrC1("URL out ");
      sub_initurl();
      sub_apl_key();
      if (g_I2CNormal) myLCD.printStrC1("Chan.ON ");
    }
    g_radio_on = !g_radio_on;
  } else if (sub_fw_SWcheck(1, SW2_PIN) == 2) {
    //channel 切替
    if (g_channel>1) g_channel = 0;
    else g_channel++;
    
    if (g_I2CNormal) myLCD.printStrC0(g_annai[g_channel]);
  } else {
    //その他は何もしない
    ;
  }
  g_pass = 1; 
}
//
//アプリ画面を呼び出すURLを出力する
void sub_initurl() {
  String w_string;
  
  //URL入力前に入力エリアにフォーカスを当てる
  sub_kbd_preURL();
  delay(WCS_DELAY_T1);
  //URL文字出力
  sub_kbd_print((char *)g_url_string.c_str());
  //文字列をtrimしてから出力する
  w_string = g_radiko_url[g_channel];
  w_string.trim();
  sub_kbd_print((char *)w_string.c_str());
  delay(WCS_DELAY_T1);
  sub_kbd_strok(KEY_RETURN);
  //
  delay(WCS_DELAY_GAMEN); //画面が表示されるまで、十分の時間待つ
  delay(WCS_DELAY_GAMEN); //少し余分に待つ
}
//アプリ用の手順にしたがってキーコードを出力する
void sub_apl_key() {
  //radiko用　Tab+Tab+Tab+Enter
  for (int j=0; j<3; j++) {
    sub_kbd_strok(KEY_TAB);
    delay(WCS_DELAY_T1);
    delay(WCS_DELAY_T1); //少し余分に待つ
  }
  // 
  sub_kbd_strok(KEY_RETURN);
  delay(WCS_DELAY_T1);  
}
//アプリ用の手順2にしたがってキーコードを出力する
void sub_apl_key2() {

  //ボタンを押す
  sub_kbd_strok(KEY_RETURN);
  delay(WCS_DELAY_T1);  
  //タブを閉じる
  sub_kbd_tabClose();

}
//タイマー割込み関数
void sub_timer_callback() {
  // 指定した間隔(default 20msec)で割込みが入る
  g_i++;
}
//
void sub_fw_timerset() {
  //20msec毎
  TC.startTimer(20000, sub_timer_callback);
}
//2カウント=約40msecでイベントを上げる
// rc true =event on
boolean sub_fw_event(uint8_t p_count) {
  boolean w_trriger;

  if (p_count > 25) return 0; //ERROR check  
  w_trriger = false;
  // g_i>50で約一秒。g_i>0で約20msec
  if (g_i >= p_count) {
    g_i = 0;
    w_trriger = true;
  }
  return w_trriger;
}
//
//SWが押されたかのチェック。二度呼ばれて、連続して押されていたらONを返す。
// rc 0:OFF 1:立ち上がり 2:立下り
uint8_t sub_fw_SWcheck(uint8_t p_number, uint8_t p_swpin) {
  static boolean s_pre_sw[2] = {1,1}; // =1はOFFを意味する
  static uint8_t s_first_sw[2] = {0,0};
  boolean w_sw;

  if (p_number > 1) return 0; //error
  
  w_sw = digitalRead(p_swpin); // =0がSWを押したとき
  //前回がOFF->ONの時、連続か？
  if (s_first_sw[p_number] == 1) {
    s_first_sw[p_number] = 0;
    s_pre_sw[p_number] = w_sw;  //次回のために
    if (w_sw == 0) return 1; //ON->ON つまり連続して押された
    else return 0;           // ON->OFFだった。
  //前回がON->OFFの時、連続か？
  } else if (s_first_sw[p_number] == 2) {
    s_first_sw[p_number] = 0;
    s_pre_sw[p_number] = w_sw;  //次回のために
    if (w_sw == 1) return 2; //OFF->OFF つまり連続して離れた
    else return 0;           // OFF->ONだった。
  }
  //OFFからONを検知する。
  if ((s_pre_sw[p_number] == 1) && (w_sw == 0)) {
    s_first_sw[p_number] = 1;
  }
  //ONからOFFを検知する。
  if ((s_pre_sw[p_number] == 0) && (w_sw == 1)) {
    s_first_sw[p_number] = 2;
  }
  s_pre_sw[p_number] = w_sw;  //次回のために
  return 0;
}
//p_timesは最大255とする
void sub_fw_Blink(uint8_t p_ledpin, byte p_times , int p_wait){
  for (byte i=0; i< p_times; i++){
    digitalWrite(p_ledpin, LOW);
    delay (p_wait);
    digitalWrite(p_ledpin, HIGH); //点灯で終わる
    delay (p_wait);
  }
}
