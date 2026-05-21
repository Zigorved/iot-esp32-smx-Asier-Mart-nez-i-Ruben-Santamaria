#include <Arduino.h>

const int pinLM35 = 34;

int readAnalog(int pin) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / 10;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  int valorTempRaw = readAnalog(pinLM35);
  float tempC = (valorTempRaw * 5.0 / 4095.0) * 100.0;
  Serial.print("Temperatura: ");
  Serial.print(tempC, 1);
  Serial.println(" °C");
  delay(1000);
}
