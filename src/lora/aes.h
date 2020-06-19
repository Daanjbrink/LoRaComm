#ifndef AES_H_
#define AES_H_

uint8_t nwkkey[16];
uint8_t appkey[16];
uint8_t devaddr[4];

void aesInit();
void aesEncryptPayload(uint8_t *data, uint8_t len, uint16_t frame_counter, uint8_t direction);
void aesCalculateMic(uint8_t *data, uint8_t *final_mic, uint8_t len, uint16_t frame_counter, uint8_t direction);

#endif