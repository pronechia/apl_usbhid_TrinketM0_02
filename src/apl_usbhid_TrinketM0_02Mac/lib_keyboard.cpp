/*
 * lib_keyboard.cpp  v1.0
 * 
 */
#include "lib_keyboard.h"
int8_t g_keyboard_OSType; //1:windows 2:Mac sub_kbd_begin時にOStypeを指定すること

//
//KBD 初期化
void sub_kbd_begin(int8_t p_ostype) {
  //p_ostype  1:windows other:Mac
  g_keyboard_OSType = p_ostype;
  Keyboard.begin();
}
//文字列　＋　TABキー出力
void sub_moji_tab(char *p_moji) {

  sub_kbd_print(p_moji);
  sub_kbd_strok(KEY_TAB);
  delay(WCS_DELAY_T1);
}
//文字列出力
void sub_kbd_print(char *p_moji) {

  while (*p_moji) {
    sub_kbd_strok(*p_moji);
    p_moji++;
    delay(WCS_DELAY_MOJI);
  }
}
//一文字のキーコード出力
void sub_kbd_strok(uint8_t p_char) {

  if (p_char == KEY_TAB) {
    if (g_keyboard_OSType == 1) {
      Keyboard.write(KEY_TAB);
    } else {
      //Mac
      Keyboard.press(KEY_LEFT_ALT);
      delay(WCS_DELAY_MOJI);
      Keyboard.write(KEY_TAB);
      delay(WCS_DELAY_MOJI);
      Keyboard.release(KEY_LEFT_ALT);
    }
  } else {
    Keyboard.press(p_char);
    Keyboard.release(p_char);
  }
  delay(WCS_DELAY_MOJI);
}
//
//URL入力前に入力エリアにフォーカスを当てる
void sub_kbd_preURL() {

  if (g_keyboard_OSType == 1) {
    //Windows用
    //Windows-KEY + r　でcmd窓を出す
    sub_kbd_withmodifire(KEY_LEFT_GUI, 'r');
  } else {
    //Mac用
    //ブラウザーが表示されていること。URL入力エリアをフォーカスする
    sub_kbd_withmodifire(KEY_LEFT_GUI, 'l');
  }
}
//一文字のキーコード出力　修飾キー付き
void sub_kbd_withmodifire(char p_modifire, char p_moji) {

  Keyboard.press(p_modifire);
  Keyboard.write(p_moji);
  Keyboard.release(p_modifire);
  delay(WCS_DELAY_T1);
}
