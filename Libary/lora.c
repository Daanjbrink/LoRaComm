#include <util/delay.h>

#include "aes.h"
#include "eeprom.h"
#include "lora.h"
#include "radio.h"
#include "spi.h"

#pragma GCC diagnostic ignored "-Wchar-subscripts"

static inline uint8_t loraTransferReg(uint8_t reg, uint8_t val)
{
	spiSelectChip(LORA);
	spiTransfer(reg); // First transfer register address
	uint8_t ret = spiTransfer(val); // Then the value
	spiUnselect();
	return ret;
}

void loraInit()
{
	aesInit();
	
	frameCounter = EEPROM_read(0);

	loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_SLEEP);
	_delay_ms(1); // Module needs 250us to switch, datasheet sx1276 section 2.5.2
	loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_SLEEP | LORA_MODE_LONG_RANGE);

	loraWriteReg(LORA_REG_PA_CONFIG, 0xFF); // Maximum output power
	loraWriteReg(lORA_REG_CONFIG,  0x78); // 125 kHz, 4/8 coding rate, explicit header mode
	loraWriteReg(lORA_REG_CONFIG2, 0xA4); // SF10, CRC On
	loraWriteReg(LORA_REG_SYNC_WORD, 0x34); // LoRa sync word
	loraWriteReg(LORA_REG_FIFO_TX_PTR, 0x00); // Tx buffer pointer

	// Set frequency - 868.1 MHz
	loraWriteReg(LORA_REG_FRF_MSB, 0xD9);
	loraWriteReg(LORA_REG_FRF_MID, 0x06);
	loraWriteReg(LORA_REG_FRF_LSB, 0x66);
}

void loraWriteReg(uint8_t reg, uint8_t val)
{
	loraTransferReg(reg | LORA_MASK_WRITE, val); // Set MSB
}

uint8_t loraReadReg(uint8_t reg)
{
	return loraTransferReg(reg & LORA_MASK_READ, 0); // Unset MSB
}

void loraSendData(uint8_t *data, uint8_t dataLength)
{
	//Direction of frame is up
	uint8_t direction = 0x00;

	uint8_t rfmData[64];
	uint8_t rfmPackageLength;

	uint8_t mic[4];
	uint8_t macHeader = 0x40;

	uint8_t frameControl = 0x00;
	uint8_t framePort = 0x01;

	//Encrypt the data
	aesEncryptPayload(data, dataLength, frameCounter, direction);

	//Build the Radio Package
	rfmData[0] = macHeader;

	rfmData[1] = devaddr[3];
	rfmData[2] = devaddr[2];
	rfmData[3] = devaddr[1];
	rfmData[4] = devaddr[0];

	rfmData[5] = frameControl;

	rfmData[6] = (frameCounter & 0x00FF);
	rfmData[7] = ((frameCounter >> 8) & 0x00FF);

	rfmData[8] = framePort;

	//Set Current package length
	rfmPackageLength = 9;

	//Load Data
	for (uint8_t i = 0; i < dataLength; i++) {
		rfmData[rfmPackageLength + i] = data[i];
	}

	//Add data Lenth to package length
	rfmPackageLength += dataLength;

	//Calculate MIC
	aesCalculateMic(rfmData, mic, rfmPackageLength, frameCounter, direction);
	EEPROM_update(0, ++frameCounter); // Update framecounter asap

	//Load MIC in package
	for (uint8_t i = 0; i < 4; i++) {
		rfmData[i + rfmPackageLength] = mic[i];
	}

	//Add MIC length to RFM package length
	rfmPackageLength += 4;

	//Send Package
	radioSend(rfmData, rfmPackageLength);
}