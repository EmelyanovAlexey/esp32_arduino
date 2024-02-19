/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

const char* serverName = "http://192.168.0.67";
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "RT-GPON-004B";
const char* password = "nXJGH4V5";
const int ledPin = 2;    // Номер пина, к которому подключен светодиод (GPIO2)

WiFiServer server(80);

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    while (client.connected()) {
      if (client.available()) {
        client.readStringUntil('\r'); // Очищаем входной буфер
        digitalWrite(ledPin, !digitalRead(ledPin)); // Инвертируем состояние светодиода
        delay(100); // Для устранения дребезга кнопки
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<head></head><body>");
        client.println("<h1>LED Toggled</h1>");
        client.println("</body></html>");
        break;
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}