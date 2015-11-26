// #define F_CPU 9600000UL

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "mcp23x08.h"

#define FALSE 0
#define TRUE !(FALSE)

#define VIBRATOR_PIN PB0

#define LEDS_PIN_DATA PB1
#define LEDS_PIN_SERIALCLOCK PB2
#define LEDS_PIN_CHIPSELECT PB3
#define MCP23X08_SLAVE_ADDRESS_A0 0
#define MCP23X08_SLAVE_ADDRESS_A1 0

#define PHOTOCELL_PIN PB4

// #define PHOTOCELL_ACTIVATE_THRESHOLD 600 // TODO: make this a diff from ambient?
#define VIBRATE_PULSE 255

int isTouching = FALSE;
int currentAmbient = 800;
mcp23s08Device mcp23s08;

inline void ledsWrite(uint8_t leds) {
    MCP23S08_GpioWrite(&mcp23s08, leds);
}

void ledPattern_Alternating() {
    for (int i = 0; i < 6; i++) {
    // TODO: while (isTouching) {
        ledsWrite(0b01101010);
        _delay_ms(500);
        ledsWrite(0b01010101);
        _delay_ms(500);
    }
}

void ledPattern_KITT() {
    int i = 0;
    uint8_t pattern = 0b00000000;
    #define numLeds 6
    for (int i = 0; i < 2; i++) {
    // TODO: while (isTouching) {
        for (; i < numLeds; i++) {
            pattern = (1 << i);
            ledsWrite((pattern|(1 << 6)));
            _delay_ms(100);
        }
        i -= 2;
        for (; i >= 0; i--) {
            pattern = (1 << i);
            ledsWrite((pattern|(1 << 6)));
            _delay_ms(100);
        }
    }
}

#define numLedPatterns 2
void (*ledPatterns[numLedPatterns])() = { ledPattern_Alternating, ledPattern_KITT };

void animateLeds(void) {
    // TODO: randomly select and apply an animation
    // (*ledPatterns[RAND_NUM from 0 to numLedPatterns-1])();
    (*ledPatterns[1])();
}

int read_photocell(void) {
    ADCSRA |= (1 << ADSC); // Start the conversion

    while (ADCSRA & (1 << ADSC)); // Wait for conversion

    return ADC;
}

inline void vibrate(int pulse) {
    OCR0A = pulse;
}

void analyze_and_activate(void) {
    // if (!isTouching) {
        // if (read_photocell() < PHOTOCELL_ACTIVATE_THRESHOLD) {
        // if (read_photocell() < currentAmbient) { // TODO: if < 5% of ambient
            // vibrate(VIBRATE_PULSE);
            // isTouching = TRUE;
            animateLeds();
        // } else { // Turn off all LEDs
            // isTouching = FALSE;
            // vibrate(0);
            // ledsWrite(0x00);
        // }
    // }
}

ISR(WDT_vect) {
    ADCSRA |= (1 << ADEN);  // Enable ADC
    analyze_and_activate();
    ADCSRA &= ~(1 << ADEN);  // Disable ADC
}

inline void init_mcp23s08() {
    mcp23s08.spi.chipSelect = LEDS_PIN_CHIPSELECT;
    mcp23s08.spi.serialClock = LEDS_PIN_SERIALCLOCK;
    mcp23s08.spi.serialDataInput = LEDS_PIN_DATA;

    MCP23S08_Init(MCP23X08_SLAVE_ADDRESS_A0, MCP23X08_SLAVE_ADDRESS_A1, &mcp23s08);
    MCP23S08_IodirWrite(&mcp23s08, 0x00); // Set all pins as output pins
    ledsWrite(0x00); // Start them off
}

inline void init_pins(void) {
    // Vibrator
    TCCR0B |= (1 << CS01); // clock/8 (See 11-9)
    TCCR0A |= (1 << WGM01) | (1 << WGM00); // Set for fast PWM with 0xFF Max (See 11-8)
    TCCR0A |= (1 << COM0A1); // Clear on compare match (See 11-5)

    DDRB |= (1 << VIBRATOR_PIN);

    // The LEDS
    init_mcp23s08();

    // The photocell ADC. Enable ADC2 / PB4 as an ADC pin
    ADMUX |= (0 << REFS0) | (1 << MUX1) | (0 << MUX0);
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN); // Enable ADC and set prescaler to clock/128
}

inline void init_interrupts(void) {
    // See table 8-2 on datasheet
    WDTCR |= (1 << WDP2) | (1 << WDP0); // Sleep for ~30s
    // WDTCR |= (1 << WDP2); // Sleep for ~15s
    WDTCR |= (1 << WDTIE) | (0 << WDE); // Enable watchdog timer
    WDTCR |= (0 << WDE);

    sei();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

int __attribute__((OS_main)) main(void) {
    init_pins();
    init_interrupts();

    ADCSRA &= ~(1 << ADEN);  // Disable ADC (to save power)

    currentAmbient = read_photocell();

    for (;;) {
        sleep_mode();
    }

    return 0;
}
