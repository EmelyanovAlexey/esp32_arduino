#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
// готовый вариант общения плат друг с другом по нопке в самой плате

const char* ssid = "RT-GPON-004B";
const char* password = "nXJGH4V5";
const char* serverAddress = "http://192.168.0.66"; // IP адрес удаленной платы 1
// const char* serverAddress = "http://192.168.0.67"; // IP адрес удаленной платы 2
const int buttonPin = 0; // Номер пина, к которому подключена кнопка (GPIO0)
const int ledPin = 2; 

WiFiServer server(80);

void setup() {
  pinMode(buttonPin, INPUT);
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
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    sendRequest();
    delay(100); // Для устранения дребезга кнопки
  }

  // ------
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

// функция отправки запроса
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