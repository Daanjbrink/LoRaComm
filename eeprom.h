#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>

void EEPROM_write(uint16_t addr, uint8_t data);
uint8_t EEPROM_read(uint16_t addr);
void EEPROM_update(uint16_t addr, uint8_t data);

#endif