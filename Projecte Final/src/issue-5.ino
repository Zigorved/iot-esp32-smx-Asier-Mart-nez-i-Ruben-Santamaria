#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid     = "Zigorneta";
const char* password = "12345678";

WebServer server(80);

const int pinLM35  = 34;
const int pinLED   = 25;
const int pinServo = 17;

Servo servo;
int posicion  = 0;
int direccion = 1;

unsigned long ultimoMovimiento = 0;
unsigned long ultimoLED        = 0;
unsigned long ultimoDisplay    = 0;
bool estadoLED = false;

float tempCGlobal = 0.0;

// ------------------------------------------------
int readAnalog(int pin) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / 10;
}

// ------------------------------------------------
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="2">
  <title>Monitor Temperatura</title>
  <style>
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: #1a1a2e;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      flex-direction: column;
    }
    h1 {
      color: white;
      font-size: 2rem;
      margin-bottom: 20px;
    }
    .card {
      background: rgba(255,255,255,0.1);
      padding: 30px 40px;
      border-radius: 20px;
      border: 2px solid rgba(255,255,255,0.3);
      text-align: center;
      color: white;
      min-width: 250px;
    }
    .temp {
      font-size: 3rem;
      font-weight: bold;
      margin: 10px 0;
    }
    .alerta {
      color: #ff4444;
      font-size: 1.1rem;
      font-weight: bold;
      margin-top: 10px;
    }
    .ok {
      color: #44ff88;
      font-size: 1.1rem;
      margin-top: 10px;
    }
  </style>
</head>
<body>
  <h1>🌡️ Monitor de Temperatura</h1>
  <div class="card">
    <div>Temperatura actual:</div>
    <div class="temp">--TEMP-- °C</div>
    <div class="--ESTADO_CLASS--">--ESTADO_MSG--</div>
  </div>
</body>
</html>
)rawliteral";

// ------------------------------------------------
void enviarPagina() {
  String tempTexto = String(tempCGlobal, 1);

  String estadoClass, estadoMsg;
  if (tempCGlobal > 25) {
    estadoClass = "alerta";
    estadoMsg   = "⚠️ ALERTA: Temperatura alta";
  } else {
    estadoClass = "ok";
    estadoMsg   = "✅ Temperatura normal";
  }

  String html = htmlPage;
  html.replace("--TEMP--",        tempTexto);
  html.replace("--ESTADO_CLASS--", estadoClass);
  html.replace("--ESTADO_MSG--",  estadoMsg);

  server.send(200, "text/html", html);
}

// ------------------------------------------------
void setup() {
  Serial.begin(115200);

  pinMode(pinLED, OUTPUT);
  servo.attach(pinServo);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  // Conectar WiFi
  WiFi.begin(ssid, password);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Conectando WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnectat!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Mostrar IP en pantalla
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("WiFi OK!");
  display.setCursor(0, 16);
  display.print(WiFi.localIP());
  display.display();
  delay(2000);

  server.on("/", enviarPagina);
  server.begin();
}

// ------------------------------------------------
void loop() {
  server.handleClient();

  int valorTempRaw = readAnalog(pinLM35);
  tempCGlobal = (valorTempRaw * 5.0 / 4095.0) * 100.0;

  unsigned long ahora = millis();

  // --- OLED ---
  if (ahora - ultimoDisplay >= 500) {
    ultimoDisplay = ahora;
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temperatura:");
    display.setTextSize(3);
    display.setCursor(0, 20);
    display.print(tempCGlobal, 1);
    display.print(" C");
    if (tempCGlobal > 25) {
      display.setTextSize(1);
      display.setCursor(0, 55);
      display.print("ALERTA TEMPERATURA");
    }
    display.display();
  }

  // --- LED y Servo (solo si temp > 25) ---
  if (tempCGlobal > 25) {
    if (ahora - ultimoLED >= 500) {
      ultimoLED  = ahora;
      estadoLED  = !estadoLED;
      digitalWrite(pinLED, estadoLED);
    }
    if (ahora - ultimoMovimiento >= 1) {
      ultimoMovimiento = ahora;
      posicion += direccion;
      if (posicion >= 180) direccion = -1;
      if (posicion <= 0)   direccion =  1;
      servo.write(posicion);
    }
  } else {
    digitalWrite(pinLED, LOW);
    estadoLED = false;
    servo.write(90);
  }
}
