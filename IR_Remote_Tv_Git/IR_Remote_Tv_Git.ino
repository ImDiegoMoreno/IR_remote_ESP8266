
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

const char* ssid = "Your wifi";
const char* password = "*********";
MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head><title>ESP8266 Demo</title></head>" \
                "<body>" \
                  "<h1>Hello from ESP8266, you can send NEC encoded IR" \
                      "signals from here!</h1>" \
                  "<p><a href=\"ir?code=0xE0E040BF\">ON / OFF</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0F00F\">Mute</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0E01F\">Volume +</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0D02F\">Volume -</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0807F\">Source</a></p>" \
                  "<p><a href=\"ir?code=0xE0E048B7\">Channel +</a></p>" \
                  "<p><a href=\"ir?code=0xE0E008F7\">Channel -</a></p>" \
                  "<p><a href=\"ir?code=0xE0E058A7\">Menu</a></p>" \
                  "<p><a href=\"ir?code=0xE0E031CE\">Media P</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0F807\">info</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0D22D\">tools</a></p>" \
                  "<p><a href=\"ir?code=0xE0E006F9\">up</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0A659\">left</a></p>" \
                  "<p><a href=\"ir?code=0xE0E046B9\">right</a></p>" \
                  "<p><a href=\"ir?code=0xE0E08679\">down</a></p>" \
                  "<p><a href=\"ir?code=0xE0E016E9\">enter</a></p>" \
                  "<p><a href=\"ir?code=0xE0E01AE5\">return</a></p>" \
                  "<p><a href=\"ir?code=0xE0E0B44B\">exit</a></p>" \
                  "<p><a href=\"ir?code=0xE0E020DF\"> 1 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E0A05F\"> 2 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E0609F\"> 3 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E010EF\"> 4 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E0906F\"> 5 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E050AF\"> 6 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E030CF\"> 7 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E0B04F\"> 8 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E0708F\"> 9 </a></p>" \
                  "<p><a href=\"ir?code=0xE0E08877\"> 0 </a></p>" \
                "</body>" \
              "</html>");
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 16);
      irsend.sendSAMSUNG(code, 32,3);
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
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

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
