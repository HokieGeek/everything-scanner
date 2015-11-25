#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

// typedef enum {
//     3WIRE,
//     5WIRE
// } SpiDeviceTypeEnum;

typedef struct {
    int chipSelect;
    int serialClock;
    int serialDataInput;
} SpiDevice;

void SetChipSelectHigh(SpiDevice *const dev);
void SetChipSelectLow(SpiDevice *const dev);
void ToggleSerialClock(SpiDevice *const dev);
void SpiSendByte(SpiDevice *const dev, uint8_t data);
void SpiWriteBytes(SpiDevice *const dev, int numBytes, uint8_t data[]);
void Init3WireSpiDevice(int chipSelect, int serialClock, int serialDataInput, SpiDevice *dev);

/*
typedef struct {
    int chipSelect;
    // TODO: is master
} SpiDevice2;

void SetChipSelectHigh2(SpiDevice2 *const dev);
void SetChipSelectLow2(SpiDevice2 *const dev);

SpiDevice2 *const InitSpiMaster(int chipSelect);
uint8_t SpiSend(SpiDevice2 *const dev, uint8_t data);
uint8_t SpiSend16(SpiDevice2 *const dev, uint8_t byte1, uint8_t byte2);
*/

#endif //__SPI_H__
