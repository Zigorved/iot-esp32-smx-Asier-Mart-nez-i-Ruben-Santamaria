#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
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
    }

    h1 {
      color: white;
      font-size: 2.5rem;
      text-align: center;

      /* Contorno negro */
      -webkit-text-stroke: 2px black;
      text-shadow:
        2px 2px 0 black,
        -2px 2px 0 black,
        2px -2px 0 black,
        -2px -2px 0 black;
    }
  </style>
</head>

<body>
  <h1>Control CPD</h1>
</body>
</html>
)rawliteral";

void enviarPaginaPrincipal() {
  server.send(200, "text/html", htmlPage);
}


const char* ssid = "Zigorneta";
const char* password = "12345678";


void setup() {
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connectant...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("\n Connectat!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", enviarPaginaPrincipal);
  server.begin();

  
}

void loop() {

  server.handleClient();
  
}
