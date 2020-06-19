#ifndef SERIAL_H_
#define SERIAL_H_

#define BAUDRATE ((F_CPU / (9600UL * 16)) - 1)

void serialInit();
void serialWrite(uint8_t *c, uint8_t len);
void serialWriteByte(uint8_t c);

#endif