#include <avr/interrupt.h>
#include <avr/sleep.h>

// #include "mcp23x08.h"
// #include "led-patterns.h"

#define FALSE 0
#define TRUE !(FALSE)

#define VIBRATOR_PIN PB0

#define LEDS_PIN_DATA PB1
#define LEDS_PIN_SERIALCLOCK PB2
#define LEDS_PIN_CHIPSELECT PB3

#define PHOTOCELL_PIN PB4

// #define PHOTOCELL_ACTIVATE_THRESHOLD 600 // TODO: make this a diff from ambient?
// #define PHOTOCELL_ACTIVATE_THRESHOLD 200 // TODO: make this a diff from ambient?
#define VIBRATE_PULSE 255

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

int isAnimated = FALSE;
int currentAmbient = 800;

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
    MCP23S08_Send(MCP23S08_OPCODE_WRITE, MCP23X08_REG_GPIO, data);
}

inline void MCP23S08_IodirWrite(uint8_t dirs) {
    MCP23S08_Send(MCP23S08_OPCODE_WRITE, MCP23X08_REG_IODIR, dirs);
}

inline void MCP23X08_Init(void) {
    SpiMasterInit();
}

inline void ledsWrite(uint8_t leds) {
    MCP23S08_GpioWrite(/*TODO blah, */leds);
}

int read_photocell(void) {
    ADCSRA |= (1 << ADSC); // Start the conversion

    while (ADCSRA & (1 << ADSC)); // Wait for conversion

    return ADC;
}

inline void vibrate(int pulse) {
    OCR0A = pulse;
}

void animateLeds(void) {
    isAnimated = TRUE;
    // TODO: randomly select and apply an animation
    ledsWrite(0xFF); // Once the animation ends, turn them all on?
    isAnimated = FALSE;
}

void analyze_and_activate(void) {
    // TODO: If animation is running, do not allow

    if (!isAnimated) {
        // if (read_photocell() < PHOTOCELL_ACTIVATE_THRESHOLD) {
        if (read_photocell() < currentAmbient) { // TODO: if < 5% of ambient
            // vibrate(VIBRATE_PULSE);
            animateLeds();
        } else { // Turn off all LEDs
            // vibrate(0);
            ledsWrite(0x00);
        }
    }
}

ISR(WDT_vect) {
    ADCSRA |= (1 << ADEN);  // Enable ADC
    analyze_and_activate();
    ADCSRA &= ~(1 << ADEN);  // Disable ADC
}

inline void init_pins(void) {
    // Vibrator
    TCCR0B |= (1 << CS01); // clock/8 (See 11-9)
    TCCR0A |= (1 << WGM01) | (1 << WGM00); // Set for fast PWM with 0xFF Max (See 11-8)
    TCCR0A |= (1 << COM0A1); // Clear on compare match (See 11-5)

    DDRB |= (1 << VIBRATOR_PIN);

    // The LEDS
    // DDRB |= (1 << LEDS_PIN_DATA);
    MCP23X08_Init();
    MCP23S08_IodirWrite(0x00); // Set all pins as output pins
    // MCP23S08_Send(MCP23S08_OPCODE_WRITE, MCP23X08_REG_IODIR, 0xFF); // Set all pins as output pins
    // MCP23S08_GpioWrite(0xFF);
    ledsWrite(0x00); // Start them off

    // The photocell ADC. Enable ADC2 / PB4 as an ADC pin
    ADMUX |= (0 << REFS0) | (1 << MUX1) | (0 << MUX0);
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN); // Enable ADC and set prescaler to clock/128
}

inline void init_interrupts(void) {
    // cli(); // JIC

    // See table 8-2 on datasheet
    WDTCR |= (1 << WDP2) | (1 << WDP0); // Sleep for ~30s
    // WDTCR |= (1 << WDP2); // Sleep for ~15s
    WDTCR |= (1 << WDTIE) | (0 << WDE); // Enable watchdog timer
    WDTCR |= (0 << WDE);

    sei();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // sleep_enable();
}

// #include <util/delay.h>
int __attribute__((OS_main)) main(void) {
    init_pins();
    init_interrupts();

    ADCSRA &= ~(1 << ADEN);  // Disable ADC (to save power)

    currentAmbient = read_photocell();

    for (;;) {
        sleep_mode();
    }
}
