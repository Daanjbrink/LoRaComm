#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

#include <stdint.h>

#define GPS		PINC2
#define LORA	PINB2
#define MOSI	PINB3
#define MISO	PINB4
#define SCK		PINB5

void spiInit();

void spiSelectChip(uint8_t port);
void spiUnselect();
uint8_t spiTransfer(uint8_t data);

void spiReadData(uint8_t *data, uint8_t length);
void spiWriteData(uint8_t *data, uint8_t length);

#endif