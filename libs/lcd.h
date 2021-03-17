#ifndef LCD_H_
#define LCD_H_

#include "i2c.h"

void    lcdInit();
void    lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void    lcdWriteByte(uint8_t byte, uint8_t isChar);
void    lcdPrint (char* str);
void    lcdBackLight(uint8_t isOn);
void    lcd2Line();
void    lcdDecimal8(uint8_t num);
void    lcdDecimal16(uint16_t num);
void    lcdHex8(uint8_t num);
void    lcdHex16(uint16_t num);
void    lcdCursor(uint8_t row, uint8_t col);
uint8_t lcdBusy();

#endif /* LCD_H_ */
