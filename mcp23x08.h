#ifndef __MCP23X08__H__
#define __MCP23X08__H__

#include <avr/interrupt.h>

#define MCP23X08_SLAVE_BASE_ADDRESS (1 << 6)
#define MCP23X08_SLAVE_ADDRESS_A0 0
#define MCP23X08_SLAVE_ADDRESS_A1 0
#define MCP23008_SLAVE_ADDRESS_A2 0
#define MCP23S08_ADDRESS (MCP23X08_SLAVE_BASE_ADDRESS|(MCP23X08_SLAVE_ADDRESS_A1 << 2)|(MCP23X08_SLAVE_ADDRESS_A0 << 1))
#define MCP23008_ADDRESS (MCP23X08_SLAVE_BASE_ADDRESS|(MCP23008_SLAVE_ADDRESS_A2 << 3)|(MCP23X08_SLAVE_ADDRESS_A1 << 2)|(MCP23X08_SLAVE_ADDRESS_A0 << 1))
#define MCP23S08_OPCODE_WRITE MCP23S08_ADDRESS
#define MCP23S08_OPCODE_READ (MCP23S08_ADDRESS|1)
#define MCP23008_OPCODE_WRITE MCP23008_ADDRESS
#define MCP23008_OPCODE_READ (MCP23008_ADDRESS|1)
#define MCP23X08_REG_IODIR 0x00
#define MCP23X08_REG_GPIO 0x09

inline void SetChipSelectHigh(void);

inline void SetChipSelectLow(void);

inline void ToggleSerialClock(void);

void SpiSendByte(uint8_t data);

void SpiMasterInit(void);

void MCP23S08_Send(uint8_t opcode, uint8_t registerAddress, uint8_t data);

inline void MCP23S08_GpioWrite(uint8_t data);

inline void MCP23S08_IodirWrite(uint8_t dirs);

void MCP23X08_Init(void);

#endif // __MCP23X08__H__
