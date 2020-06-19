#include <util/delay.h>

#include "lcd.h"
#include "spi.h"

static void lcdSpiTwoBits()
{
	SPIPORT |= (1<<MOSI);
	SPIPORT |= (1<<SCK);
	SPIPORT &= ~(1<<SCK);
	SPIPORT &= ~(1<<MOSI);
	
	SPIPORT &= ~(1<<MOSI);
	SPIPORT |= (1<<SCK);
	SPIPORT &= ~(1<<SCK);
	SPIPORT &= ~(1<<MOSI);	
}

static void lcdSendInst(uint16_t instruction)
{
	spiSelectChip(LCD);
	spiTransfer(instruction>>2); // Only first 10-bits matter
	spiTransfer(instruction<<6);
	spiUnselect();
	
	_delay_us(600);
	
	uint8_t busy = 1;
	
	do {
		spiSelectChip(LCD);
		spiTransfer(INST_BUSYF);
		busy = spiTransfer(0x3F);
		spiUnselect();
	} while (busy);
}

static void lcdSendData(uint8_t *buffer, uint8_t length)
{
	spiSelectChip(LCD);
	lcdSpiTwoBits();
	spiWriteData(buffer, length);
	spiUnselect();
}

void lcdInit()
{
	lcdSendInst(INST_FUNCS);
	lcdSendInst(INST_DISPD);
	lcdSendInst(INST_CLEAR);
	lcdSendInst(INST_ENTMS);
	lcdSendInst(INST_HOMEC);
	lcdSendInst(INST_DISPE);
}

void lcdWriteLine(uint8_t line, uint8_t *buffer, uint8_t length)
{
	uint8_t offset = 0;
	if (line == 1) {
		offset = 0;
	} else if (line == 2) {
		offset = 0x40;
	} else if (line == 3) {
		offset = 0x14;
	} else if (line == 4) {
		offset = 0x54;
	}
	lcdSendInst(INST_SETDR + offset);
	lcdSendData(buffer, length);
}

void lcdClear()
{
	lcdSendInst(INST_CLEAR);
}