#include <Arduino.h>
#include <ESP32Servo.h>

const int pinLM35 = 34;
const int pinLED = 25;
const int pinServo = 17;

Servo servo;
int posicion = 0;
int direccion = 1;

unsigned long ultimoMovimiento = 0;
unsigned long ultimoLED = 0;
bool estadoLED = false;

const int intervaloMs = 1;
const int gradosPorPaso = 1;

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
  servo.attach(pinServo);
}

void loop() {
  int valorTempRaw = readAnalog(pinLM35);
  float tempC = (valorTempRaw * 5.0 / 4095.0) * 100.0;

  unsigned long ahora = millis();

  if (tempC > 23) {
    // Parpadeo LED sin delay
    if (ahora - ultimoLED >= 500) {
      ultimoLED = ahora;
      estadoLED = !estadoLED;
      digitalWrite(pinLED, estadoLED);
    }

    // Movimiento servo sin delay
    if (ahora - ultimoMovimiento >= intervaloMs) {
      ultimoMovimiento = ahora;
      posicion += direccion * gradosPorPaso;
      if (posicion >= 180) direccion = -1;
      if (posicion <= 0)   direccion = 1;
      servo.write(posicion);
    }
  } else {
    digitalWrite(pinLED, LOW);
    estadoLED = false;
    servo.write(90);
  }
}
