#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

#define INST_FUNCS	0x0029 // DL = 0, Western European #1
#define INST_DISPD	0x000A // Display is off, cursor is on, blinking is off
#define INST_CLEAR	0x0001
#define INST_ENTMS	0x0006
#define INST_HOMEC	0x0002
#define INST_DISPE	0x000E // Display is off, cursor is on, blinking is off
#define INST_BUSYF	0x0040
#define INST_SETDR	0x0080

void lcdInit();
void lcdWriteLine(uint8_t line, uint8_t *buffer, uint8_t length);
void lcdWriteScreen(uint8_t *screen);
void lcdClear();

#endif