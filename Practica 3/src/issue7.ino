#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const int pinLDR = 35;
const int pinLM35 = 34;
const int pinLED = 25;  // LED

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

// ----------- PAGINA PRINCIPAL -----------
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
    .btn {
      margin-top: 15px;
      padding: 10px 15px;
      background: black;
      color: white;
      border: none;
      border-radius: 10px;
      cursor: pointer;
      font-size: 1rem;
    }
  </style>
</head>
<body>
  <h1>Control CPD</h1>
  <div class="card">
    <div class="value">💡 LDR: --LDR--</div>
    <div class="value">🌡️ Temperatura: --TEMP--</div>

    <a href="/credits">
      <button class="btn">Credits</button>
    </a>
  </div>
</body>
</html>
)rawliteral";

// ----------- PAGINA CREDITOS -----------
const char creditsPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Créditos</title>
</head>
<body style="background:#222; color:white; text-align:center; font-family:Arial;">
  <h1>Créditos</h1>
  <p>Autors: Asier Martínez y Ruben Santamaria</p>
  <a href="/" style="color:cyan;">⬅ Volver</a>
</body>
</html>
)rawliteral";

// ----------- PAGINA PRINCIPAL -----------
void enviarPaginaPrincipal() {

  int valorLDR = readAnalog(pinLDR);
  int valorTempRaw = readAnalog(pinLM35);
  float tempC = (valorTempRaw * 5.0 / 4095.0) * 100.0;

  // 🔴 CONTROL DEL LED
  if (valorLDR > 1000 || tempC > 22) {
    digitalWrite(pinLED, HIGH);
  } else {
    digitalWrite(pinLED, LOW);
  }

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

// ----------- CREDITOS -----------
void enviarCreditos() {
  server.send(200, "text/html", creditsPage);
}

void setup() {
  Serial.begin(115200);

  pinMode(pinLED, OUTPUT); // ✅ IMPORTANTE

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnectat!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", enviarPaginaPrincipal);
  server.on("/credits", enviarCreditos);

  server.begin();
}

void loop() {
  server.handleClient();
}
