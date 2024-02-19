#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

// const char* ssid = "HONOR 50";
// const char* password = "Ae97980525";
const char* ssid = "HONOR 50";
const char* password = "Ae97980525";
int LED = 2;   // led connected to GPIO2 (D4)
String serverName = "192.168.1.199";

WiFiServer server(80);

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup()
{
  Serial.begin(115200); //Default Baudrate
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected"); 
  server.begin();  // Starts the Server
  Serial.println("Server started");
  Serial.print("IP Address of network: "); // will IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: https://"); // Will print IP address in URL format
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop()
{
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("Waiting for new client");
  while(!client.available()) {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  int value = LOW;
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


  // Send an HTTP
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + state;
      http.begin(client, serverPath.c_str());
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

//*------------------HTML Page Code---------------------*//

  client.println("HTTP/1.1 200 OK"); //
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print(" CONTROL LED: ");
  if(value == HIGH) {
    client.print("ON");
  }
  else {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<a href=\"/?LED=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/?LED=OFF\"\"><button>OFF</button></a><br />");
  client.println("</html>");
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

void sendGETRequest(String path) {
  WiFiClient client;
  if (!client.connect(serverName, 80)) {
    Serial.println("Connection failed");
    return;
  }
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + serverName + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  client.stop();
}