# Shift_Register_Controller
A library for seamlessly controlling 74HC595 with Arduino.
74HC595シフトレジスタをArduinoでシームレスに制御できるようにするライブラリです。

The English text in this library has been machine translated from Japanese.

### 概要 overview
このライブラリは、74HC595を簡単に制御出来るようにするものです。
シフトレジスタを扱っているというよりは、デジタルI/Oピンを増設したような感覚でシームレスに使えます。
もちろん、シフトレジスタ専用の書き込み/読み取りメソッドやバイトデータで上書きする機能も搭載しています。

This library is designed to easily control the 74HC595 shift register. Rather than dealing directly with the shift register, it provides a seamless experience as if you were expanding digital I/O pins. Of course, it includes dedicated methods for writing/reading to the shift register and the ability to overwrite with byte data.

注意：値の読み取り機能はあくまでマイコンにバッファリングされている値を読み取るものであり、実際にシフトレジスタのピンを読み取っているわけではありません。
シフトレジスタのピンに入力電圧を印加しないように気をつけてください。
また、このライブラリはデイジーチェーンには対応していません。(理論上はこのライブラリをラッピングすることで比較的容易に実装できますが、私はシフトレジスタを一つしか持っていないので試せません。)
もしこの仕様にがっかりしたなら、Jonathan Bartlett氏が提供している[Shiftyライブラリ](https://www.github.com/johnnyb/Shifty)
を試すことがおすすめです。

Note: The read function is intended to read values buffered in the microcontroller, not to read the actual values on the shift register pins. Please be cautious not to apply input voltage to the shift register pins. Additionally, this library does not support daisy-chaining (theoretically, it can be implemented relatively easily by wrapping this library, but I cannot test it as I only have one shift register). If you find this specification disappointing, it is recommended to try the [Shifty library](https://www.github.com/johnnyb/Shifty) provided by Mr.Jonathan Bartlett.

### 搭載されている機能 Features
1. `digitalWriteExt()`:標準のdigitalWrite()を拡張する関数です。デジタルI/Oピンの番号(UNOの場合は0～13)を指定するとその番号のピンに対して操作が行われ、それ以降の数(UNOなら14～)からシフトレジスタの0番ピン～に対応しています。(`digitalReadExt()`も同様です。)
2. `write()/read()`:シフトレジスタ専用の制御関数です。使い方は`digitalWrite(Read)()`と同じです。
3. `clear()`:バッチ書き込みモードが有効ならバッファをクリアし、無効ならシフトレジスタのピン状態を初期化します。
4. `batchWriteStart()`/`batchWriteApply()`:バッチ書き込みモードを開始/終了します。開始してから終了するまでは書き込みが適用されず、`batchWriteApply()`が呼ばれると一括でシフトレジスタに適用されます。
5. `overwrite()`バイトデータを引数として渡すと、バッチ書き込みが有効でも無視して即座に適用されます。(バッファデータは保持されます。)
6. `readAllpins()`:全てのピンの状態をbyte型で返します。
7. `init()`:シフトレジスタを初期化します。バッチ書き込みモードが無効なら`clear()`と同じ動作をします。

-

1. `digitalWriteExt()`: Extends the standard `digitalWrite()` function. When specifying the number of a digital I/O pin (0 to 13 for UNO), it operates on that pin, and from then on, it corresponds to the 0th pin to the shift register from subsequent numbers (14 and onwards for UNO) (similarly for `digitalReadExt()`).
2. `write()`/`read()`: Shift register-specific control functions. Usage is the same as `digitalWrite(Read)()`.
3. `clear()`: If batch write mode is active, clears the buffer; otherwise, initializes the shift register pin states.
4. `batchWriteStart()`/`batchWriteApply()`: Initiates/ends batch write mode. During this period, the writes are not applied until `batchWriteApply()` is called, which then applies them all at once to the shift register.
5. `overwrite()`: Passing byte data as an argument immediately applies it, ignoring the batch write mode (buffer data is retained).
6. `readAllpins()`: Returns the state of all pins as a byte.
7. `init()`: Initializes the shift register. If batch write mode is inactive, it behaves the same as `clear()`.

| 関数名 function name | 引数 argument | 戻り値 return value |
|---------------------|---------------|---------------------|
| init                | void           | void                |
| digitalWriteExt     | uint8_t, bool | void                |
| digitalReadExt     | uint8_t        | bool                |
| write              | uint8_t, bool  | void                |
| read               | uint8_t        | bool                |
| batchWriteStart    | void           | void                |
| batchWriteApply      | void           | void                |
| overwrite          | byte           | void                |
| readAllPins        | void           | byte                |

### 基本的な使い方 Basic Usage
例えば、クロックピン=2, ラッチピン=3, データピン=4に繋がっているシフトレジスタをUNOで制御する場合、デジタルI/Oの13番ピンとシフトレジスタの0番ピンを点滅させるプログラムは以下のようになります。

For example, if you want to control a shift register connected to clock pin 2, latch pin 3, and data pin 4 with an UNO, and you want to blink digital I/O pin 13 and shift register pin 0, the program would look like the following:

```
#include <ShiftRegisterController.h>
ShiftRegisterController sr(2, 3, 4, boardNumber.UNO);

void setup() {}

void loop() {
    //拡張digitalWrite() Extend digitalWrite()
    sr.digitalWriteExt(13, HIGH); //デジタルI/Oの13番ピンが操作されます。 Digital I/O pin 13 is manipulated.
    sr.digitalWriteExt(14, HIGH); //シフトレジスタの0番ピンが操作されます。 Shift register pin 0 is manipulated.
    delay(1000);
    sr.digitalWriteExt(14, LOW); //同じく Similarly
    sr.digitalWriteExt(13, LOW);
    delay(1000);
}
```

### その他の機能の使用例 The use case of other features
- digitalReadExt()
  ```
    bool a = sr.digitalReadExt(8); //デジタルI/Oの8番ピンの値が読み取られます。 The value of Digital I/O pin 8 is read.
    //シフトレジスタの4番ピンの値(出力状態)が読み取られます。 The value (output state) of shift register pin 4 is read
    bool b = sr.digitalReadExt(18);
  ```
- write()/read()
  ```
  sr.write(3, HIGH); //シフトレジスタの3番ピンが操作されます。 Shift register pin 3 is manipulated.
  sr.read(6); //シフトレジスタの6番ピンの値(出力状態)が読み取られます。 The value (output state) of shift register pin 6 is read
  ```

- batchWriteStart/Apply()
  ```
  sr.batchWriteStart(); //バッチ書き込みモードを始めます。 Start batch write mode.
  sr.digitalWriteExt(5, HIGH);
  sr.write(3, HIGH);
  delay(1000);
  sr.batchWriteApply(); //バッチ書き込みモードを始めてから入力された処理(今回は2つ)が適用されます。 The processes input since starting batch write mode (2 processes in this case) are applied.
  delay(1000);
  ```
- overwrite()
  ```
  sr.overwrite(0b00110011); //これはすぐに適用されます。 This is applied immediately.
  ```
- readAllPins()
  ```
  byte c = sr.readAllPins(); //全てのピン出力の状態を取得します。 Get the state of all pin outputs.
  ```

詳細は付属しているサンプルコードを確認してください。

For more details, please refer to the accompanying sample code.

