#ifndef CAMERA_PINS_H
#define CAMERA_PINS_H

#if defined(CAMERA_MODEL_WROVER_KIT)
#define CAMERA_NAME "CAMERA_MODEL_WROVER_KIT"
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22
#define MAX_RESOULTION   FRAMESIZE_UXGA

#elif defined(CAMERA_MODEL_ESP_EYE)
#define CAMERA_NAME "CAMERA_MODEL_ESP_EYE"
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    4
#define SIOD_GPIO_NUM    18
#define SIOC_GPIO_NUM    23

#define Y9_GPIO_NUM      36
#define Y8_GPIO_NUM      37
#define Y7_GPIO_NUM      38
#define Y6_GPIO_NUM      39
#define Y5_GPIO_NUM      35
#define Y4_GPIO_NUM      14
#define Y3_GPIO_NUM      13
#define Y2_GPIO_NUM      34
#define VSYNC_GPIO_NUM   5
#define HREF_GPIO_NUM    27
#define PCLK_GPIO_NUM    25
#define MAX_RESOULTION   FRAMESIZE_UXGA

#elif defined(CAMERA_MODEL_M5CAM)
#define CAMERA_NAME "CAMERA_MODEL_M5CAM"
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM       5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       17
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21
#define MAX_RESOULTION    FRAMESIZE_UXGA

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
#define CAMERA_NAME "CAMERA_MODEL_M5STACK_PSRAM"
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21
#define MAX_RESOULTION    FRAMESIZE_QXGA

//#define LED_PIN             2
//#define BAT_OUTPUT_HOLD_PIN 33
//#define BAT_ADC_PIN         38

#define BM8563_SDA_PIN      12
#define BM8563_SCL_PIN      14

#define CAMERA_LED_GPIO 2
#define BAT_OUTPUT_HOLD_PIN 33
#define BAT_ADC_PIN 38
#define Ext_PIN_1 4
#define Ext_PIN_2 13
#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include <camera_pins.h>
#include "battery.h"
#include "led.h"
#include "bmm8563.h"

#elif defined(CAMERA_MODEL_M5STACK_WIDE)
#define CAMERA_NAME "CAMERA_MODEL_M5STACK_WIDE"
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     22
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21
#define MAX_RESOULTION   FRAMESIZE_UXGA

#elif defined(CAMERA_MODEL_AI_THINKER) //Board definition "AI Thinker ESP32-CAM"
#define CAMERA_NAME "CAMERA_MODEL_AI_THINKER"
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define FLASH_LAMP_PIN      4

// AI thinker has an SD CARD attached to it
#define SD_CARD_ON true
#define MAX_RESOULTION   FRAMESIZE_UXGA

#elif defined(CAMERA_MODEL_TTGO_T1_CAMERA) // Board definition "ESP32 WROVER Module"
#define CAMERA_NAME "CAMERA_MODEL_TTGO_T1_CAMERA"
#define PWDN_GPIO_NUM     26
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     32
#define SIOD_GPIO_NUM     13
#define SIOC_GPIO_NUM     12

#define Y9_GPIO_NUM       39
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       23
#define Y6_GPIO_NUM       18
#define Y5_GPIO_NUM       15
#define Y4_GPIO_NUM        4
#define Y3_GPIO_NUM       14
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    27
#define HREF_GPIO_NUM     25
#define PCLK_GPIO_NUM     19
#define MAX_RESOULTION   FRAMESIZE_UXGA

#define SDA_PIN 21
#define SCL_PIN 22

#define I2C_DISPLAY_ADDR  0x3c
#define USE_OLED_AS_FLASH 1 // the OLEDis on the same side as the camera
//#define I2C_BME280_ADDR   0x3d
#define PIR_PIN           33 //GPIO_INPUT_IO_33 //AS312
#define PIR_PIN_ON        HIGH
#define BUTTON_PIN        34
#elif defined(CAMERA_MODEL_TTGO_T1_CAMERA_162) // Board definition "LILYGO T1 v1.6.2 White with Mic"
#define CAMERA_NAME "CAMERA_MODEL_TTGO_T1_CAMERA_162"
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     4
#define SIOD_GPIO_NUM     18
#define SIOC_GPIO_NUM     23

/*
// Sources: 
// https://github.com/lewisxhe/esp32-camera-series
// https://github.com/Xinyuan-LilyGO/LilyGo-Camera-Series/tree/master/esphome
// same as before
#define Y9_GPIO_NUM       36
#define Y8_GPIO_NUM       37
#define Y7_GPIO_NUM       38
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM       35
#define Y4_GPIO_NUM       14
#define Y3_GPIO_NUM       13
#define Y2_GPIO_NUM       34
*/
/*
// Source: Whats written on the package
#define Y9_GPIO_NUM       13
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       36
#define Y6_GPIO_NUM       37
#define Y5_GPIO_NUM       38
#define Y4_GPIO_NUM       39
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       14
*/

//SOurce https://github.com/Xinyuan-LilyGO/LilyGo-Camera-Series/blob/master/docs/T_CarmerV16.md
#define Y9_GPIO_NUM       36
#define Y8_GPIO_NUM       15
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM       35
#define Y4_GPIO_NUM       14
#define Y3_GPIO_NUM       13
#define Y2_GPIO_NUM       34

#define VSYNC_GPIO_NUM    5
#define HREF_GPIO_NUM     27
#define PCLK_GPIO_NUM     25
#define MAX_RESOULTION   FRAMESIZE_UXGA

#define SDA_PIN 21
#define SCL_PIN 22

#define I2C_DISPLAY_ADDR  0x3c
#define USE_OLED_AS_FLASH 1 // the OLEDis on the same side as the camera
//#define I2C_BME280_ADDR   0x3d
#define PIR_PIN           19  //NOT RTC IO, i.e. Can't wake up the board from sleep
#define PIR_PIN_ON        HIGH
#define BUTTON_PIN        15

#define MIC_SCK           26
#define MIC_WS            32
#define MIC_SDO           33

#define ENABLE_IP5306

#else
#error "Camera model not selected"
#endif
////////////////////////////////////////
//if you attach your own PIR sensor
// notice: please check the PINS and if they are shared with any other devices
/*
#define PIR_PIN             13
#define PIR_PIN_ON          HIGH
//*/
////////////////////////////////////////
//if there is an SD Attached
// note: PINs 12 and 13 are used for the SD card on AI Thinker board.
// i.e. don't attach PIR or Buzzer while having an SD card.
/*
#define SD_CARD_ON true
//*/
////////////////////////////////////////
// in case you want to add a buzzer
/*
#define BUZZER_PIN           12
#define BUZZER_PIN_ON        HIGH
//*/
////////////////////////////////////////
#if  defined(FLASH_LAMP_PIN)
#define IS_THERE_A_FLASH 1
#endif

#if  defined(USE_OLED_AS_FLASH)
#define IS_THERE_A_FLASH 1
#endif
////////////////////////////////////////
#endif //CAMERA_PINS_H
