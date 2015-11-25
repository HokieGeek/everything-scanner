#include "mcp23x08.h"

// #include <stdlib.h>

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

/*
mcp23x08Device *const MCP23S08_Init(int chipSelect, int serialClock, int serialDataInput, int addressA0, int addressA1) {

    const int structSize = sizeof(mcp23x08Device);
    mcp23x08Device *const dev = (mcp23x08Device*)malloc(structSize);
    dev->spi = Init3WireSpiDevice(chipSelect, serialClock, serialDataInput);
    dev->address = (MCP23X08_SLAVE_BASE_ADDRESS|(addressA1 << 2)|(addressA0 << 1));

    return dev;
}
*/

void MCP23S08_Init(int chipSelect, int serialClock, int serialDataInput, int addressA0, int addressA1, mcp23x08Device *dev) {
    // dev.spi = Init3WireSpiDevice(chipSelect, serialClock, serialDataInput);
    Init3WireSpiDevice(chipSelect, serialClock, serialDataInput, &dev->spi);
    dev->address = (MCP23X08_SLAVE_BASE_ADDRESS|(addressA1 << 2)|(addressA0 << 1));
}
