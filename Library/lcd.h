#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

void lcdInit();
void lcdWrite(uint8_t line, uint8_t *buffer, uint8_t line);
void lcdClear();

#endif