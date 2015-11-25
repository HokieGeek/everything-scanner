#include "mcp23x08.h"

void MCP23S08_Send(mcp23s08Device *const dev, uint8_t opcode, uint8_t registerAddress, uint8_t data) {
    SetChipSelectLow(&dev->spi);

    SpiSendByte(&dev->spi, opcode);
    SpiSendByte(&dev->spi, registerAddress);
    SpiSendByte(&dev->spi, data);

    SetChipSelectHigh(&dev->spi);
}

void MCP23S08_GpioWrite(mcp23s08Device *const dev, uint8_t data) {
    MCP23S08_Send(dev, dev->address, MCP23X08_REG_GPIO, data);
}

void MCP23S08_IodirWrite(mcp23s08Device *const dev, uint8_t dirs) {
    MCP23S08_Send(dev, dev->address, MCP23X08_REG_IODIR, dirs);
}

void MCP23S08_Init(int addressA0, int addressA1, mcp23s08Device *dev) {
    Init3WireSpiDevice(dev->spi);

    dev->address = (MCP23X08_SLAVE_BASE_ADDRESS|(addressA1 << 2)|(addressA0 << 1));
}
