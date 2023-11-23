#include <ShiftRegisterController.h>

//以下はArduino UNO R3を前提にしています。 This assumes Arduino UNO R3.

/*
  もし引数を省略した場合、
  クロックピン=2, ラッチピン=3, データピン=4, デジタルI/Oピン数=14(UNO)
  と設定されます。
  あなたがdigitalWrite(Read)Ext()を使用しない場合、デジタルI/Oピン数は無視しても良いです。
  boardNumber.[ボード名]はいくつかのボードのデジタルI/Oピン数を提供しますが、ただの数値なので直接入力しても構いません。
  アナログピンも含めて使用したい場合は、単純に使用したいピンの数を足せば良いです。例:boardNumber.UNO+6
*/

/*
  If the arguments are omitted, the configuration will be set to Clock Pin = 2, Latch Pin = 3, Data Pin = 4, and Digital I/O Pins = 14 (UNO).
  If you don't use digitalWrite(Read)Ext(), you can ignore the number of digital I/O pins.
  boardNumber.[BoardName] provides the number of digital I/O pins for several boards, but since it's just a numerical value, you can input it directly.
  If you want to include analog pins, simply add the number of pins you want to use. Example: boardNumber.UNO + 6.
*/
ShiftRegisterController sr(2, 3, 4, boardNumber.UNO);

void setup() {
    pinMode(8, INPUT); //デモのためなので記述する必要はありません。 Not needed to write as it is for a demo.
}

void loop() {

    //拡張digitalWrite() Extend digitalWrite()
    sr.digitalWriteExt(13, HIGH); //デジタルI/Oの13番ピンが操作されます。 Digital I/O pin 13 is manipulated.
    sr.digitalWriteExt(14, HIGH); //シフトレジスタの0番ピンが操作されます。 Shift register pin 0 is manipulated.
    delay(1000);
    sr.digitalWriteExt(14, LOW); //同じく Similarly
    sr.digitalWriteExt(13, LOW);
    delay(1000);

    //拡張digitalRead() Extend digitalRead()
    /*
      注意：値の読み取り機能はあくまでマイコンにバッファリングされている値を読み取るものであり、実際にシフトレジスタのピンを読み取っているわけではありません。
        シフトレジスタのピンに電圧を印加しないように気をつけてください。
    */
    /*
      Note: The value reading function merely reads the values buffered in the microcontroller and does not directly read the pins of the shift register.
      Please be cautious not to apply input voltage to the output pins of the shift register.
    */
    bool a = sr.digitalReadExt(8); //デジタルI/Oの8番ピンの値が読み取られます。 The value of Digital I/O pin 8 is read.
    //シフトレジスタの4番ピンの値(出力状態)が読み取られます。 The value (output state) of shift register pin 4 is read
    bool b = sr.digitalReadExt(18);

    sr.write(3, HIGH); //シフトレジスタの3番ピンが操作されます。 Shift register pin 3 is manipulated.
    sr.read(6); //シフトレジスタの6番ピンの値(出力状態)が読み取られます。 The value (output state) of shift register pin 6 is read

    //バッチ書き込みとclear(), overwrite() Batch write and clear(), overwrite()
    sr.batchWriteStart(); //バッチ書き込みモードを始めます。 Start batch write mode.
    sr.digitalWriteExt(5, HIGH);
    sr.clear(); //バッファをクリアします。(バッチ書き込みモードが無効の場合はinit()と同じ動作になります。) Clear the buffer (if batch write mode is disabled, it will behave the same as init())
    sr.write(3, HIGH);
    delay(1000);
    sr.overwrite(0b00110011); //これはすぐに適用されます。 This is applied immediately.
    delay(1000);
    sr.batchWriteApply(); //バッチ書き込みモードを始めてから入力された処理(今回は2つ)が適用されます。 The processes input since starting batch write mode (2 processes in this case) are applied.
    delay(1000);

    byte c = sr.readAllPins(); //全てのピン出力の状態を取得します。 Get the state of all pin outputs.

    sr.init(); //シフトレジスタの全てのピンをLOWにします。 Set all pins of the shift register to LOW.
}