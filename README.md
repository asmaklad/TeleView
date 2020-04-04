# TeleView:
	# A telegram-bot to control an ESP-Camera enabled board with many features

# Main Faetures: 
## Send Photo through telegram upon request.
## The telegram bot only responds to the only the ADMINID (find your telegram CHATID through https://web.telegram.org/#/im?p=@chatid_echo_bot )
## control options such as Camera Resolution, Camera-Flip ,Camera-Mirror ,image-resolution ,Time-Lapse send photo which are standard features
## control options such as Flash ,OLED display enable, OLED as Flash, Screen Flip,Motion Detector specific features per some ESP-CAM Boards when available.
## AutoConnect / WiFi config feature (no hard coding of the WiFI SSID,Pass )
## Web Portal to configure the control options and WiFi AP.
## Web Portal to configure Telegram-BOT-Token, AdminID of the BOT
## Web Server for /capture.jpg to make photo available through web. 
## The Device-Name (configurable) decides the hostname of the device on LAN/WiFi 
## 




*Required Installation:*
	# ESP32 for Arduino IDE https://dl.espressif.com/dl/package_esp32_index.json

*Required Libraries to compile:*
	# AutoConnect
	# ArduinoJson - V5.13.5
	# Adafruit SSD1306 and Dependecies such as Adaruit GFX..etc
