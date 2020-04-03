Arduinoを USB/HIDデバイスとして動作させるアプリ。
ボードは、Arduino互換の Adafruit Trinket M0　を使用する。
マイコンから定期的に、BME280で測定した温度を Browserにアップする、リアルタイムモニター。
詳細はQiita参照 https://qiita.com/pronechia/items/0d3c78723d0c88176a2b


注意）

・Windows用とMac用で、ソースを分離しました。環境に合わせて利用してください。

　各自の環境にあわせて確認、修正する箇所があります。ソースコードの上部のコメントを参考に、対応してください。

・実行時の注意

  BME280のI2Cアドレスが間違っている場合は、LEDが点滅し、先に進みません。
  
　WindowsPC、Macとも、入力モードは、英数字である必要があります。日本語モードになっていないことを確認してください。
 
 This is an Arduino USB/HID  sample program .
