#ifndef __MCP23X08__H__
#define __MCP23X08__H__

#include <avr/interrupt.h>

#include "spi.h"

#define MCP23X08_SLAVE_BASE_ADDRESS (1 << 6)
#define MCP23X08_REG_IODIR 0x00
#define MCP23X08_REG_GPIO 0x09

typedef struct {
    SpiDevice spi;
    uint8_t address;
} mcp23s08Device;

void MCP23S08_Send(mcp23s08Device *const dev, uint8_t opcode, uint8_t registerAddress, uint8_t data);

void MCP23S08_GpioWrite(mcp23s08Device *const dev, uint8_t data);

void MCP23S08_IodirWrite(mcp23s08Device *const dev, uint8_t dirs);

void MCP23S08_Init(int addressA0, int addressA1, mcp23s08Device *dev);

#endif // __MCP23X08__H__
