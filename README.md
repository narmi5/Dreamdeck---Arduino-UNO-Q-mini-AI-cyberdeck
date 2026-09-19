# Dreamdeck 0.1 - AI cyberdeck

The Dreamdeck is an AI cyberdeck that features plenty of features such as a chat with an offline, local AI, a morse code translator and emitter, dht11 temperature and humidity control, buzzer piano, command terminal and other utility features.

# Requirements

For this device you will need two components:
1. Arduino UNO Q ( https://www.amazon.com/ABX00173-Dragonwing-microprocessor-STM32U585-Microcontroller/dp/B0GFN669S4/ )
2. Freenove ESP32 CYD ( https://www.amazon.com/FREENOVE-ESP32-Display-Screen-240x320/dp/B0GVXSXD7Q/ )

# Setup for Arduino UNO Q

1. Go into the app lab, then go to apps, click on create new app - import app and select the zip folder "cyberdeck.zip".
2. Click on the LLM brick and download the AI model (Qwen if you have 2GB of ram or Gemma if you have 4GB)
3. Run the program once so it can set up and download all the libraries then stop it once it's fully booted.
4. Open the command prompt on the app lab and create the hotspot connection that we'll need to make the UNO Q communicate with the ESP32 CYD by issuing this command: `sudo nmcli device wifi hotspot ssid CYD_AI_Mesh password ArduinoUnoQ123`
5. To start the hotspot then type this into the command prompt and hit send: `sudo nmcli connection up CYD_AI_Mesh`
6. To make the hotspot automatically turn on when starting the board (so that it works when using it on your standalone device once finished without needing to start it manually): `sudo nmcli connection modify CYD_AI_Mesh connection.autoconnect yes`

# Setup for ESP32 CYD

1. Connect the ESP32 CYD to your pc and open the display sketch on the Arduino IDE
2. Go to boards manager (second icon on the vertical bar on the left) and download esp32 by Espressif Systems
3. In the board connection settings, set the board as "ESP32 Wrover Module"
4. Download the following libraries from the library manager: ArduinoWebsockets by Gil Maimon, TFT_eSPI by Bodmer, XPT2046_Touchscreen by Paul Stoffregen
5. On your PC, head to Documents/Arduino/libraries/TFT_eSPI and replace User_Setup and User_Setup_Select with the provided files
6. Upload the cyberdeck_display sketch to the ESP32 CYD

# Wiring

To have both boards running with only one plugged grab the JST wires provided with the ESP32 CYD in the box (the one with 2 wires, black and red) and plug it into the 5V and GND ports on the back of the CYD, then use 2 jumper wires to connect the red one to 5V and the black one to GND on the Arduino.

# GPIO

Currently, the deck supports interactions with the following components: passive buzzer, built in led, dht11 temperature and humidity sensor.
These components are not mandatory for the cyberdeck to work, but they unlock some interesting functions:
1. Connect the buzzer do D3 and GND to use the piano inside the GPIO section
2. Connect the DHT11 to A0, 3.3V and GND to see temperature and humidity in the sensor section

# Notes

1. You can attach the UNO Q to a powerbank so that it works as a standalone device.
2. When plugging/turning on the device, the CYD will fail its initial attempt to connect to the UNO Q, that's completely normal since the Arduino is still booting. When ready, a pulsing heart animation will appear on the LED matrix; when that happens, press the RESET button on the back of the CYD so that it's able to connect.
3. Since the display is quite small, use the stylus to type on the keyboard.

# IMPORTANT FOR MORSE CODE

To translate sentences to and from morse code, head to the AI section and type MORSE before your sentence. You can either choose to translate from morse code, or to morse code, in which case the built in led will blink the sequence before outputting the translation. Don't use punctuation if you're translating to morse and don't use letters if you're translating from morse (except for the word "morse" at the start).

# TERMINAL COMMAND LIST

| Command    | Purpose                                         |
| ---------- | ----------------------------------------------- |
| `ls`       | Lists files and directories                     |
| `pwd`      | Shows the current directory                     |
| `whoami`   | Shows the current user                          |
| `uname`    | Shows system/kernel information                 |
| `date`     | Shows the current date and time                 |
| `uptime`   | Shows how long the UNO Q has been running       |
| `df`       | Shows disk/storage usage                        |
| `free`     | Shows RAM/memory usage                          |
| `hostname` | Shows the device hostname                       |
| `ip`       | Shows network/interface information             |
| `neofetch` | Displays a formatted system-information summary |

