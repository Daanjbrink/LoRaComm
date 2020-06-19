#include <avr/io.h>

#include <stdint.h>

#include "serial.h"

void serialInit()
{
	UBRR0H = (BAUDRATE>>8);
    UBRR0L = BAUDRATE;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0); // Enable transmitter
}

void serialWrite(uint8_t *c, uint8_t length)
{
	while (length--)
		serialWriteByte(*c++);
}

inline void serialWriteByte(uint8_t c)
{
	while (!(UCSR0A & (1<<UDRE0))); // Wait until UART is ready
	UDR0 = c;
	while (!(UCSR0A & (1<<UDRE0))); // Wait until byte is shifted out
}