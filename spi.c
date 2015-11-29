#include "spi.h"

#include <avr/io.h>

inline void SetChipSelectHigh(SpiDevice *const dev) {
    PORTB |= (1<<dev->chipSelect);
}

inline void SetChipSelectLow(SpiDevice *const dev) {
    PORTB &= ~(1<<dev->chipSelect);
}

inline void ToggleSerialClock(SpiDevice *const dev) {
    PORTB |= (1<<dev->serialClock);
    PORTB &= ~(1<<dev->serialClock);
}

void SpiSendByte(SpiDevice *const dev, uint8_t data) {
    for (uint8_t bit = 7; bit >= 0; --bit) {
        if ((data & (1 << bit))) {
            PORTB |= (1 << dev->serialDataInput);
        } else {
            PORTB &= ~(1 << dev->serialDataInput);
        }
        ToggleSerialClock(dev);
    }
}

void SpiWriteBytes(SpiDevice *const dev, int numBytes, uint8_t data[]) {
    SetChipSelectLow(dev);
    for (int byte = 0; byte < numBytes; byte++) {
        SpiSendByte(dev, data[byte]);
    }
    SetChipSelectHigh(dev);
}

void Init3WireSpiDevice(SpiDevice *dev) {
    DDRB |= (1<<dev->chipSelect)|(1<<dev->serialClock)|(1<<dev->serialDataInput);

    SetChipSelectHigh(dev);
}

// While this is pretty great, the attiny85's registers are 8-bit so 
// I can't use it to set the MCP41010's 16-bit registers. Still need to do bit-banging.

/*
SpiDevice2 *const InitSpiMaster(int chipSelect) {
    const int sdsize = sizeof(SpiDevice2);
    SpiDevice2 *const dev = (SpiDevice2*)malloc(sdsize);
    dev->chipSelect = chipSelect;

    USICR &= ~((1<<USISIE)|(1<<USIOIE)|(1<<USIWM1));
    USICR |= (1<<USIWM0)|(1<<USICS1)|(1<<USICLK);

    // Set SCK and MOSI as output
    DDRB |= (1<<PB2)|(1<<PB0)|(1<<chipSelect);

    SetChipSelectHigh2(dev);

    return dev;
}

uint8_t spiSend(uint8_t data) {
    USIDR = data;
    USISR = (1<<USIOIF);

    asm volatile("nop");
    while (!(USISR & (1<<USIOIF))) USICR |= (1<<USITC);

    return USIDR;
}

uint8_t SpiSend(SpiDevice2 *const dev, uint8_t data) {
    SetChipSelectLow2(dev);

    uint8_t ret = spiSend(data);

    SetChipSelectHigh2(dev);

    return ret;
}
*/
