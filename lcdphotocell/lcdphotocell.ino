#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <avr/interrupt.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

int last = 0;

void setup() {
  Serial.begin(9600);

  display.begin();

  display.setContrast(38);
  display.setTextSize(2);
  display.setTextColor(BLACK);
  
  // See table 8-2 on datasheet
  WDTCR |= (1 << WDP2) | (1 << WDP0); // Sleep for ~30s
  // WDTCR |= (1 << WDP2); // Sleep for ~15s
  WDTCR |= (1 << WDTIE); // Enable watchdog timer

  sei();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

int val() {
  int v = 0;
  for (int i = 0; i < 8; i++) {
    v += analogRead(A0);
  }

  return (int)(v / 8);
}

void displayVal(int v, int d) {
  display.clearDisplay();
  display.setCursor(25,0);
  display.println(v, DEC);
  display.setCursor(35, 15);
  display.println(d, DEC);
  // display.setCursor(32, 30);
  // display.println(p, DEC);
  display.display();
  if (d < -20) {
    delay(2000);
  }
}

void doit() {
  int v = val();
  int d = v - last; // abs(last - v);
  last = v;
  Serial.print(v);
  Serial.print(", ");
  Serial.println(d); 
  displayVal(v, d);
}

ISR(WDT_vect) {
    ADCSRA |= (1 << ADEN);  // Enable ADC
    doit();
    ADCSRA &= ~(1 << ADEN);  // Disable ADC
}

// the loop routine runs over and over again forever:
void loop() {
  ADCSRA &= ~(1 << ADEN); // Disable ADC (to save power)
  sleep_mode();
  // doit();
}
