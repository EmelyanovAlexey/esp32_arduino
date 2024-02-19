#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
// готовый вариант общения плат друг с другом по нопке в самой плате

const char* ssid = "RT-GPON-004B";
const char* password = "nXJGH4V5";
const char* serverAddress = "http://192.168.0.66?LED="; // IP адрес удаленной платы 1
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
  server.begin();  // Запуск HTTP сервера.
  Serial.println("Server started");
  Serial.print("IP Address of network: "); // will IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: https://"); // Will print IP address in URL format
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r'); // Очищаем входной буфер
        // ------------------
        Serial.println(request); // Вывод полученного запроса.
        client.flush(); // Очистка буфера клиента.
        if(request.indexOf("?LED=ON") != -1)
        {
          digitalWrite(ledPin, HIGH); // Инвертируем состояние светодиода
        }
        if(request.indexOf("?LED=OFF") != -1)
        {
          digitalWrite(ledPin, LOW); // Инвертируем состояние светодиода
        }
 
        // Отправка ответа клиенту
        String htmlResponse = generateHTML(); // Генерируем HTML-страницу
        client.print(htmlResponse); // Отправляем HTML-страницу клиенту
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

// Функция для генерации HTML-страницы
String generateHTML() {
  String html = "";
  html += "HTTP/1.1 200 OK\r\n";
  html += "Content-Type: text/html\r\n\r\n";
  html += "<!DOCTYPE html>\r\n";
  html += "<html>\r\n";
  html += "<head>\r\n";
  html += "<title>Control Lamp</title>\r\n";
  html += "<script>\r\n";
  html += "function sendRequest(state) {\r\n";
  html += "  var xhttp = new XMLHttpRequest();\r\n";
  html += "  xhttp.onreadystatechange = function() {\r\n";
  html += "    if (this.readyState == 4 && this.status == 200) {\r\n";
  html += "      document.getElementById(\"lampState\").innerHTML = this.responseText;\r\n";
  html += "    }\r\n";
  html += "  };\r\n";
  html += "  xhttp.open(\"GET\", \"" + String(serverAddress) + "\" + state, true);\r\n";
  html += "  xhttp.send();\r\n";
  html += "}\r\n";
  html += "</script>\r\n";
  html += "</head>\r\n";
  html += "<body>\r\n";
  html += "<div id=\"lampState\">\r\n";
  html += "<!-- Сюда будет подставляться состояние лампочки -->\r\n";
  html += "</div>\r\n";
  html += "<button onclick=\"sendRequest('ON')\">Turn On Lamp</button>\r\n";
  html += "<button onclick=\"sendRequest('OFF')\">Turn Off Lamp</button>\r\n";
  html += "</body>\r\n";
  html += "</html>\r\n";
  return html;
}