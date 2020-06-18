#include <avr/io.h>

#include "eeprom.h"

void EEPROM_write(uint16_t addr, uint8_t data)
{
	/*EEAR = addr; // Write eeprom address
	EEDR = data; // Write eeprom data
	while(EECR & (1<<EEPE)); // Wait until an eeprom operation is done
	EEAR = addr; // Write eeprom address
	EEDR = data; // Write eeprom data
	EECR |= (1<<EEMPE); // Enable eeprom write access
	EECR |= (1<<EEPE); // Enable eeprom write operation*/
}

uint8_t EEPROM_read(uint16_t addr)
{
	/*while (EECR & (1<<EEPE)); // Wait until an eeprom operation is done
	EEAR = addr; // Write eeprom address
	EECR |= (1<<EERE); // Enable eeprom read operation
	return EEDR; // Return the eeprom data stored in EEDR*/
	return 0;
}

void EEPROM_update(uint16_t addr, uint8_t data)
{
	/*if (EEPROM_read(addr) != data)
		EEPROM_write(addr, data);*/
}