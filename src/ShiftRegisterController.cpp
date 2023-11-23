#include <Arduino.h>
#include "ShiftRegisterController.h"

//ボードモデル(デジタルI/Oピン数) Board Model (Number of Digital I/O Pins)
const BoardNumber boardNumber = {
    .UNO =      14,
    .NANO =     14,
    .MICRO =    10,
    .MEGA =     54,
    .DUE =      54,
    .LEONARDO = 20
};

//public=================================================================================================================

//コンストラクタ Constructor
ShiftRegisterController::ShiftRegisterController(uint8_t clockPin, uint8_t latchPin, uint8_t dataPin, uint8_t numPins)
: clockPin_(clockPin), latchPin_(latchPin), dataPin_(dataPin), eachPinStates_(0), eachPinStatesBuffer_(0), NUM_PINS(numPins), isBatchWrite_(false) {
    //初期化 Initialization
    pinMode(clockPin_, OUTPUT);
    pinMode(latchPin_, OUTPUT);
    pinMode(dataPin_, OUTPUT);
}

//初期化 Initialization
void ShiftRegisterController::init() {
    eachPinStates_ = 0;
    sendData();
}

//通常のインターフェース Normal Interface
//  標準のI/Oを拡張する Extend Standard I/O
//      拡張digitalWrite() Extend digitalWrite()
void ShiftRegisterController::digitalWriteExt(uint8_t pin, bool mode) {
    //通常のピンの範囲であれば標準のdigitalWrite()を呼ぶ Call the standard digitalWrite() within the range of normal pins.
    if (pin < NUM_PINS) {
        digitalWrite(pin, mode);
        return;
    }

    uint8_t srPin = pin - NUM_PINS; //シフトレジスタのピンを取得する Retrieve the pins of the shift register.
    setEachPinStates(srPin, mode); //範囲外ならシフトレジスタのビットをセットする If outside the range, set the bit of the shift register.
}

//      拡張digitalRead() Extend digitalRead()
bool ShiftRegisterController::digitalReadExt(uint8_t pin) {
    //通常のピンの範囲であれば標準のdigitalRead()を呼ぶ Call the standard digitalRead() within the range of normal pins.
    if (pin < NUM_PINS) { return digitalRead(pin); }
    return getBit(pin - NUM_PINS); //指定したピンからデジタルIOピンを引いた数(シフトレジスタのピン)の状態を返す Return the state of the specified number of digital I/O pins (shift register pins) pulled from the specified pin.
}

//  シフトレジスタのピンを直接操作する Directly Manipulate Shift Register Pins
//      ビットデータを書き込む Write Bit Data
void ShiftRegisterController::write(uint8_t srPin, uint8_t mode) { setEachPinStates(srPin, mode); }
//      読み込む Read
bool ShiftRegisterController::read(uint8_t srPin) { return getBit(srPin); }
//      クリア Clear
void ShiftRegisterController::clear() {
    if (isBatchWrite_) { eachPinStatesBuffer_ = 0; }
    else { init(); }
}

//バッチ書き込み Batch Write
//  バッチ書き込みを始める Begin Batch Write
void ShiftRegisterController::batchWriteStart() {
    isBatchWrite_ = true;
    eachPinStatesBuffer_ = eachPinStates_;
}

//  バッチ書き込みを終わる(適用する) End Batch Write (Apply)
void ShiftRegisterController::batchWriteApply() {
    isBatchWrite_ = false;
    eachPinStates_ = eachPinStatesBuffer_; //バッファを適用 Apply the buffer.
    eachPinStatesBuffer_ = 0;
    sendData();
}

//比較的高度な処理用 For Relatively Advanced Processing
//  手動でデータを更新する(バッチ書き込みは無視する) Manually Update Data (Ignoring Batch Write)
void ShiftRegisterController::overwrite(byte eachPinStates) {
    eachPinStates_ = eachPinStates; //ビットデータを上書きする Overwrite bit data.
    sendData();
}

//  全てのシフトレジスタのピン状態を取得する Get State of All Shift Register Pins
byte ShiftRegisterController::readAllPins() { return eachPinStates_; }

//private=================================================================================================================

//ピンの状態を更新する Update Pin States
void ShiftRegisterController::setEachPinStates(uint8_t srPin, bool mode) {
    if (srPin >= NUM_SR_PINS) { return; } //シフトレジスタのピンの範囲外ならキャンセル Cancel if outside the range of shift register pins.

    controlBit(srPin, mode); //ビットを操作する Operate on the bit.
    if (!isBatchWrite_) { sendData(); }
}

//データをシフトレジスタに送信する Send Data to the Shift Register
void ShiftRegisterController::sendData() {
    setLatch(LOW);     //ラッチをLOWにする Set the latch to LOW.
    shiftOutAllBits(); //ビット情報を送信する Transmit bit information.
    setLatch(HIGH);    //ラッチをHIGHにする Set the latch to HIGH.
    setData(LOW);      //データピンをリセットする Reset the data pin.
}

//シフトレジスタの制御ピンを管理する Manage Control Pins of the Shift Register
//クロックを制御する Control the Clock
void ShiftRegisterController::setClock(bool mode) { digitalWrite(clockPin_, mode); }
//ラッチを制御する Control the Latch
void ShiftRegisterController::setLatch(bool mode) { digitalWrite(latchPin_, mode); }
//データを制御する Control the Data
void ShiftRegisterController::setData(bool mode) { digitalWrite(dataPin_, mode); }

//ビット制御系 Bit Control System
//  指定したビットの状態を取得する Get State of Specified Bit
bool ShiftRegisterController::getBit(uint8_t bit) { return eachPinStates_ & (1 << bit); }
//  ビットを管理する Manage Bits
void ShiftRegisterController::controlBit(uint8_t bit, bool mode) { mode ? setBit(bit) : clearBit(bit); }
//  指定したビットを立てる Set a Specified Bit
void ShiftRegisterController::setBit(uint8_t bit) {
    //1をbit分左にシフトしてOR演算(片方が1なので必ず1になる) Shift 1 left by the bit and perform OR operation (always becomes 1 since one side is 1).
    //バッチ書き込みが有効だったらバッファを更新する If batch write is enabled, update the buffer.
    if (isBatchWrite_) { eachPinStatesBuffer_ |= (1 << bit); }
    else { eachPinStates_ |= (1 << bit); }
}

//  指定したビットを折る Clear a Specified Bit
void ShiftRegisterController::clearBit(uint8_t bit) {
    //1をbit分左にシフトして反転->AND演算(片方が0なので必ず0になる) Shift 1 left by the bit, then invert -> AND operation (always becomes 0 since one side is 0).
    //バッチ書き込みが有効だったらバッファを更新する If batch write is enabled, update the buffer.
    if (isBatchWrite_) { eachPinStatesBuffer_ &= ~(1 << bit); }
    else { eachPinStates_ &= ~(1 << bit); }
}

//  全てのビットをシフトアウトする Shift Out All Bits
void ShiftRegisterController::shiftOutAllBits() {
    //シフトレジスタの全ての出力ピンに適用 Apply to all output pins of the shift register.
    for (uint8_t i = 0; i < NUM_SR_PINS; i++) {
        setData(getBit((NUM_SR_PINS-1) - i)); //シフトレジスタのピンと対応するビットの状態を出力する Output the state of the shift register pins corresponding to the bits.

        //クロックピンを切り替えてデータを送る Switch the clock pin and send the data.
        setClock(HIGH);
        setClock(LOW);
    }
}
