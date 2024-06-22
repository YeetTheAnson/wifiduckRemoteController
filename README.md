# wifiduckRemoteController
A portable remote controller for WifiDuck by SpacehuhnTech

This repository is intended to add on to the work of [SpacehuhnTech's WifiDuck](https://github.com/SpacehuhnTech/WiFiDuck).


## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Acknowledgements](#acknowledgements)

## Installation

1. Go to the [Releases](https://github.com/YeetTheAnson/wifiduckRemoteController/releases) page and download the latest release file.
2. Open the `.ino` file in Arduino IDE.
3. Set the configuration by changing the SSID and password in the code.
4. Add ESP8266 to the board manager:
    - Go to `File` >> `Preferences` >> `Additional Boards Manager URLs`
    - Paste the following URL: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
5. Select the board `NodeMCU 1.0 (ESP-12E Module)`.
6. Flash the code to the ESP8266.

## Usage
![Circuit Diagram](https://github.com/YeetTheAnson/wifiduckRemoteController/raw/main/Circuit%20Diagram.png)

1. Connect the buttons:
    - Up button to GPIO12
    - Down button to GPIO14
    - Select button to GPIO13
2. Connect the SSD1306 display:
    - SCL to GPIO5
    - SDA to GPIO4
3. Turn on the ESP8266. It will connect to the WiFi credentials set in the code.
4. Booting the ESP8266 while holding the Select button will connect it to the alternate WiFi credentials set in the code.
5. Use the buttons:
    - Press Up and Down to navigate between scripts.
    - Press Select to run the selected script.

## Features

- Portable remote controller to control SpacehuhnTech's WiFiDuck without using a phone or a laptop.


## Acknowledgements

Special thanks to [SpacehuhnTech](https://github.com/SpacehuhnTech/WiFiDuck) for WiFiDuck.


