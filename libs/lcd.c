#include "lcd.h"
#include "i2c.h"

uint8_t lcdAddr = 0x27;

#define BT BIT3
#define EN BIT2
#define RW BIT1


void lcdInit() {
                                        // Testa o endereço do PCF/LCD
    if(i2cSendByte(lcdAddr, 0))
        lcdAddr = 0x3F;

    lcdWriteNibble(0x3, 0);             // Entra em modo 8bits
    lcdWriteNibble(0x3, 0);
    lcdWriteNibble(0x3, 0);

    lcdWriteNibble(0x2, 0);             // Entra em modo 4bits

    lcdWriteByte(0x0F, 0);              // Liga o cursos piscando e liga o display
    lcdWriteByte(0x02, 0);              // Return home and clear display
    lcdWriteByte(0x01, 0);
}


void lcdWriteNibble(uint8_t nibble, uint8_t rs){
                                   //  BT   EN   RW  RS
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | 0 | rs );
    i2cSendByte(lcdAddr, nibble << 4 | BT | EN | 0 | rs );
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | 0 | rs );
}

void lcdWriteByte(uint8_t byte, uint8_t rs){

    // (rs = 1) write, (rs = 0) instruction
    lcdWriteNibble(byte >> 4, rs);
    lcdWriteNibble(byte & 0x0F , rs);
}

void lcdPrint (char* str){
    uint8_t count = 0;

    while(*str){
        count++;
        lcdWriteByte(*str++, 1);

        if (count == 16)
            lcd2Line();
    }
}

void lcd2Line(){
    lcdWriteByte(0xC0, 0);
}

void lcdBackLight(uint8_t isOn){
    i2cSendByte(lcdAddr, isOn << 3);
}

uint8_t lcdBusy(){
    // NOT SURE ABOUT THIS
    return i2cSend(lcdAddr, &lcdAddr, 1);
}

void lcdDecimal8(uint8_t num){
    uint8_t unidade, dezena, centena, resto;
    centena = num/100;
    resto = num%100;

    dezena = resto/10;
    resto = resto%10;

    unidade = resto;

    lcdWriteByte(centena + 0x30, 1);
    lcdWriteByte(dezena + 0x30, 1);
    lcdWriteByte(unidade + 0x30, 1);
    lcdPrint(" ");
}

void lcdDecimal16(uint16_t num){
    uint16_t unidade, dezena, centena, milhar, dezenaMilhar, resto;

    dezenaMilhar = num/10000;
    resto = num%10000;

    milhar = resto/1000;
    resto = resto%1000;

    centena = resto/100;
    resto = resto%100;

    dezena = resto/10;
    resto = resto%10;

    unidade = resto;

    lcdWriteByte(dezenaMilhar + 0x30, 1);
    lcdWriteByte(milhar + 0x30, 1);
    lcdWriteByte(centena + 0x30, 1);
    lcdWriteByte(dezena + 0x30, 1);
    lcdWriteByte(unidade + 0x30, 1);
    lcdPrint(" ");
}

void lcdHex8(uint8_t num){
    uint8_t num1, num2, char1, char2;

    num1 = (num & 0xF0) >> 4;
    num2 = (num & 0x0F);

    if (num1 < 10)
        char1 = num1 + 0x30;
    else
        char1 = num1 + 0x41 - 10;

    if (num2 < 10)
            char2 = num2 + 0x30;
        else
            char2 = num2 + 0x41 - 10;

    lcdPrint("0x");
    lcdWriteByte(char1, 1);
    lcdWriteByte(char2, 1);
    lcdPrint(" ");
}

void lcdHex16(uint16_t num){
    uint8_t num1, num2, num3, num4;
    uint8_t char1, char2, char3, char4;

    num1 = (num & 0xF000) >> 12;
    num2 = (num & 0x0F00) >> 8;
    num3 = (num & 0x00F0) >> 4;
    num4 = (num & 0x000F);

    if (num1 < 10)
        char1 = num1 + 0x30;
    else
        char1 = num1 + 0x41 - 10;

    if (num2 < 10)
            char2 = num2 + 0x30;
    else
            char2 = num2 + 0x41 - 10;
    if (num3 < 10)
        char3 = num3 + 0x30;
    else
        char3 = num3 + 0x41 - 10;

    if (num4 < 10)
            char4 = num4 + 0x30;
    else
            char4 = num4 + 0x41 - 10;


    lcdPrint("0x");
    lcdWriteByte(char1, 1);
    lcdWriteByte(char2, 1);
    lcdWriteByte(char3, 1);
    lcdWriteByte(char4, 1);
    lcdPrint(" ");
}

void lcdCursor(uint8_t row, uint8_t col){

    if (row > 2 || col > 40)
        return;

    lcdWriteByte(0x02, 0);
    if (row == 2)
        lcd2Line();
    while(col--)
        lcdWriteByte(0x14, 0);
}






