#include "mcp23x08.h"

// TODO: ARGH
#define LEDS_PIN_DATA PB1
#define LEDS_PIN_SERIALCLOCK PB2
#define LEDS_PIN_CHIPSELECT PB3

inline void SetChipSelectHigh(void) {
    PORTB |= (1<<LEDS_PIN_CHIPSELECT);
}

inline void SetChipSelectLow(void) {
    PORTB &= ~(1<<LEDS_PIN_CHIPSELECT);
}

inline void ToggleSerialClock(void) {
    PORTB |= (1<<LEDS_PIN_SERIALCLOCK);
    PORTB &= ~(1<<LEDS_PIN_SERIALCLOCK);
}

void SpiSendByte(uint8_t data) {
    for (int bit =7; bit >= 0; --bit) {
        if ((data & (1 << bit))) {
            PORTB |= (1 << LEDS_PIN_DATA);
        } else {
            PORTB &= ~(1 << LEDS_PIN_DATA);
        }
        ToggleSerialClock();
    }
}

void SpiMasterInit(void) {
    DDRB |= (1<<LEDS_PIN_CHIPSELECT)|(1<<LEDS_PIN_SERIALCLOCK)|(1<<LEDS_PIN_DATA);
    SetChipSelectHigh();
}

void MCP23S08_Send(uint8_t opcode, uint8_t registerAddress, uint8_t data) {
    SetChipSelectLow();

    SpiSendByte(opcode);
    SpiSendByte(registerAddress);
    SpiSendByte(data);

    SetChipSelectHigh();
}

inline void MCP23S08_GpioWrite(uint8_t data) {
    // MCP23S08_Send(MCP23S08_OPCODE_WRITE, MCP23X08_REG_GPIO, data);
}

inline void MCP23S08_IodirWrite(uint8_t dirs) {
    // MCP23S08_Send(MCP23S08_OPCODE_WRITE, MCP23X08_REG_IODIR, dirs);
}

void MCP23X08_Init(void) {
    SpiMasterInit();
}
