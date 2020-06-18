#include <util/delay.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lora.h"
#include "serial.h"
#include "spi.h"

// This is debug file, not all function will be used
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"

static const uint8_t DEBUG_STATUS[] =	"+-----------------------------------+\n\r"
										"|               DEBUG               |\n\r"
										"|            MCUSR STATUS           |\n\r"
										"|      B7 B6 B5 B4 B3 B2 B1 B0      |\n\r"
										"|       !  @  #  $  %  ^  &  *      |\n\r"
										"+-----------------------------------+\n\r";

static const uint8_t DEBUG_GSTATUS[] =	"+-----------------------------------+\n\r"
										"|               DEBUG               |\n\r"
										"|      B7 B6 B5 B4 B3 B2 B1 B0      |\n\r"
										"|       !  @  #  $  %  ^  &  *      |\n\r"
										"+-----------------------------------+\n\r";

static uint8_t* replace_char(uint8_t *str, uint8_t find, uint8_t replace)
{
	char *current_pos = strchr((const char *)str, find);
	while (current_pos) {
		*current_pos = replace;
		current_pos = strchr(current_pos, find);
	}
	return str;
}

void DEBUG_PRINT_MCUSR()
{
	_delay_ms(100);

	uint8_t tmp[sizeof(DEBUG_STATUS)];
	memcpy(tmp, DEBUG_STATUS, sizeof(DEBUG_STATUS));

	replace_char(tmp, '!', 'X');
	replace_char(tmp, '@', 'X');
	replace_char(tmp, '#', 'X');
	replace_char(tmp, '$', 'X');
	replace_char(tmp, '%', (MCUSR & (1<<WDRF))  ? '1' : '0');
	replace_char(tmp, '^', (MCUSR & (1<<BORF))  ? '1' : '0');
	replace_char(tmp, '&', (MCUSR & (1<<EXTRF)) ? '1' : '0');
	replace_char(tmp, '*', (MCUSR & (1<<PORF))  ? '1' : '0');

	serialWrite(tmp, sizeof(tmp));

	MCUSR = 0;

	_delay_ms(100);
}

void DEBUG_PRINT_BITS(uint8_t var)
{
	_delay_ms(100);

	uint8_t tmp[sizeof(DEBUG_GSTATUS)];

	memcpy(tmp, DEBUG_GSTATUS, sizeof(DEBUG_GSTATUS));

	replace_char(tmp, '!', (var & 128)  ? '1' : '0');
	replace_char(tmp, '@', (var & 64)  ? '1' : '0');
	replace_char(tmp, '#', (var & 32)  ? '1' : '0');
	replace_char(tmp, '$', (var & 16)  ? '1' : '0');
	replace_char(tmp, '%', (var & 8)  ? '1' : '0');
	replace_char(tmp, '^', (var & 4)  ? '1' : '0');
	replace_char(tmp, '&', (var & 2)  ? '1' : '0');
	replace_char(tmp, '*', (var & 1)  ? '1' : '0');

	serialWrite(tmp, sizeof(DEBUG_GSTATUS));

	_delay_ms(100);
}