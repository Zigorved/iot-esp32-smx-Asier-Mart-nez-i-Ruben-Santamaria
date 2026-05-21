#include <Arduino.h>

const int pinLM35 = 34;
const int pinLED = 25;

int readAnalog(int pin) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / 10;
}

void setup() {
  pinMode(pinLED, OUTPUT);
}

void loop() {
  int valorTempRaw = readAnalog(pinLM35);
  float tempC = (valorTempRaw * 5.0 / 4095.0) * 100.0;

  if (tempC > 24) {
    digitalWrite(pinLED, HIGH);
    delay(500);
    digitalWrite(pinLED, LOW);
    delay(500);
  } else {
    digitalWrite(pinLED, LOW);
  }
}
