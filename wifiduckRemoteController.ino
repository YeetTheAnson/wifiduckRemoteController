#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// WiFi credentials
const char* ssid = "wifiduck";
const char* password = "wifiduck";      
const char* ssidAlt = "wifiduckalt";     
const char* passwordAlt = "wifiduckalt"; 
const char* webSocketServerAddress = "192.168.4.1";
const int webSocketServerPort = 80;

// OLED display setup
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Button GPIO pins
const int upButton = 12;
const int downButton = 14;
const int selectButton = 13;

// Menu handling
int menuOption = 0;
int topLine = 0;
int numberhaha = 0;
String receivedMessage;
String menuItemsArray[100];

void sendWebSocketMessage(WiFiClient& client, String message) {
  // Craft the WebSocket frame
  byte header[2];
  header[0] = 0b10000001; 
  header[1] = message.length

  // Send the frame header
  client.write(header, sizeof(header));

  // Send the message payload
  client.print(message);
}

bool receiveAndPrintWebSocketMessage(WiFiClient& client) {
  String message = "";
  while (client.available()) {
    char c = client.read();
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '\n') {
      message += c;
    }
  }

  
  int start = 0;
  bool lastCharWasSpace = true;
  for (int i = 0; i < message.length(); i++) {
    if (message[i] == ' ' || message[i] == '\n') {
      if (!lastCharWasSpace) {
        String item = message.substring(start, i);
        menuItemsArray[numberhaha++] = item; 
      }
      start = i + 1;
      lastCharWasSpace = true;
    } else {
      lastCharWasSpace = false;
    }
  }
  // Add the last item
  if (!lastCharWasSpace) {
    String lastItem = message.substring(start);
    menuItemsArray[numberhaha++] = lastItem;
  }

  receivedMessage = message;
  Serial.println(receivedMessage);
  return true;
}

void setup() {
  // Initialize Serial and display
  Serial.begin(9600);

  
  Wire.begin(5, 4); 

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);

  // Connect to WiFi
  bool connectToAlternateWiFi = false; // Assume default connection, modify as needed
  if (connectToAlternateWiFi) {
    WiFi.begin(ssidAlt, passwordAlt);
  } else {
    WiFi.begin(ssid, password);
  }

  // Display "Connecting to WiFi" 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 32);
  display.println("Connecting to WiFi");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  Serial.println("Connected to WiFi");

  display.clearDisplay();
  display.display();

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

    while (!client.available()) {
      delay(10);
    }
    while (client.available()) {
      Serial.write(client.read());
    }

    sendWebSocketMessage(client, "ls");

    while (client.connected()) {
      if (client.available()) {
        if (receiveAndPrintWebSocketMessage(client)) {
          break; // Exit the loop if a message was received
        }
      }
    }

    // Close the WebSocket connection
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
  // button presses for scrolling
  int upButtonState = digitalRead(upButton);
  int downButtonState = digitalRead(downButton);

  if (upButtonState == LOW) {
    menuOption = (menuOption > 0) ? menuOption - 1 : numberhaha - 1;
    // selector moves above the top line, scroll up
    // selector wraps around to the bottom, jump to the bottom
    topLine = (menuOption < topLine || menuOption == numberhaha - 1) ? menuOption : topLine;
    delay(200); 
  } else if (downButtonState == LOW) {
    menuOption = (menuOption < numberhaha - 1) ? menuOption + 1 : 0;
    // selector moves below the bottom line, scroll down
    // selector wraps around to the top, jump to the top
    topLine = (menuOption >= topLine + 3 || menuOption == 0) ? menuOption : topLine;
    delay(200); // 
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Display Content");
  display.display();
}
