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

#define PHOTOCELL_ACTIVATE_THRESHOLD 200 // TODO: make this a diff from ambient?
#define VIBRATE_PULSE 255

#define NUM_LEDS 6
#define ANIMATION_REPETITION 8

int isAnimating = FALSE;
int currentAmbient = 800;
mcp23s08Device mcp23s08;

inline void ledsWrite(uint8_t leds) {
    MCP23S08_GpioWrite(&mcp23s08, leds);
}

void ledPattern_Alternating() {
    ledsWrite(0b01101010);
    _delay_ms(500);
    ledsWrite(0b01010101);
    _delay_ms(500);
}

void ledPattern_KITT() {
    int i = 0;
    uint8_t pattern = 0;
    for (; i < NUM_LEDS; i++) {
        pattern = (1 << i);
        ledsWrite((pattern|(1 << NUM_LEDS)));
        _delay_ms(100);
    }
    _delay_ms(50);
    i -= 2;
    for (; i >= 0; i--) {
        pattern = (1 << i);
        ledsWrite((pattern|(1 << NUM_LEDS)));
        _delay_ms(100);
    }
}

void ledPattern_LandingStrip() {
    int i = 0;
    uint8_t pattern = 0;
    for (; i < NUM_LEDS; i++) {
        pattern |= (1 << i);
        ledsWrite((pattern|(1 << NUM_LEDS)));
        _delay_ms(200);
    }
    ledsWrite(0b01000000);
    _delay_ms(200);
}

void ledPattern_Blinky() {
    ledsWrite(0b01111111);
    _delay_ms(500);
    ledsWrite(0b01000000);
    _delay_ms(500);
}

#define NUM_LED_PATTERNS 4
void (*ledPatterns[NUM_LED_PATTERNS])() = { ledPattern_Alternating, ledPattern_KITT,
                                            ledPattern_LandingStrip, ledPattern_Blinky };

int readPhotocell(void) {
    ADCSRA |= (1 << ADSC); // Start the conversion

    while (ADCSRA & (1 << ADSC)); // Wait for conversion

    return ADC;
}

void animateLeds(void) {
    // TODO: randomly select and apply an animation
    // int rand = 3;
    int rand = readPhotocell() % (NUM_LED_PATTERNS-1);
    isAnimating = TRUE;
    // for (int i = 0; i < ANIMATION_REPETITION && isAnimating; i++) {
        (*ledPatterns[rand])();
    // }
}

inline void vibrate(int pulse, int duration) {
    // TODO: only want to do this for a small amount of time and then stop
    OCR0A = pulse;
}

inline int isTouching() {
    // TODO: better sampling
    if (readPhotocell() < PHOTOCELL_ACTIVATE_THRESHOLD) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void analyze_and_activate(void) {
    if (isTouching()) {
        // if (isAnimating) {
            // isAnimating = FALSE;
        // }
        // vibrate(VIBRATE_PULSE, __duration__);
        animateLeds();
    } else { // Turn off all LEDs
    //     isAnimating = FALSE;
        ledsWrite(0x00);
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

    // LEDS
    mcp23s08.spi.chipSelect = LEDS_PIN_CHIPSELECT;
    mcp23s08.spi.serialClock = LEDS_PIN_SERIALCLOCK;
    mcp23s08.spi.serialDataInput = LEDS_PIN_DATA;

    MCP23S08_Init(MCP23X08_SLAVE_ADDRESS_A0, MCP23X08_SLAVE_ADDRESS_A1, &mcp23s08);
    MCP23S08_IodirWrite(&mcp23s08, 0x00); // Set all pins as output pins
    ledsWrite(0x00); // Start them off

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

    currentAmbient = readPhotocell();

    // vibrate(255, 0);
    // _delay_ms(1000);
    // vibrate(0, 0);

    for (;;) {
        sleep_mode();
    }

    return 0;
}
