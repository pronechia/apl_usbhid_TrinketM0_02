Arduinoを USB/HIDデバイスとして動作させるアプリ。
ボードは、Arduino互換の Adafruit Trinket M0　を使用する。
マイコンから定期的に、BME280で測定した温度を Browserにアップする、リアルタイムモニター。
詳細はQiita参照 https://qiita.com/pronechia/items/0d3c78723d0c88176a2b

注意）

・WEbServerのURLを、各自の環境に合わせて変更してください。

・BME280のI2Cアドレスを、各自の環境に合わせて変更してください。間違った場合は、LEDが点滅し、先に進みません。

 This is an Arduino USB/HID  sample program .
