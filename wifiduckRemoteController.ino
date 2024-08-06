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

    String handshake = "GET /ws HTTP/1.1\r\n"
                       "Host: " + String(webSocketServerAddress) + "\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Key: dgzBUei0vRHnbKKdsuH6Yg==\r\n"
                       "Sec-WebSocket-Version: 13\r\n"
                       "Origin: http://192.168.4.1\r\n\r\n";
    client.print(handshake);

    while (!client.available()) {
      delay(10);
    }
    while (client.available()) {
      Serial.write(client.read());
    }

    // Send a test message
    String message = "ls";
    byte header[2];
    header[0] = 0b10000001; // FIN bit set, text frame (opcode 1)
    header[1] = message.length(); // Payload length
    client.write(header, sizeof(header));
    client.print(message);

    // Wait for and print the server's response
    while (client.connected()) {
      if (client.available()) {
        while (client.available()) {
          Serial.write(client.read());
        }
      }
    }

    // Close the WebSocket connection
    client.stop();
    Serial.println("WebSocket connection closed");
  } else {
    Serial.println("Failed to connect to WebSocket server");
  }
}

void loop() {
  // Empty loop
}
