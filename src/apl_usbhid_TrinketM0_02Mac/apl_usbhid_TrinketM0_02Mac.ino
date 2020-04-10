/*
 *公開第二回）リアルタイムモニター
 * 2020/04/01 Windows/MAC
 * 2020/04/03 ライブラリを分離。設定説明追記
 * 2020/04/10 I2Cエラー時も動かすが値ゼロ。URL入力後、TABを出す、Chrome動作確認
 */
// Mac用

//環境に合わせて内容を確認または修正すること　ーーここから -----------------------------
//①BME280 I2Cアドレスの設定　（0x76 or 0x77 のどちらか）
#define BME280DEVADDR 0x76
//#define BME280DEVADDR 0x77
//②Webserverに合わせて、ＵＲＬを記述すること
//String g_url_string = "http'//192.168.xxx.xxx/xxxxxxxx.htm"; //内部WebServer用
String g_url_string = "http'//localhost/example0201.htm"; //localhost用
//③ハード設定
#define LED_PIN 13
#define SW_PIN  1
//環境に合わせて内容を確認または修正すること　ーーここまで -----------------------------

#define WCS_DELAY_T1 150 //T1 ブラウザー応答時間150
#define WCS_DELAY_GAMEN 3000  //URLたたいてから画面が表示されるまで待つ
#include <Wire.h>
#include <ZeroTimer.h>
#include <SparkFunBME280.h>
#include "lib_keyboard.h"

BME280 mySensor;
int16_t g_pass;  //HID出力したら後の待ち時間を制御する。 pass1=20msec pass2=40msec ...
boolean g_I2CNormal;
volatile int g_i; //timerカウント

//
//
//
void setup(){  
  pinMode(LED_PIN,OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  Wire.begin();
  //sub_kbd_begin(1);  //Windows用に初期化
  sub_kbd_begin(2);  //Mac用に初期化
  delay(100); //HIDデバイス、I2Cデバイスの初期化待ち
  sub_fw_Blink(LED_PIN, 3, 50); //動き始めたことを知らせる
  digitalWrite(LED_PIN, HIGH);  //明確に点灯
  mySensor.setI2CAddress(BME280DEVADDR);
  //BME280の初期化ができない場合、値ゼロで動かす
  if (mySensor.beginI2C() == false) g_I2CNormal = false;
  else                              g_I2CNormal = true;
  g_pass = 1;
  g_i = 0;
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
  static byte s_first = 0;
  
  //待ち時間がゼロになるまで何もしない。
  if (g_pass-- >= 0 ) return;
  //処理振り分け
  if (s_command == 0) { 
    s_command = 1;
    s_first = 1;
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
  if (s_command == 1) {
    if (s_first == 1) {
      g_pass = 50; //40msec*50= 2sec
      s_first = 0;
    } else {
      g_pass = 200; //40msec*200= 8sec    
    }
  } else {
    g_pass = 25; //40msec*25= 1sec
  }
}
//
//周期によって、決まった文字列処理（主にHTML画面への文字列入力）をおこなう
//
void sub_out_kbd(int8_t p_ctl) {
  int32_t w_temp;
  int32_t w_temp2;
  uint32_t w_pres;
  uint32_t w_hum;
  char w_buf[16];

  if (p_ctl == 0) {
    delay(WCS_DELAY_T1 );
  } else if (p_ctl == 10) {
    sub_moji_tab("Start");
    sub_kbd_strok(KEY_RETURN);
  } else if (p_ctl == 11) {
    //ラベルと値を出力する
    sub_moji_tab("Temprature");
    //センサー値を得る
    if (g_I2CNormal == true) {
      w_temp = (int32_t)(mySensor.readTempC() * 100); //xx.xx度をxxxxに変換
      if (w_temp > 9999) sprintf(w_buf, "100.00");
      else {
        w_temp2 = w_temp - (w_temp / 100 * 100);
        sprintf(w_buf, "%d.%02d", w_temp / 100, w_temp2);
      }
    } else {
      sprintf(w_buf, "0.0");
    }
    sub_moji_tab(w_buf);
    sub_moji_tab("C");
    sub_kbd_strok(KEY_RETURN);  //ボタンを押す
  }
}
//
//アプリ画面を呼び出すURLを出力する
void sub_initurl() {

  //URL入力前に入力エリアにフォーカスを当てる
  sub_kbd_preURL();
  delay(WCS_DELAY_T1);
  //URL文字出力
  sub_kbd_print((char *)g_url_string.c_str());
  sub_kbd_strok(KEY_RETURN);
  //
  delay(WCS_DELAY_GAMEN); //画面が表示されるまで、十分の時間待つ
  sub_kbd_strok(KEY_TAB);
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
uint8_t sub_fw_SWcheck(uint8_t p_swpin) {
  static boolean s_pre_sw = 1; // =1はOFFを意味する
  static uint8_t s_first_sw = 0;
  boolean w_sw;

  w_sw = digitalRead(p_swpin); // =0がSWを押したとき
  //前回がOFF->ONの時、連続か？
  if (s_first_sw == 1) {
    s_first_sw = 0;
    s_pre_sw = w_sw;  //次回のために
    if (w_sw == 0) return 1; //ON->ON つまり連続して押された
    else return 0;           // ON->OFFだった。
  //前回がON->OFFの時、連続か？
  } else if (s_first_sw == 2) {
    s_first_sw = 0;
    s_pre_sw = w_sw;  //次回のために
    if (w_sw == 1) return 2; //OFF->OFF つまり連続して離れた
    else return 0;           // OFF->ONだった。
  }
  //OFFからONを検知する。
  if ((s_pre_sw == 1) && (w_sw == 0)) {
    s_first_sw = 1;
  }
  //ONからOFFを検知する。
  if ((s_pre_sw == 0) && (w_sw == 1)) {
    //org s_first_sw = 2;
    s_first_sw = 3;
  }
  s_pre_sw = w_sw;  //次回のために
  return 0;
}
//p_timesは最大255とする
void sub_fw_Blink(uint8_t p_ledpin, byte p_times , int p_wait){
  for (byte i=0; i< p_times; i++){
    digitalWrite(p_ledpin, HIGH);
    delay (p_wait);
    digitalWrite(p_ledpin, LOW);
    delay (p_wait);
  }
}
