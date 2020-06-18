#include <avr/pgmspace.h>

#include <string.h>

#include "aes.h"

#pragma GCC diagnostic ignored "-Wchar-subscripts"
#pragma GCC diagnostic ignored "-Wsequence-point"

static const uint8_t s_table[16][16] PROGMEM = {
	{0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76},
	{0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0},
	{0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15},
	{0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75},
	{0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84},
	{0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF},
	{0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8},
	{0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2},
	{0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73},
	{0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB},
	{0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79},
	{0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08},
	{0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A},
	{0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E},
	{0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF},
	{0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16}
};

void aesInit()
{
	memcpy(nwkkey, (uint8_t [16]){ 0xDA, 0x6E, 0x9E, 0xBC, 0x37, 0xDD, 0x24, 0x80, 0xD9, 0xB5, 0xF5, 0x17, 0x2E, 0xBF, 0x16, 0x61 }, 16);
	memcpy(appkey, (uint8_t [16]){ 0xF3, 0x1B, 0xF7, 0xD2, 0x55, 0x66, 0x1D, 0xED, 0x93, 0xA9, 0x9E, 0x19, 0xB7, 0x5F, 0x38, 0x31 }, 16);
	memcpy(devaddr, (uint8_t [4]){ 0x26, 0x01, 0x1B, 0x7E }, 4);
}

static void aesShiftLeft(uint8_t *data)
{
	uint8_t overflow = 0;
	for (uint8_t i = 0; i < 16; i++) {
		if (i < 15) {
			if ((data[i+1] & 0x80) == 0x80) {
				overflow = 1;
			} else {
				overflow = 0;
			}
		} else {
			overflow = 0;
		}
		data[i] = (data[i] << 1) + overflow;
	}
}

static void aesXor(uint8_t *newData, uint8_t *oldData)
{
	for (uint8_t i = 0; i < 16; i++) {
		newData[i] = newData[i] ^ oldData[i];
	}
}

static void aesAddRoundKey(uint8_t *roundKey, uint8_t (*state)[4])
{
	for (uint8_t collum = 0; collum < 4; collum++) {
		for (uint8_t row = 0; row < 4; row++) {
			state[row][collum] ^= roundKey[row + (collum << 2)];
		}
	}
}

static void aesShiftRows(uint8_t (*state)[4])
{
	uint8_t buffer;

	// Store firt byte in buffer
	buffer      = state[1][0];
	
	// Shift all bytes
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = buffer;

	buffer      = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = buffer;
	buffer      = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = buffer;

	buffer      = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = state[3][0];
	state[3][0] = buffer;
}

static void aesMixCollums(uint8_t (*state)[4])
{
	uint8_t row, collum;
	uint8_t a[4], b[4];

	for (collum = 0; collum < 4; collum++) {
		for (row = 0; row < 4; row++) {
			a[row] =  state[row][collum];
			b[row] = (state[row][collum] << 1);

			if ((state[row][collum] & 0x80) == 0x80) {
				b[row] ^= 0x1B;
			}
		}

		state[0][collum] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
		state[1][collum] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
		state[2][collum] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
		state[3][collum] = a[0] ^ b[0] ^ a[1] ^ a[2] ^ b[3];
	}
}

static uint8_t aesSubByte(uint8_t byte)
{
	return pgm_read_byte(&(s_table[((byte >> 4) & 0x0F)][((byte >> 0) & 0x0F)]));
}

static void aesCalculateRoundKey(uint8_t round, uint8_t *roundKey)
{
	uint8_t i, j, b, rcon;
	uint8_t tmp[4];

	rcon = 0x01;
	while (round-- != 1) {
		b = rcon & 0x80;
		rcon = rcon << 1;

		if (b == 0x80) {
			rcon ^= 0x1b;
		}
	}

	tmp[0] = aesSubByte( roundKey[12 + 1] );
	tmp[1] = aesSubByte( roundKey[12 + 2] );
	tmp[2] = aesSubByte( roundKey[12 + 3] );
	tmp[3] = aesSubByte( roundKey[12 + 0] );

	tmp[0] ^= rcon;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			roundKey[j + (i << 2)]  ^= tmp[j];
			tmp[j] = roundKey[j + (i << 2)];
		}
	}
}

static void aesEncrypt(uint8_t *data, uint8_t *key)
{
	uint8_t row, column, round = 0;
	uint8_t roundKey[16];
	uint8_t state[4][4];

	for (column = 0; column < 4; column++) {
		for (row = 0; row < 4; row++) {
			state[row][column] = data[row + (column << 2)];
		}
	}

	memcpy(&roundKey[0], &key[0], 16);
	aesAddRoundKey(roundKey, state);

	for (round = 1; round < 10; round++) {
		for(column = 0; column < 4; column++) {
			for(row = 0; row < 4; row++) {
				state[row][column] = aesSubByte(state[row][column]);
			}
		}

		aesShiftRows(state);
		aesMixCollums(state);
		aesCalculateRoundKey(round, roundKey);
		aesAddRoundKey(roundKey, state);
	}

	for (column = 0; column < 4; column++) {
		for (row = 0; row < 4; row++) {
			state[row][column] = aesSubByte(state[row][column]);
		}
	}

	aesShiftRows(state);
	aesCalculateRoundKey(round, roundKey);
	aesAddRoundKey(roundKey, state);

	for (column = 0; column < 4; column++) {
		for (row = 0; row < 4; row++) {
			data[row + (column << 2)] = state[row][column];
		}
	}
}

static void aesGeneratekeys(uint8_t *k1, uint8_t *k2)
{
	uint8_t msb = 0;

	aesEncrypt(k1, nwkkey);

	if ((k1[0] & 0x80) == 0x80) {
		msb = 1;
	} else {
		msb = 0;
	}

	aesShiftLeft(k1);

	if (msb) {
		k1[15] = k1[15] ^ 0x87;
	}

	for (uint8_t i = 0; i < 16; i++) {
		k2[i] = k1[i];
	}

	if ((k2[0] & 0x80) == 0x80) {
		msb = 1;
	}

	aesShiftLeft(k2);

	if (msb) {
		k2[15] = k2[15] ^ 0x87;
	}
}

void aesEncryptPayload(uint8_t *data, uint8_t len, uint16_t frameCounter, uint8_t direction)
{
	uint8_t i = 0;
	uint8_t j;
	uint8_t numberOfBlocks = 0;
	uint8_t incompleteBlockSize = 0;

	uint8_t block_A[16];

	numberOfBlocks = len / 16;
	incompleteBlockSize = len % 16;
	if (incompleteBlockSize != 0) {
		numberOfBlocks++;
	}

	for (i = 1; i <= numberOfBlocks; i++) {
		block_A[0] = 0x01;
		block_A[1] = 0x00;
		block_A[2] = 0x00;
		block_A[3] = 0x00;
		block_A[4] = 0x00;

		block_A[5] = direction;

		block_A[6] = devaddr[3];
		block_A[7] = devaddr[2];
		block_A[8] = devaddr[1];
		block_A[9] = devaddr[0];

		block_A[10] = (frameCounter & 0x00FF);
		block_A[11] = ((frameCounter >> 8) & 0x00FF);

		block_A[12] = 0x00;
		block_A[13] = 0x00;

		block_A[14] = 0x00;

		block_A[15] = i;

		aesEncrypt(block_A, appkey);

		if (i != numberOfBlocks) {
			for (j = 0; j < 16; j++) {
				*data = *data ^ block_A[j];
				data++;
			}
		} else {
			if (incompleteBlockSize == 0) {
				incompleteBlockSize = 16;
			}
			for (j = 0; j < incompleteBlockSize; j++) {
				*data = *data ^ block_A[j];
				data++;
			}
		}
	}
}

void aesCalculateMic(uint8_t *data, uint8_t *finalMIC, uint8_t len, uint16_t frameCounter, uint8_t direction)
{
	uint8_t i;
	uint8_t blockB[16];

	uint8_t keyk1[16] = {0};
	uint8_t keyK2[16] = {0};

	uint8_t oldData[16] = {0};
	uint8_t newData[16] = {0};

	uint8_t numberOfBlocks = 0x00;
	uint8_t incompleteBlockSize = 0x00;
	uint8_t blockCounter = 0x01;

	blockB[0] = 0x49;
	blockB[1] = 0x00;
	blockB[2] = 0x00;
	blockB[3] = 0x00;
	blockB[4] = 0x00;

	blockB[5] = direction;

	blockB[6] = devaddr[3];
	blockB[7] = devaddr[2];
	blockB[8] = devaddr[1];
	blockB[9] = devaddr[0];

	blockB[10] = (frameCounter & 0x00FF);
	blockB[11] = ((frameCounter >> 8) & 0x00FF);

	blockB[12] = 0x00;
	blockB[13] = 0x00;

	blockB[14] = 0x00;
	blockB[15] = len;

	numberOfBlocks = len / 16;
	incompleteBlockSize = len % 16;

	if (incompleteBlockSize != 0) {
		numberOfBlocks++;
	}

	aesGeneratekeys(keyk1, keyK2);
	aesEncrypt(blockB, nwkkey);

	for (i = 0; i < 16; i++) {
		oldData[i] = blockB[i];
	}

	while (blockCounter < numberOfBlocks) {
		for (i = 0; i < 16; i++) {
			newData[i] = *data;
			data++;
		}

		aesXor(newData, oldData);
		aesEncrypt(newData, nwkkey);

		for (i = 0; i < 16; i++) {
			oldData[i] = newData[i];
		}
		blockCounter++;
	}

	if (incompleteBlockSize == 0) {
		for (i = 0; i < 16; i++) {
			newData[i] = *data;
			data++;
		}

		aesXor(newData, keyk1);
		aesXor(newData, oldData);

		aesEncrypt(newData, nwkkey);
	} else {
		for (i =  0; i < 16; i++) {
			if (i < incompleteBlockSize) {
				newData[i] = *data;
				data++;
			}

			if(i == incompleteBlockSize) {
				newData[i] = 0x80;
			}

			if(i > incompleteBlockSize) {
				newData[i] = 0x00;
			}
		}

		aesXor(newData, keyK2);
		aesXor(newData, oldData);

		aesEncrypt(newData, nwkkey);
	}

	finalMIC[0] = newData[0];
	finalMIC[1] = newData[1];
	finalMIC[2] = newData[2];
	finalMIC[3] = newData[3];
}