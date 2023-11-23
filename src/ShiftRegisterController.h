#ifndef SHIFT_REGISTER_CONTROLLER_H
#define SHIFT_REGISTER_CONTROLLER_H


//ボードモデル(デジタルI/Oピン数) Board Model (Number of Digital I/O Pins)
struct BoardNumber {
    const uint8_t UNO;
    const uint8_t NANO;
    const uint8_t MICRO;
    const uint8_t MEGA;
    const uint8_t DUE;
    const uint8_t LEONARDO;
};

extern const BoardNumber boardNumber;

//シフトレジスタ制御 Shift Register Control
class ShiftRegisterController {
public:
    //コンストラクタ Constructor
    ShiftRegisterController(uint8_t clockPin=2, uint8_t latchPin=3, uint8_t dataPin=4, uint8_t numPins=boardNumber.UNO);

    //初期化 Initialization
    void init();

    //通常のインターフェース Normal Interface
    //  標準のI/Oを拡張する Extend Standard I/O
    //      拡張digitalWrite() Extend digitalWrite()
    void digitalWriteExt(uint8_t pin, bool mode);
    //      拡張digitalRead() Extend digitalRead()
    bool digitalReadExt(uint8_t pin);

    //  シフトレジスタのピンを直接操作する Directly Manipulate Shift Register Pins
    //      ビットデータを書き込む Write Bit Data
    void write(uint8_t srPin, uint8_t mode);
    //      読み込む Read
    bool read(uint8_t srPin);
    //      クリア Clear
    void clear();

    //バッチ書き込み Batch Write
    //  バッチ書き込みを始める Begin Batch Write
    void batchWriteStart();
    //  バッチ書き込みを終わる(適用する) End Batch Write (Apply)
    void batchWriteApply();

    //比較的高度な処理用 For Relatively Advanced Processing
    //  手動でデータを更新する(バッチ書き込みは無視する) Manually Update Data (Ignoring Batch Write)
    void overwrite(byte eachPinStates);
    //  全てのシフトレジスタのピン状態を取得する Get State of All Shift Register Pins
    byte readAllPins();

private:
    //クロック(11) Clock (11)
    const uint8_t clockPin_;
    //ラッチ(12) Latch (12)
    const uint8_t latchPin_;
    //データ(14) Data (14)
    const uint8_t dataPin_;
    //ピンの数 Number of Pins
    const uint8_t NUM_PINS;
    //シフトレジスタの出力ピンの数 Number of Output Pins on the Shift Register
    const uint8_t NUM_SR_PINS = 8;

    //シフトレジスタの各ピンの状態 State of Each Pin on the Shift Register
    byte eachPinStates_;
    //バッチ書き込みのバッファ Batch Write Buffer
    byte eachPinStatesBuffer_;

    //バッチ書き込みモード Batch Write Mode
    bool isBatchWrite_;

    //ピンの状態を更新する Update Pin States
    void setEachPinStates(uint8_t srPin, bool mode);

    //データをシフトレジスタに送信する Send Data to the Shift Register
    void sendData();

    //シフトレジスタの制御ピンを管理する Manage Control Pins of the Shift Register
    //クロックを制御する Control the Clock
    void setClock(bool mode);
    //ラッチを制御する Control the Latch
    void setLatch(bool mode);
    //データを制御する Control the Data
    void setData(bool mode);

    //ビット制御系 Bit Control System
    //  指定したビットの状態を取得する Get State of Specified Bit
    bool getBit(uint8_t bit);
    //  ビットを管理する Manage Bits
    void controlBit(uint8_t bit, bool mode);
    //  指定したビットを立てる Set a Specified Bit
    void setBit(uint8_t bit);
    //  指定したビットを折る Clear a Specified Bit
    void clearBit(uint8_t bit);
    //  全てのビットをシフトアウトする Shift Out All Bits
    void shiftOutAllBits();
};

#endif
