#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

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

// the loop routine runs over and over again forever:
void loop() {
  int v = val();
  int d = v - last; // abs(last - v);
  last = v;
  Serial.print(v);
  Serial.print(", ");
  Serial.println(d); 
  displayVal(v, d);
}
