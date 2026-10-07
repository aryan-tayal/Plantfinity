#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Wire.h>

#define RXD2 16
#define TXD2 17
// Replace with your network credentials
const char* ssid = "###";
const char* password = "###";

String g, s, t, h, err;

const byte soilHumidity[] = {60, 80};
const byte temperature[] = {15, 30};
const byte humidity[] = {60, 80};

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);



String getError() {
  String errorMsg = ",";
  if (s.toFloat() < soilHumidity[0]) {
    errorMsg = "Soil Humidity Too Low,";
  }
  else if (s.toFloat() > soilHumidity[1]) {
    errorMsg = "Soil Humidity Too High,";
  }
  else {
    errorMsg = "All Good";
  }
  if (h.toFloat() < humidity[0]) {
    if (errorMsg == "All Good") {
      errorMsg = "Humidity Too Low,";
    } else {
      errorMsg += "Humidity Too Low,";
    }
  }
  else if (h.toFloat() > humidity[1]) {
    if (errorMsg == "All Good") {
      errorMsg = "Humidity Too Low,";
    } else {
      errorMsg += "Humidity Too High,";
    }
  }
  else {
    if (errorMsg == "") {
      errorMsg = "All Good";
    }
  }
  if (t.toFloat() < temperature[0]) {
    if (errorMsg == "All Good") {
      errorMsg = "Temperature Too Low,";
    } else {
      errorMsg += "Temperature Too Low,";
    }
  }
  else if (t.toFloat() > temperature[1]) {
    if (errorMsg == "All Good") {
      errorMsg = "Temperature Too Low,";
    } else {
      errorMsg += "Temperature Too High,";
    }
  }
  else {
    if (errorMsg == "") {
      errorMsg = "All Good";
    }
  }
  return errorMsg;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  Serial.print("C");
  WiFi.mode(WIFI_STA);
  Serial.print("B");
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println("Connected to WIFI");

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js", "text/js");

  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", t.c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", h.c_str());
  });
  server.on("/growth", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", g.c_str());
  });
  server.on("/soilhumidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", s.c_str());
  });
  server.on("/err", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getError().c_str());
  });
  // Start server
  server.begin();
}


void loop() {
  String receivedData = Serial2.readString();
  String value = receivedData;
  char first = receivedData.charAt(0);
  value.remove(0, 1);
  if (isDigit(value.charAt(2))) {
    value.remove(3, value.length() - 3);
  } else if (isDigit(value.charAt(1))) {
    value.remove(2, value.length() - 2);
  } else if (isDigit(value.charAt(0))) {
    value.remove(1, value.length() - 1);
  }
  if (first == 's') {
    Serial.print("Soil Humidity : ");
    s = value;
  } else if (first == 'g') {
    Serial.print("Plant Growth : ");
    g = value;
  } else if (first == 'h') {
    Serial.print("Humidity : ");
    h = value;
  } else if (first == 't') {
    Serial.print("Temperature : ");
    t = value;
  }
  Serial.println(value);
  delay(2350);
}
