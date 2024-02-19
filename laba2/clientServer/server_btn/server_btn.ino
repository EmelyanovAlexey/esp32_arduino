#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "RT-GPON-004B";
const char* password = "nXJGH4V5";
const char* serverAddress = "http://192.168.0.66"; // IP адрес удаленной платы
const int buttonPin = 0; // Номер пина, к которому подключена кнопка (GPIO0)

void setup() {
  pinMode(buttonPin, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    sendRequest();
    delay(100); // Для устранения дребезга кнопки
  }
}

void sendRequest() {
  WiFiClient client;
  HTTPClient http;
  
  if (http.begin(client, serverAddress)) {  // Начинаем HTTP соединение
    int httpCode = http.GET();      // Выполняем GET запрос
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();  // Завершаем соединение
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }
}