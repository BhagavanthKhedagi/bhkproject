#include <EEPROM.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);  // RS, E, D4, D5, D6, D7

long duration, inches;
int set_val, percentage;
bool state, pump;

void setup() {
  lcd.begin(16, 2);
  lcd.print("WATER LEVEL:");
  lcd.setCursor(0, 1);
  lcd.print("PUMP:OFF MANUAL");

  pinMode(8, OUTPUT);      // Trig
  pinMode(9, INPUT);       // Echo
  pinMode(10, INPUT_PULLUP); // SET Button
  pinMode(11, INPUT_PULLUP); // MODE Button
  pinMode(12, OUTPUT);     // Relay Output

  set_val = EEPROM.read(0);   // Read max depth
  if (set_val > 150) set_val = 150;
}

void loop() {
  digitalWrite(8, LOW);
  delayMicroseconds(2);
  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);
  duration = pulseIn(9, HIGH);
  inches = microsecondsToInches(duration);

  percentage = (set_val - inches) * 100 / set_val;
  lcd.setCursor(12, 0);
  if (percentage < 0) percentage = 0;
  lcd.print(percentage);
  lcd.print("% ");

  // Pump ON logic
  if (percentage < 30 && digitalRead(11)) pump = 1;
  if (percentage > 99) pump = 0;
  digitalWrite(12, pump);

  // Display Pump State
  lcd.setCursor(5, 1);
  if (pump == 1) lcd.print("ON ");
  else lcd.print("OFF");

  // Mode display
  lcd.setCursor(9, 1);
  if (!digitalRead(11)) lcd.print("MANUAL");
  else lcd.print("AUTO  ");

  // Set level button logic
  if (!digitalRead(10) && !state && digitalRead(11)) {
    state = 1;
    set_val = inches;
    EEPROM.write(0, set_val);
  }

  // Manual pump toggle button
  if (!digitalRead(10) && !state && !digitalRead(11)) {
    state = 1;
    pump = !pump;
  }

  if (digitalRead(10)) state = 0;

  delay(500);
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}
