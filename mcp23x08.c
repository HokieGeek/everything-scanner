#include "mcp23x08.h"

void MCP23S08_Send(mcp23x08Device *const dev, uint8_t opcode, uint8_t registerAddress, uint8_t data) {
    SetChipSelectLow(&dev->spi);

    SpiSendByte(&dev->spi, opcode);
    SpiSendByte(&dev->spi, registerAddress);
    SpiSendByte(&dev->spi, data);

    SetChipSelectHigh(&dev->spi);
}

void MCP23S08_GpioWrite(mcp23x08Device *const dev, uint8_t data) {
    MCP23S08_Send(dev, dev->address, MCP23X08_REG_GPIO, data);
}

void MCP23S08_IodirWrite(mcp23x08Device *const dev, uint8_t dirs) {
    MCP23S08_Send(dev, dev->address, MCP23X08_REG_IODIR, dirs);
}

void MCP23S08_Init(int chipSelect, int serialClock, int serialDataInput, int addressA0, int addressA1, mcp23x08Device *dev) {
    SpiDevice spi;
    Init3WireSpiDevice(chipSelect, serialClock, serialDataInput, &spi);

    dev->spi = spi;

    if (dev->spi.chipSelect != PB3) {
        OCR1A = 255;
    } else {
        OCR0A = 0;
    }
    dev->address = (MCP23X08_SLAVE_BASE_ADDRESS|(addressA1 << 2)|(addressA0 << 1));
}
