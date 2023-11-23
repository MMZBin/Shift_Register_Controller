#include <ShiftRegisterController.h>

//7セグメントLEDを点灯させるサンプル Sample of lighting up 7 segment LED

ShiftRegisterController sr(2, 3, 4, boardNumber.UNO);

//LEDの点灯パターン LED lighting pattern
const byte pattern[10] = {
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00100111, //7
    0b01111111, //8
    0b01101111, //9
};

int count = 0;

void setup() {}

void loop() {
    for (int i = 0; i < 10; i++) {
        sr.overwrite(pattern[i]);
        delay(1000);
    }
}