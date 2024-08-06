#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "wifiduck";          
const char* password = "wifiduck";        
const char* ssidAlt = "wifiduckalt";      
const char* passwordAlt = "wifiduckalt";  
const char* webSocketServerAddress = "192.168.4.1";
const int webSocketServerPort = 80;

String receivedMessage;
String menuItemsArray[100];
int numberhaha = 0;

void sendWebSocketMessage(WiFiClient& client, String message) {
  // Craft the WebSocket frame
  byte header[2];
  header[0] = 0b10000001; 
  header[1] = message.length(); 

  // Send the frame header
  client.write(header, sizeof(header));

  client.print(message);
}


// Strips out unnecessary parts 
bool receiveAndPrintWebSocketMessage(WiFiClient& client) {
  String message = "";
  while (client.available()) {
    char c = client.read();
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '\n') {
      message += c;
    }
  }

  // Split the message into array
  int start = 0;
  bool lastCharWasSpace = true;
  for (int i = 0; i < message.length(); i++) {
    if (message[i] == ' ' || message[i] == '\n') {
      if (!lastCharWasSpace) {
        String item = message.substring(start, i);
        menuItemsArray[numberhaha++] = item; // Store in the array
      }
      start = i + 1;
      lastCharWasSpace = true;
    } else {
      lastCharWasSpace = false;
    }
  }
  if (!lastCharWasSpace) {
    String lastItem = message.substring(start);
    menuItemsArray[numberhaha++] = lastItem;
  }

  receivedMessage = message;
  Serial.println(receivedMessage);
  return true;
}

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  bool connectToAlternateWiFi = false; 
  if (connectToAlternateWiFi) {
    WiFi.begin(ssidAlt, passwordAlt);
  } else {
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Establish a WebSocket connection
  WiFiClient client;
  if (client.connect(webSocketServerAddress, webSocketServerPort)) {
    Serial.println("Connected to WebSocket server");

    // Craft and send the WebSocket handshake request
    String handshake = "GET /ws HTTP/1.1\r\n"
                       "Host: " + String(webSocketServerAddress) + "\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Key: dgzBUei0vRHnbKKdsuH6Yg==\r\n"
                       "Sec-WebSocket-Version: 13\r\n"
                       "Origin: http://192.168.4.1\r\n\r\n";
    client.print(handshake);

    // Wait for and print the server's response
    while (!client.available()) {
      delay(10);
    }
    while (client.available()) {
      Serial.write(client.read());
    }

    // Send the ls
    sendWebSocketMessage(client, "ls");

    // Receive and print the WebSocket message
    while (client.connected()) {
      if (client.available()) {
        if (receiveAndPrintWebSocketMessage(client)) {
          break; 
        }
      }
    }

    client.stop();
    Serial.println("WebSocket connection closed");

    for (int i = 0; i < numberhaha; i++) {
      Serial.println(menuItemsArray[i]);
    }
  } else {
    Serial.println("Failed to connect to WebSocket server");
  }
}

void loop() {
}
