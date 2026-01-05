#ifndef __DHT11_H
#define __DHT11_H

void DHT11_Start(void);
void DHT11_Init(void);
void DHT11_GetDataPack(uint8_t *Data);
uint8_t DHT11_GetRxFlag(void);
void DHT11_ClearRxFlag(void);
void DHT11_WaitRxFlag(void);
void DHT11_Reset(void);
uint8_t DHT11_GetData(float *Rh, float *Temp);

#endif
