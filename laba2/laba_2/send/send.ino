#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

// const char* ssid = "HONOR 50";
// const char* password = "Ae97980525";
const char* ssid = "RT-GPON-004B";
const char* password = "nXJGH4V5";
int LED = 2;  // Светодиод подключен к GPIO2 (D4).
String serverName = "192.168.0.67";

WiFiServer server(80);

void setup()
{
  Serial.begin(115200); // Инициализация последовательной связи с скоростью 115200 бит/с.
  pinMode(LED, OUTPUT); // Установка пина светодиода на вывод.
  digitalWrite(LED, LOW); // Изначально выключаем светодиод.
  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password); // Подключение к Wi-Fi сети.
  while (WiFi.status() != WL_CONNECTED) // Ждем, пока ESP8266 не подключится к сети.
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected"); 
  server.begin();  // Запуск HTTP сервера.
  Serial.println("Server started");
  Serial.print("IP Address of network: "); // will IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: https://"); // Will print IP address in URL format
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop()
{
  int value = LOW;

  WiFiClient client = server.available(); // Проверка наличия клиентского подключения.
  if (!client) { // Если клиент не подключен, выходим из функции loop.
    return;
  }

  Serial.println("Waiting for new client");
  while(!client.available()) { // Пока нет доступных данных от клиента, ждем.
    delay(1);
  }

  String request = client.readStringUntil('\r'); // Чтение запроса от клиента до символа возврата каретки.
  Serial.println(request); // Вывод полученного запроса.
  client.flush(); // Очистка буфера клиента.

  String state = "?LED=ON";
  if(request.indexOf("LED=ON") != -1) {
    digitalWrite(LED, HIGH); // Turn LED ON
    value = HIGH;
  }

  if(request.indexOf("LED=OFF") != -1){
    digitalWrite(LED, LOW); // Turn LED OFF
    value = LOW;
    state = "?LED=OFF";
  }

  controlLamp(state.c_str()); // Отправка HTTP-запроса на сервер управления лампочкой.  
  
  //*------------------HTML Page Code---------------------*//
  // Отправляем HTTP-заголовки
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html"); // Устанавливаем тип контента как HTML
  client.println(""); // Пустая строка между заголовками и телом ответа

  // HTML-разметка
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>Control Lamp</title>");
  client.println("<script>");
  client.println("function sendRequest(state) {");
  client.println("  var xhttp = new XMLHttpRequest();");
  client.println("  xhttp.onreadystatechange = function() {");
  client.println("    if (this.readyState == 4 && this.status == 200) {");
  client.println("      document.getElementById(\"lampState\").innerHTML = this.responseText;");
  client.println("    }");
  client.println("  };");
  client.println("  xhttp.open(\"GET\", \"/\" + state, true);");
  client.println("  xhttp.send();");
  client.println("}");
  client.println("</script>");
  client.println("</head>");
  client.println("<body>");
  client.println("<div id=\"lampState\">");
  client.println("<!-- Сюда будет подставляться состояние лампочки -->");
  client.println("</div>");
  client.println("<button onclick=\"sendRequest('lamp=ON')\">Turn On Lamp</button>");
  client.println("<button onclick=\"sendRequest('lamp=OFF')\">Turn Off Lamp</button>");
  client.println("</body>");
  client.println("</html>");
  
  
  
  
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

// Функция для отправки запроса на сервер управления лампочкой.
void controlLamp(const char* command) {
  if(WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Создаем объект WiFiClient для использования в HTTPClient
    HTTPClient http;
    String url = "http://" + String(serverName) + "/control" + String(command);
    http.begin(client, url); // Используем метод begin(WiFiClient, url) для установки соединения
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
