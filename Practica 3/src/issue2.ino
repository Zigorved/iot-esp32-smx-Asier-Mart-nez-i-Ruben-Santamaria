#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

void enviarPaginaPrincipal() {
  server.send(200, "text/plain", "Hola");
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
