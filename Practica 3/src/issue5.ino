#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const int pinLDR = 35;
const int pinLM35 = 34;

const char* ssid = "Zigorneta";
const char* password = "12345678";

int readAnalog(int pin) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / 10;
}

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="2">
  <title>Control CPD</title>
  <style>
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: #f23af3;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      flex-direction: column;
    }
    h1 {
      color: white;
      font-size: 2.5rem;
      text-align: center;
      -webkit-text-stroke: 2px black;
      text-shadow: 2px 2px black;
      margin-bottom: 20px;
    }
    .card {
      background: rgba(255,255,255,0.2);
      padding: 20px 30px;
      border-radius: 15px;
      border: 2px solid black;
      text-align: center;
      color: white;
      min-width: 220px;
    }
    .value {
      font-size: 1.3rem;
      margin: 10px 0;
    }
  </style>
</head>
<body>
  <h1>Control CPD</h1>
  <div class="card">
    <div class="value">💡 LDR: --LDR--</div>
    <div class="value">🌡️ Temperatura: --TEMP--</div>
  </div>
</body>
</html>
)rawliteral";

void enviarPaginaPrincipal() {

  int valorLDR = readAnalog(pinLDR);
  int valorTempRaw = readAnalog(pinLM35);
  float tempC = (valorTempRaw * 5.0 / 4095.0) * 100.0;

  String ldrTexto = String(valorLDR);
  String tempTexto = String(tempC, 1);

  if (valorLDR > 1000) {
    ldrTexto = "<span style='color:red'>" + ldrTexto + " ⚠️</span>";
  }

  tempTexto = tempTexto + " °C";

  if (tempC > 22) {
    tempTexto = "<span style='color:red'>" + tempTexto + " ⚠️</span>";
  }

  String html = htmlPage;
  html.replace("--LDR--", ldrTexto);
  html.replace("--TEMP--", tempTexto);

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnectat!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", enviarPaginaPrincipal);
  server.begin();
}

void loop() {
  server.handleClient();
}
