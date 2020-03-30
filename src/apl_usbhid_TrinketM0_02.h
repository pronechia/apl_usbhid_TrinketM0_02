/*
 * apl_usbhid_TrinketM0_02 公開用
 */
#include "Keyboard.h"
#include <Wire.h>
#include "SparkFunBME280.h"
//環境に合わせてどちらかを選択すること
//#define BME280DEVADDR 0x76
//#define BME280DEVADDR 0x77
#define LED_PIN 13
#define SW_PIN  1
#define WCS_DELAY_T1 150 //T1 ブラウザー応答時間150
#define WCS_DELAY_MOJI 10 //文字間隔
#define WCS_DELAY_GAMEN 3000  //URLたたいてから画面が表示されるまで待つ

BME280 mySensor;
int16_t g_pass;  //HID出力したら後の待ち時間を制御する。 pass1=20msec pass2=40msec ...
String g_url_string = "http'//192.168.xxx.xxx/xxxxxxxx.htm"; //WindowsPC用,MAC用
