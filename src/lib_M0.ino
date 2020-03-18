/*
 * 
 */
//
#include <ZeroTimer.h>

volatile int g_i;
//
//タイマー割込み関数
//
void sub_timer_callback() {
  // 指定した間隔(default 20msec)で割込みが入る
  g_i++;
}
//
//
//
void sub_fw_timerset() {
  //timer設定。動き始めるので、最後に行う。
  TC.startTimer(20000, sub_timer_callback);  //20msec
}
//
//2カウント=約40msecでイベントを上げる
// rc true =event on
boolean sub_fw_event(uint8_t p_count) {
  boolean w_trriger;

  if (p_count > 25) return 0; //ERROR check  
  w_trriger = false;
  // g_i>50で約一秒。g_i>0で約20msec
  if (g_i > p_count) {
    g_i = 0;
    w_trriger = true;
  }
  return w_trriger;
}
//
//SWが押されたかのチェック。二度呼ばれて、連続して押されていたらONを返す。
// rc 0:OFF 1:立ち上がり 2:立下り
//
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
//
void sub_fw_Blink_onoff(uint8_t p_ledpin, byte p_onoff ,int p_wait) {
  digitalWrite(p_ledpin, p_onoff);
  delay (p_wait);
}
//p_timesは最大255とする
void sub_fw_Blink(uint8_t p_ledpin, byte p_times , int p_wait){
  for (byte i=0; i< p_times; i++){
    sub_fw_Blink_onoff(p_ledpin, HIGH ,p_wait);
    sub_fw_Blink_onoff(p_ledpin, LOW  ,p_wait);    
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
    w_temp =  BME280temperature(); //xx.xx度
    if (w_temp > 9999) sprintf(w_buf, "100.00");
    else {
      w_temp2 = w_temp - (w_temp / 100 * 100);
      sprintf(w_buf, "%d.%02d", w_temp / 100 ,w_temp2);
    }
    sub_moji_tab(w_buf);
    sub_moji_tab("C");
    sub_kbd_strok(KEY_RETURN);  //ボタンを押す
  }
}
//
//アプリ画面を呼び出すURLを出力する
//
void sub_initurl() {
    
  //Windows-KEY + r　でcmd窓を出す
  sub_kbd_withmodifire(KEY_LEFT_GUI, 'r');
  delay(WCS_DELAY_T1);
  //URL文字出力
  sub_kbd_print((char *)g_url_string.c_str());
  sub_kbd_strok(KEY_RETURN);
  //
  delay(WCS_DELAY_GAMEN); //画面が表示されるまで、十分の時間待つ
}
//
//KBD 初期化
//
void sub_kbd_begin() {
    Keyboard.begin();
}
//
void sub_moji_tab(char *p_moji) {

  sub_kbd_print(p_moji);
  sub_kbd_strok(KEY_TAB);
  delay(WCS_DELAY_T1);
}
//
void sub_kbd_print(char *p_moji) {

  while (*p_moji) {
    sub_kbd_strok(*p_moji);
    p_moji++;
    delay(WCS_DELAY_MOJI);
  }
}
//USB/HIDキーコードを送る
void sub_kbd_strok(uint8_t p_char) {

  if (p_char == KEY_TAB) {
    Keyboard.write(KEY_TAB);
  } else {
    Keyboard.press(p_char);
    Keyboard.release(p_char);
  }
  delay(WCS_DELAY_MOJI);
}
//修飾キー付きの文字（一文字）
void sub_kbd_withmodifire(char p_modifire, char p_moji) {

  Keyboard.press(p_modifire);
  Keyboard.write(p_moji);
  Keyboard.release(p_modifire);
  delay(WCS_DELAY_T1);
}
