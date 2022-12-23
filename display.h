

#ifndef DISPLAY_H
#define DISPLAY_H
#if defined(I2C_DISPLAY_ADDR)

static const char* TAG_OLED = "DISPLAY";
#include "esp_log.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String line1="";
String line2="";
String line3="";
boolean displayEnabled=true;
///////////////////////////////////////////
void display_init();
void display_Textlines(String lline1, String lline2, String lline3);
void display_AllWhite();
void display_Clear();
///////////////////////////////////////////
void display_init(){
  Wire.begin(SDA_PIN, SCL_PIN);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_DISPLAY_ADDR)) { // Address 0x3D for 128x64
    ESP_LOGV( TAG_PS, "SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.  
  display.clearDisplay();
  display.display();
}

void display_Textlines(String lline1, String lline2="", String lline3=""){
  display.clearDisplay();
  if (displayEnabled) {
    display.setCursor(0, 0);
    //
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2); // Draw 2X-scale text
    display.println(lline1);  
    //
    display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    display.setTextSize(1);
    display.println(lline2);
    //
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println(lline3);
  }
  //
  display.display();
}

void display_AllWhite(){
  display.clearDisplay();
  if (displayEnabled) {
    display.fillRect(0, 0, display.width()*2, display.height()*2, SSD1306_INVERSE);
  }
  display.display();
}

void display_Clear(){
  display.clearDisplay();  
  display.display();
}
#endif //defined(I2C_DISPLAY_ADDR)

#endif //CAMERA_PINS_H
