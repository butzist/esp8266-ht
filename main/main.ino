#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Sodaq_SHT2x.h>

#ifndef STASSID
#define STASSID "XXX"
#define STAPSK  "XXX"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

void handleRoot() {
  digitalWrite(led, 0);
  server.send(200, "text/plain", "hello from esp8266!\r\n");
  digitalWrite(led, 1);
}

void handleHT() {
  digitalWrite(led, 0);

  String message = "{\n";
  message += "  \"humidity\": " + String(SHT2x.GetHumidity()) + ",\n";
  message += "  \"temperature\": " + String(SHT2x.GetTemperature()) + ",\n";
  message += "  \"dewpoint\": " + String(SHT2x.GetDewPoint()) + "\n";
  message += "}";

  server.send(200, "application/json", message);
  digitalWrite(led, 1);
}

void handleNotFound() {
  digitalWrite(led, 0);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 1);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Wire.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(led, 1);

  if (MDNS.begin("esp8266-ht")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ht", handleHT);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
