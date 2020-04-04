# TeleView:
	* A telegram-bot to control an ESP-Camera enabled board with many features

## Main Faetures: 
* Send Photo through telegram upon request.
* The telegram bot responds to only the ADMINID (find your telegram CHATID through https://web.telegram.org/#/im?p=@chatid_echo_bot )
* Control options such as Camera Resolution, Camera-Flip ,Camera-Mirror ,image-resolution ,Time-Lapse send photo which are standard features
* control options such as Flash ,OLED display enable, OLED as Flash, Screen Flip,Motion Detector specific features per some ESP-CAM Boards when available.
* AutoConnect / WiFi config feature (no hard coding of the WiFI SSID,Pass )
* Web Portal to configure the control options and WiFi AP.
* Web Portal to configure Telegram-BOT-Token, AdminID of the BOT
* Web Server for /capture.jpg to make photo available through web. 
* The Device-Name (configurable) decides the hostname of the device on LAN/WiFi 

## Required Installation:
* ESP32 for Arduino IDE https://dl.espressif.com/dl/package_esp32_index.json

## Required Libraries to compile:
* AutoConnect
* ArduinoJson - V5.13.5
* Adafruit SSD1306 and Dependecies such as Adaruit GFX..etc

User Guide:
## Compile and Upload 
 This project is Compiled and uploaded through ArduinoIDE, use the normal procedure for installing ESP32 in ArduinoIDE and install the following Libs:
*    AutoConnect
*    ArduinoJson - V5.13.5
*    Adafruit SSD1306 and Dependecies such as Adaruit GFX..etc

Before uploading chose the target ESP32-CAM Board :

'''
// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
//#define CAMERA_MODEL_AI_THINKER         // Board definition "AI Thinker ESP32-CAM"
#define CAMERA_MODEL_TTGO_T1_CAMERA      // Board definition "TTGO T1" 
'''

The "camera_pins.h" has the speces of each board pins and also things like supported features such as OLED , Flash ..etc
This code has been tested on CAMERA_MODEL_AI_THINKER and CAMERA_MODEL_TTGO_T1_CAMERA modules.

For the CAMERA_MODEL_TTGO_T1_CAMERA borad, please chose the "TTGO T1" and and set Tools-> Partiton Scheme --> Huge App (3MB No OTA/1MB SPIFF)

## Setting up the WiFi:
This sektch uses the Autoconnect Arduino library (https://github.com/Hieromon/AutoConnect) to let the user configure WiFi SSID and Password.
When the sketch starts, it looks for a familiar WiFi. If it can't fid one, it will starts its own Access Point when the owner can connect. 
First Time- SSID will be "TeleView" with the password "tv-ei-694"

The AutoConnect library will start a portal wiht the bage /config to configure the Wifi and other configuration parameters.


