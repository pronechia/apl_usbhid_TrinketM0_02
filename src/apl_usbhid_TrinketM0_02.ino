/*
 *公開第二回）リアルタイムモニター
 * 2020/03/25 Windows/MAC対応
 */
//どちらか選択せよ -- Windows or MAC -------------------------------------
//#define USBHOST_WINPC
#define USBHOST_MAC

#include "apl_usbhid_TrinketM0_02.h"

void setup(){  
  pinMode(LED_PIN,OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  Wire.begin(); 
  sub_kbd_begin();
  delay(100); //HIDデバイス、I2Cデバイスの初期化待ち
  sub_fw_Blink(LED_PIN, 3, 50); //動き始めたことを知らせる
  digitalWrite(LED_PIN, HIGH);  //明確に点灯
  mySensor.setI2CAddress(BME280DEVADDR);
  //BME280の初期化ができない場合、LED点滅して先に進まない
  if (mySensor.beginI2C() == false) {
    while (1) sub_fw_Blink(LED_PIN, 10, 500); //異常
  }
  g_pass = 1;
  while (sub_fw_SWcheck(SW_PIN) == 0); //SWが押されるまで待つ
  sub_fw_timerset();  //タイマー起動
}
//
//main loop
//
void loop(){

  // 40msec毎に処理を行う
  if (sub_fw_event(2)) sub_proc();
}
//
//USB/HID処理
//
void sub_proc() {
  static byte s_command = 0; //処理振り分け

  //待ち時間がゼロになるまで何もしない。
  if (g_pass-- >= 0 ) return;
  //処理振り分け
  if (s_command == 0) { 
    s_command = 1;
    sub_initurl(); //一回だけ実施
  } else if (s_command == 1) { 
    s_command = 2;
    sub_out_kbd(10);  //start
  } else if (s_command == 2) {
    sub_out_kbd(11); //センサー値UP
    s_command = 1; //繰り返し
  } else {
    ; //その他は何もしない。
  }
  //次への待ち時間設定
  if (s_command == 1) g_pass = 200; //40msec*200= 8sec
  else                g_pass = 50; //40msec*50= 2sec
}
