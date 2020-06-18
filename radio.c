#include <util/delay.h>

#include <stdio.h>
#include <stdint.h>

#include "debug.h"
#include "lora.h"
#include "radio.h"
#include "serial.h"

void radioSend(uint8_t *buffer, uint8_t length)
{
	loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_SLEEP | LORA_MODE_LONG_RANGE);

	_delay_ms(1); // Module needs 250us to switch, datasheet sx1276 section 2.5.2

	loraWriteReg(LORA_REG_LENGTH, length);
	loraWriteReg(LORA_REG_FIFO_ADDR_PTR, 0x00);

	for (uint8_t i = 0; i < length; i++) {
		loraWriteReg(LORA_REG_FIFO, *buffer++);
	}

	loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_LONG_RANGE | LORA_MODE_TX); // Transmit packet
	while ((loraReadReg(LORA_REG_IRQ_FLAGS) & LORA_FLAG_TX_DONE) != LORA_FLAG_TX_DONE);
	loraWriteReg(LORA_REG_IRQ_FLAGS, LORA_FLAG_TX_DONE); // Clear flag
	loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_SLEEP);
}