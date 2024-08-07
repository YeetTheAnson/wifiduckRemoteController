#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
int topLine = 0;
int numberhaha = 0;


const char* ssid = "wifiduck";            //Set ssid1
const char* password = "wifiduck";        //Set pass1
const char* ssidAlt = "wifiduckalt";      //For setting alternate ssid, if you want to connect between 2 wifiduck
const char* passwordAlt = "wifiduckalt";  //Hold selectButton while booting to connect to alternate wifi
const char* webSocketServerAddress = "192.168.4.1";
const int webSocketServerPort = 80;

#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 64 // OLED display height

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const int upButton = 12;
const int downButton = 14;
const int selectButton = 13;

int menuOption = 0;
String menuItemsArray[100];
String receivedMessage;

void sendWebSocketMessage(WiFiClient& client, String message) {
  // Craft the WebSocket frame
  byte header[2];
  header[0] = 0b10000001; // FIN bit set, text frame (opcode 1)
  header[1] = message.length(); // Payload length
  
  // Send the frame header
  client.write(header, sizeof(header));
  
  // Send the message payload
  client.print(message);
}

bool receiveAndPrintWebSocketMessage(WiFiClient& client) {
  String message = "";
  while (client.available()) {
    char c = client.read();
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '/' || c == '\n') {
      message += c;
    }
  }
  
  // Split the message into items
  String formattedMessage = "";
  int start = 0;
  bool lastCharWasSpace = true;
  for (int i = 0; i < message.length(); i++) {
    if (message[i] == ' ' || message[i] == '\n') {
      if (!lastCharWasSpace) {
        String item = message.substring(start, i);
        formattedMessage += "\"" + item + "\", ";
        numberhaha++; // Increment the count
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
    formattedMessage += "\"" + lastItem + "\"";
    numberhaha++; // Increment the count
  }
  
  receivedMessage = formattedMessage;
  Serial.println(receivedMessage);
  Serial.println(numberhaha);
  return true;
}

bool receiveAndPrintWebSocketMessageAlt(WiFiClient& client) {
  String message = "";
  while (client.available()) {
    char c = client.read();
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '/' || c == '\n') {
      message += c;
    }
  }
  
  // Split the message into items
  String formattedMessage = "";
  int start = 0;
  bool lastCharWasSpace = true;
  for (int i = 0; i < message.length(); i++) {
    if (message[i] == ' ' || message[i] == '\n') {
      if (!lastCharWasSpace) {
        String item = message.substring(start, i);
        formattedMessage += "\"" + item + "\", ";
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
    formattedMessage += "\"" + lastItem + "\"";
  }
  
  receivedMessage = formattedMessage;
  Serial.println(receivedMessage);
  Serial.println(numberhaha);
  return true;
}


void setup() {
  Wire.begin(5, 4);

  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Set up the buttons
  delay(1000);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);

  // Check if the selectButton is pressed during setup
  bool connectToAlternateWiFi = digitalRead(selectButton) == LOW;

  // Initialize I2C

  // Display "Connecting to WiFi" on the screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 32);
  if (connectToAlternateWiFi) {
    display.println("Connecting to alternate WiFi");
  } else {
    display.println("Connecting to WiFi");
  }
  display.display();

  // Connect to appropriate WiFi based on button press
  if (connectToAlternateWiFi) {
    WiFi.begin(ssidAlt, passwordAlt);
  } else {
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");

    // Add a dot to the screen every half second
    display.print(".");
    display.display();
  }

  Serial.println("Connected to WiFi");

  // Clear the display after connecting to WiFi
  display.clearDisplay();
  display.display();

  if (WiFi.status() == WL_CONNECTED) {
    // Establish a TCP connection to the WebSocket server
    WiFiClient client;
    if (client.connect(webSocketServerAddress, webSocketServerPort)) {
      // Craft the WebSocket handshake request
      String handshake = "GET /ws HTTP/1.1\r\n"
                         "Host: " + String(webSocketServerAddress) + "\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         "Sec-WebSocket-Key: dgzBUei0vRHnbKKdsuH6Yg==\r\n"
                         "Sec-WebSocket-Version: 13\r\n"
                         "Origin: http://192.168.4.1\r\n\r\n";
      
      // Send the handshake request
      client.print(handshake);

      // Wait for the server's response
      while (!client.available()) {
        delay(10);
      }

      // Print the server's response
      while (client.available()) {
        Serial.write(client.read());
      }

      // Send the message
      String message = "ls";
      sendWebSocketMessage(client, message);
      
      // Receive and print WebSocket messages
      while (client.connected()) {
        if (client.available()) {
          if (receiveAndPrintWebSocketMessage(client)) {
            break; // Exit the loop if a message was received
          }
        }
      }
      
      // Close the connection
      client.stop();
    } else {
      Serial.println("Failed to connect to WebSocket server");
    }
  }

  Serial.println(receivedMessage);
  String menuItems = receivedMessage;

  // Split the string into an array of strings (items)
  int arrayIndex = 0;
  int pos = 0;
  while ((pos = menuItems.indexOf(',')) != -1) {
    menuItemsArray[arrayIndex] = menuItems.substring(0, pos);
    menuItems = menuItems.substring(pos + 2); // +2 to skip the comma and space
    arrayIndex++;
  }
  menuItemsArray[arrayIndex] = menuItems; // Add the last item

  // Print all items in the array to the serial monitor
  for (int i = 0; i < numberhaha; i++) {
    Serial.println(menuItemsArray[i]);
  }
}





void loop() {
  int upButtonState = digitalRead(upButton);
  int downButtonState = digitalRead(downButton);
  int selectButtonState = digitalRead(selectButton);

  if (upButtonState == LOW) {
    menuOption = (menuOption > 0) ? menuOption - 1 : numberhaha - 1;
    // If the selector moves above the top line, scroll up
    // If the selector wraps around to the bottom, jump to the bottom
    topLine = (menuOption < topLine || menuOption == numberhaha - 1) ? menuOption : topLine;
    delay(200);
  } else if (downButtonState == LOW) {
    menuOption = (menuOption < numberhaha - 1) ? menuOption + 1 : 0;
    // If the selector moves below the bottom line, scroll down
    // If the selector wraps around to the top, jump to the top
    topLine = (menuOption >= topLine + 3 || menuOption == 0) ? menuOption : topLine;
    delay(200);
  } else if (selectButtonState == LOW) {
    Serial.println(menuItemsArray[menuOption]);
    if (WiFi.status() == WL_CONNECTED) {
    // Establish a TCP connection to the WebSocket server
    WiFiClient client;
    if (client.connect(webSocketServerAddress, webSocketServerPort)) {
      // Craft the WebSocket handshake request
      String handshake = "GET /ws HTTP/1.1\r\n"
                         "Host: " + String(webSocketServerAddress) + "\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         "Sec-WebSocket-Key: dgzBUei0vRHnbKKdsuH6Yg==\r\n"
                         "Sec-WebSocket-Version: 13\r\n"
                         "Origin: http://192.168.4.1\r\n\r\n";
      
      // Send the handshake request
      client.print(handshake);

      // Wait for the server's response
      while (!client.available()) {
        delay(10);
      }

      // Print the server's response
      while (client.available()) {
        Serial.write(client.read());
      }

      // Send the message
      sendWebSocketMessage(client, "run " + menuItemsArray[menuOption]);

      
      // Receive and print WebSocket messages
      while (client.connected()) {
        if (client.available()) {
          if (receiveAndPrintWebSocketMessageAlt(client)) {
            break; // Exit the loop if a message was received
          }
        }
      }
      
      // Close the connection
      client.stop();
    } else {
      Serial.println("Failed to connect to WebSocket server");
    }
  }
    delay(100);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  for (int i = 0; i < 3; i++) {
    if (topLine + i < numberhaha) {
      if (topLine + i == menuOption) {
        display.print("> ");
      } else {
        display.print("  ");
      }
      display.println(menuItemsArray[topLine + i]);
    }
  }

  display.display();
}
