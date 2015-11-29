#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

typedef struct {
    int chipSelect;
    int serialClock;
    int serialDataInput;
} SpiDevice;

void SetChipSelectHigh(SpiDevice *const dev);
void SetChipSelectLow(SpiDevice *const dev);
void ToggleSerialClock(SpiDevice *const dev);
uint8_t SpiSendByte(SpiDevice *const dev, uint8_t data);
void SpiWriteBytes(SpiDevice *const dev, int numBytes, uint8_t data[]);
void Init3WireSpiDevice(SpiDevice *dev);

#endif //__SPI_H__
