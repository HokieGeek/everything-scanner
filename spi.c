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

uint8_t SpiSendByte(SpiDevice *const dev, uint8_t data) {
#if defined(__AVR_ATtiny13__)
    for (int bit = 7; bit >= 0; --bit) { // MSB first
        if ((data & (1 << bit))) {
            PORTB |= (1 << dev->serialDataInput);
        } else {
            PORTB &= ~(1 << dev->serialDataInput);
        }
        ToggleSerialClock(dev);
    }
    return 0; // TODO
#else
    USIDR = data;
    USISR = (1<<USIOIF);

    asm volatile("nop");
    while (!(USISR & (1<<USIOIF))) USICR |= (1<<USITC);

    return USIDR;
#endif
}

void SpiWriteBytes(SpiDevice *const dev, int numBytes, uint8_t data[]) {
    SetChipSelectLow(dev);
    for (int byte = 0; byte < numBytes; byte++) {
        SpiSendByte(dev, data[byte]);
    }
    SetChipSelectHigh(dev);
}

void Init3WireSpiDevice(SpiDevice *dev) {
#if defined(__AVR_ATtiny13__)
    DDRB |= (1<<dev->chipSelect)|(1<<dev->serialClock)|(1<<dev->serialDataInput);
#else
    USICR &= ~((1<<USISIE)|(1<<USIOIE)|(1<<USIWM1));
    USICR |= (1<<USIWM0)|(1<<USICS1)|(1<<USICLK);
    DDRB |= (1<<dev->chipSelect);
#endif

    SetChipSelectHigh(dev);
}
