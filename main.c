#include <avr/interrupt.h>
#include <avr/sleep.h>

// #include "led-patterns.h"

#define VIBRATOR_PIN PB0

#define LEDS_PIN_DATA PB1
#define LEDS_PIN_SERIALCLOCK PB2
#define LEDS_PIN_CHIPSELECT PB3

#define PHOTOCELL_PIN PB4

#define PHOTOCELL_ACTIVATE_THRESHOLD 600 // TODO: make this a diff from ambient?
// #define VIBRATE_PULSE 50

inline void SetChipSelectHigh() {
    PORTB |= (1<<LEDS_PIN_CHIPSELECT);
}

inline void SetChipSelectLow() {
    PORTB &= ~(1<<LEDS_PIN_CHIPSELECT);
}

inline void ToggleSerialClock() {
    PORTB |= (1<<LEDS_PIN_SERIALCLOCK);
    PORTB &= ~(1<<LEDS_PIN_SERIALCLOCK);
}

void SpiWriteBytes(int numBytes, uint8_t data) {
    SetChipSelectLow();
    for (int bit = 7; bit >= 0; --bit) {
        if ((data & (1 << bit))) {
            PORTB |= (1 << LEDS_PIN_DATA);
        } else {
            PORTB &= ~(1 << LEDS_PIN_DATA);
        }
        ToggleSerialClock();
    }
    SetChipSelectHigh();
}

ISR(WDT_vect) {
    // Nothing to do. Just wake up
    ADCSRA |= (1 << ADEN);  // Enable ADC
    analyze_and_activate();
    ADCSRA &= ~(1 << ADEN);  // Disable ADC
}

int read_photocell() {
    ADCSRA |= (1 << ADSC); // Start the conversion

    while (ADCSRA & (1 << ADSC)); // Wait for conversion

    return ADC;
}

void vibrate(int pulse) {
    OCR0A = pulse;
}

void ledsOff() {
    // TODO: this is an spi write operation
    PORTB &= ~(1 << LEDS_PIN_DATA);
}

void animateLeds() {
    // TODO: randomly select and apply an animation
    // TODO: this is an spi write operation
    PORTB |= (1 << LEDS_PIN_DATA);
}

void analyze_and_activate() {
    if (read_photocell() < PHOTOCELL_ACTIVATE_THRESHOLD) {
        // TODO: vibrate(VIBRATE_PULSE);
        vibrate(255);
        animateLeds();
    } else {
        // TODO: Turn off all leds
        ledsOff();
    }
}

void init_pins() {
    // Vibrator
    TCCR0B |= (1 << CS01); // clock/8 (See 11-9)
    TCCR0A |= (1 << WGM01) | (1 << WGM00); // Set for fast PWM with 0xFF Max (See 11-8)
    TCCR0A |= (1 << COM0A1); // Clear on compare match (See 11-5)

    DDRB |= (1 << VIBRATOR_PIN);

    // The LEDS
    DDRB |= (1 << LEDS_PIN_DATA);
    // DDRB |= (1<<LEDS_PIN_CHIPSELECT)|(1<<LEDS_PIN_SERIALCLOCK)|(1<<LEDS_PIN_DATA);
    // SetChipSelectHigh();

    // The photocell ADC. Enable ADC2 / PB4 as an ADC pin
    ADMUX |= (0 << REFS0) | (1 << MUX1) | (0 << MUX0);
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN); // Enable ADC and set prescaler to clock/128
}

void init_interrupts() {
    cli(); // JIC

    // See table 8-2 on datasheet
    // WDTCR |= (1 << WDP2) | (1 << WDP1) | (1 << WDP0); // Sleep for ~2s
    // WDTCR |= (1 << WDP2) | (1 << WDP1); // Sleep for ~1s
    WDTCR |= (1 << WDP2) | (1 << WDP0); // Sleep for ~30s
    // WDTCR |= (1 << WDP2); // Sleep for ~15s
    WDTCR |= (1 << WDTIE) | (0 << WDE); // Enable watchdog timer
    WDTCR |= (0 << WDE);

    sei();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
}

int main(void) {
    init_pins();
    init_interrupts();

    for (;;) {
        ADCSRA &= ~(1 << ADEN);  // Disable ADC
        sleep_mode();
    }
}
