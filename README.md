Arduinoを USB/HIDデバイスとして動作させるアプリ。 ボードは、Arduino互換の Adafruit Trinket M0　を使用する。 radiko(ラジコ)を Arduino から操作する。
Arduinoの USB/HID を使って文字列を出力することで、 radiko をON/OFFしたり、チャンネル切り替えをおこないます。操作は Arduinoに設置したSW二つを使って行います。USB/HIDからブラウザーへの上り通信だけで実現しています。下り通信はありません。

詳細はQiita参照 https://qiita.com/pronechia/private/9247f6ff507f5a7d04de (限定公開中)

注意）

・Windows用とMac用で、ソースを分離しました。環境に合わせて利用してください。

各自の環境にあわせて確認、修正する箇所があります。ソースコードの上部のコメントを参考に、対応してください。

・実行時の注意

WindowsPC、Macとも、入力モードは、英数字である必要があります。日本語モードになっていないことを確認してください。

This is an Arduino USB/HID with Tone sample program .
