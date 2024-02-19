#include <ESP8266WiFi.h>

// const char* ssid = "HONOR 50";
// const char* password = "Ae97980525";
const char* ssid = "RT-GPON-004B";
const char* password = "nXJGH4V5";
int LED = 2;   // Светодиод подключен к GPIO2 (D4).

WiFiServer server(80); // Создание сервера на порту 80 для обработки запросов.

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
  WiFiClient client = server.available(); // Проверка наличия клиентского подключения.
  if (!client) // Если клиент не подключен, выходим из функции loop.
  {
    return;
  }
  Serial.println("Waiting for new client");
  while(!client.available()) // Пока нет доступных данных от клиента, ждем.
  {
    delay(1);
  }

  String request = client.readStringUntil('\r'); // Чтение запроса от клиента до символа возврата каретки.
  Serial.println(request); // Вывод полученного запроса.
  client.flush(); // Очистка буфера клиента.

  int value = LOW;

  if(request.indexOf("/LED=ON") != -1)
  {
    digitalWrite(LED, HIGH); // Turn LED ON
    value = HIGH;
  }

  if(request.indexOf("/LED=OFF") != -1)
  {
    digitalWrite(LED, LOW); // Turn LED OFF
    value = LOW;
  }


//*------------------HTML Page Code---------------------*//

  client.println("HTTP/1.1 200 OK"); //
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print(" CONTROL LED: ");
  if(value == HIGH)
  {
    client.print("ON");
  }
  else
  {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>OFF</button></a><br />");
  client.println("</html>");
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}