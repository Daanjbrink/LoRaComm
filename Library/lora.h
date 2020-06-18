#ifndef LORA_H_
#define LORA_H_

#define LORA_REG_FIFO			0x00
#define LORA_REG_OP_MODE		0x01
#define LORA_REG_FRF_MSB		0x06
#define LORA_REG_FRF_MID		0x07
#define LORA_REG_FRF_LSB		0x08
#define LORA_REG_PA_CONFIG		0x09
#define LORA_REG_FIFO_ADDR_PTR	0x0D
#define LORA_REG_FIFO_TX_PTR	0x0E
#define LORA_REG_IRQ_FLAGS		0x12
#define lORA_REG_CONFIG			0x1D
#define lORA_REG_CONFIG2		0x1E
#define LORA_REG_LENGTH			0x22
#define LORA_REG_CONFIG3		0x26
#define	LORA_REG_SYNC_WORD		0x39

#define LORA_MODE_SLEEP			0x00
#define LORA_MODE_STDBY			0x01
#define LORA_MODE_TX			0x03
#define LORA_MODE_LONG_RANGE	0x80

#define LORA_HOP_CHANNEL		0x1C

#define LORA_FLAG_TX_DONE		0x08

#define LORA_MASK_WRITE	0x80
#define LORA_MASK_READ	0x7F

uint16_t frameCounter;

void loraInit();
void loraWriteReg(uint8_t reg, uint8_t val);
uint8_t loraReadReg(uint8_t reg);
void loraSendData(uint8_t *data, uint8_t data_length);

#endif