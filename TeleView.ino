/*
   Copyright (c) 2020 Ahmed Maklad. All right reserved.

   Teleview- A telegram photo sending bot using the ESP32 CAM on Arduino IDE.

   This Software is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This Software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this Software; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*
 * To compile and upload:
 * Required Installation:
 *    ESP32 for Arduino IDE https://dl.espressif.com/dl/package_esp32_index.json
 * Required Libraries to compile:
 *    AutoConnect
 *    ArduinoJson - V5.13.5
 *    Adafruit SSD1306 and Dependecies such as Adaruit GFX..etc
*/
//****************************************************************//
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <AutoConnect.h>

#include <ArduinoJson.h>
#include <Ticker.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"

#include <Arduino.h>
static const char* TAG_MAIN = "MAIN";
#include "esp_log.h"

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM      // Board definition Boards->ESP32 Arduino->"M5Stack Timer-CAM"
                                          //  Don't use the  Boards->M5Stack Arduino ->"M5Stack Timer CAM"
//#define CAMERA_MODEL_M5STACK_WIDE
//#define CAMERA_MODEL_AI_THINKER         // Board definition "AI Thinker ESP32-CAM"
#define CAMERA_MODEL_TTGO_T1_CAMERA     // Board definition "ESP32 WROVER Module" or "TTGO T1"
                                          // to Have OTA Working:
                                          // tools->Patition Schema-> Minimal SPIFFS(1.9MB with OTA/190KB SPIFFS)
//#define CAMERA_MODEL_TTGO_T1_CAMERA_162   // Board definition "ESP32 WROVER Module" or "TTGO T1"
                                          // to Have OTA Working:
                                          // tools->Patition Schema-> Minimal SPIFFS(1.9MB with OTA/190KB SPIFFS)
//#define CAMERA_MODEL_M5CAM              // Board Difinition  "AI Thinker ESP32-CAM"
//////////////////////////////////////    // and set Tools-> Partiton Scheme --> Huge App (3MB No OTA/1MB SPIFF)
#include "camera_pins.h"

framesize_t maxRes=MAX_RESOULTION;

//////////////////////////////////////
String compileDate=String(__DATE__);
String compileTime=String(__TIME__);
String compileCompiler=String(__cplusplus);
int PICTURES_COUNT=0;
//////////////////////////////////////
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
static const char* TAG01 = "SETUP";

//////////////////////////////////////
#include "persist.h"
void applyConfigItem (config_item* ci);
#include "webPages.h"
#include "motionDetect.h"

#if defined(I2C_DISPLAY_ADDR)
#include "display.h"
#endif

#include "telegram_utils.h"
#define uS_TO_S_FACTOR 1000000 

/* One I day I will give up and remove this.. but not today.
#if defined(SD_CARD_ON)
#include "FSBRowser.h"
#endif
*/

const char* ntpServer = "pool.ntp.org";
long  gmtOffset_sec = 0;
int   daylightOffset_sec = 0;

bool bTakePhotoTick=false;
boolean bMotionDetected=false;
boolean bESPMayGoToSleep=false;
Ticker tkTimeLapse;
//int consequentChangedFrames=0;

//motionDetection
bool haveMotion = false;
// struct tm *startTM; you find it in webPages.h

esp_sleep_wakeup_cause_t print_wakeup_reason();

//****************************************************************//
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disbale the burnout reset
  Serial.begin(115200);
  ESP_LOGV(TAG_MAIN,"SETUP START: %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
  ESP_LOGV(TAG_MAIN,CAMERA_NAME);
  psramInit();
  ESP_LOGV(TAG_MAIN,"Total heap: %d", ESP.getHeapSize());
  ESP_LOGV(TAG_MAIN,"Free heap: %d", ESP.getFreeHeap());
  ESP_LOGV(TAG_MAIN,"Total PSRAM: %d", ESP.getPsramSize());
  ESP_LOGV(TAG_MAIN,"Free PSRAM: %d", ESP.getFreePsram());
#ifdef CAMERA_MODEL_M5STACK_PSRAM
// will hold bat output
  bat_init();
  led_init(CAMERA_LED_GPIO);
  bmm8563_init();
#endif
  delay (1000);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_INFO);
  //esp_log_level_set("*", ESP_LOG_ERROR);        // set all components to ERROR level
  //esp_log_level_set("wifi", ESP_LOG_WARN);      // enable WARN logs from WiFi stack
  //esp_log_level_set("dhcpc", ESP_LOG_INFO);     // enable INFO logs from DHCP client

  ESP_LOGV(TAG_MAIN,"Compile Date:%s",compileDate);
  ESP_LOGV(TAG_MAIN,"Compile Time:%s",compileTime);
  ESP_LOGV(TAG_MAIN,"Compile Compiler:",compileCompiler);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  //
  //ESP CAMERA
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    maxRes=MAX_RESOULTION;
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    maxRes=FRAMESIZE_SVGA;
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  ESP_LOGV(TAG_MAIN,"frame_size: %s:%s",
    resolutions[config.frame_size][0],
    resolutions[config.frame_size][1] 
  );
  ESP_LOGV(TAG_MAIN,"jpeg_quality: %d",config.jpeg_quality);
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG_MAIN,"Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
  bCameraInitiated=true;
  ////////////////////////////
  configItems.frameSize=config.frame_size;
  keyboardJson=formulateKeyboardJson();
  configItems=loadConfiguration();
  applyConfigItem(&configItems);
  ESP_LOGV(TAG_MAIN,"%s",printConfiguration(&configItems,""));

////////////////////////////
#if defined(I2C_DISPLAY_ADDR)
  display_init();
#endif
  ////////////////////////////
  /*
#if defined(SD_CARD_ON)
  setupFSBrowser();
#endif
*/
  ESP_LOGV("SETUP","Configuring the Web Pages!");
  Portal.host().on("/",rootPage);
  Portal.host().on("/delete",deletePage);
  Portal.host().on("/capture",capturePage);
  Portal.host().on("/capture2",capture2Page)
  Portal.host().on("/capture.jpg",capturePageJpeg);
  //
  auxPageConfig.load(AUX_CONFIGPAGE);
  populateResolutionsSelects(auxPageConfig);
  auxPageConfig.on(onPage);
  //
  auxPageCapture.load(AUX_CAPTURE);
  auxPageCapture.on(onCapture);
  //
  Portal.join(auxPageCapture);
  Portal.join(auxPageConfig);
  ////////////////////////////
  acConfig.apid = configItems.deviceName;
  acConfig.psk  = "tv-ei-694";
  acConfig.hostName=configItems.deviceName;
  acConfig.autoRise=true;
  acConfig.title = "TeleView";
  //AUTOCONNECT_USE_PREFERENCES
  acConfig.autoSave=AC_SAVECREDENTIAL_AUTO;
  //acConfig.portalTimeout = 60000;  // It will time out in 60 seconds
  #define AC_DEBUG 1
  Portal.config(acConfig);
  Portal.onDetect(captivePortalStarted);
  WiFi.setSleep(false);
  WiFi.setHostname(configItems.deviceName.c_str());
  /////////////////////////////
#if defined(I2C_DISPLAY_ADDR)
  line1=String(acConfig.apid);
  line2=String(acConfig.psk);
  line3=String(acConfig.apip.toString());
  display_Textlines(line1,line2,line3);
#endif
  
  /////////////////////////////
  if (Portal.begin()) {
    ESP_LOGV(TAG_MAIN,"WiFi connected: %s " , WiFi.localIP().toString().c_str() );
    //MDNS
    if (!MDNS.begin(configItems.deviceName.c_str())) {
        ESP_LOGE(TAG_MAIN,"Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
    // Add service to MDNS-SD
    // With applying AutoConnect, the MDNS service must be started after
    // establishing a WiFi connection.  
    MDNS.addService("http", "tcp", 80);
    ESP_LOGV(TAG_MAIN,"mDNS responder started");
  }else{
    ESP_LOGE(TAG_MAIN,"Portal not startd");
  }
  ESP_LOGV(TAG_MAIN,"HTTP server started");
  ESP_LOGV(TAG_MAIN,"Connected SSID: %s",WiFi.SSID() );
  ESP_LOGV(TAG_MAIN,"RSSI:%d",WiFi.RSSI());
  //
  byte mac[6];
  WiFi.macAddress(mac);
  ESP_LOGV(TAG_MAIN,"WiFi MAC= %x:%x:%x:%x:%x:%x",
      mac[5],
      mac[4],
      mac[3],
      mac[2],
      mac[1],
      mac[0]
  );
  //
  ESP_LOGV(TAG_MAIN,"IP address: %s",WiFi.localIP().toString().c_str());
  // NTP //////////////////////////////
  //init and get the time
  for (int i=0;i<=23;i++) {
    Timezone_t tempTz = TZ [i];
    //if (strcmp(tempTz.zone,configItems.timeZone.c_str())==0){
    if (configItems.timeZone.equals(tempTz.zone) ){
      //configItems.timeZone=tempTz.zone;
      ntpServer=(char*)tempTz.ntpServer;
      gmtOffset_sec=tempTz.tzoff*60*60;
      break;
    }
  }
  ///////////////////////////////////
  daylightOffset_sec=0;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  #ifdef CAMERA_MODEL_M5STACK_PSRAM
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      ESP_LOGV(TAG_MAIN,"Failed to obtain time");
      return;
    }
    _rtc_data_t  timeToSet;
    timeToSet.year=timeinfo.tm_year+1900;
    timeToSet.month=timeinfo.tm_mon+1;
    timeToSet.day=timeinfo.tm_mday;
    timeToSet.hour=timeinfo.tm_hour;
    timeToSet.second=timeinfo.tm_sec;
    bmm8563_setTime(&timeToSet);
  #endif
  ///////////////////////////////////
  botClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  //botClient.setInsecure();  
  bot.updateToken(configItems.botTTelegram);
  if ( !configItems.botTTelegram.equals("0123456789")  ) {
    ESP_LOGV(TAG_MAIN,"I am Alive :-) ");
    //botClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    //botClient.setInsecure();
    if(bot.getMe()){
      ESP_LOGV(TAG_MAIN,"bot.getMe():TRUE");
    }else{
      ESP_LOGV(TAG_MAIN,"bot.getMe():FALSE");
    }
    //bot.sendMessage(configItems.adminChatIds, "I am Alive!!", "");
    bool bSendMessageWithReplyKeyboard=bot.sendMessageWithReplyKeyboard(configItems.adminChatIds, "I am Alive :-) ", "Markdown", formulateKeyboardJson(), true);
    if(bSendMessageWithReplyKeyboard){
      ESP_LOGV(TAG_MAIN,"bSendMessageWithReplyKeyboard:TRUE");
    }else{
      ESP_LOGV(TAG_MAIN,"bSendMessageWithReplyKeyboard:FALSE");
    }
    if (configItems.alertALL && configItems.userChatIds.toDouble()>0){
      bot.sendMessageWithReplyKeyboard(configItems.userChatIds,"I am Alive :-) ", "Markdown", formulateKeyboardJson(), true);
    }
  }else{
    ESP_LOGV(TAG_MAIN,"Bot Token not yet set, I am not alive yet :-( ");
  }
  
  ///////////////////
#if defined(FLASH_LAMP_PIN)
  pinMode(FLASH_LAMP_PIN, OUTPUT);
#endif
#if defined(PIR_PIN)
  // depends on which type of PIR your are using
  if (PIR_PIN_ON)
    pinMode(PIR_PIN, INPUT_PULLDOWN); //set default incomming signal to LOW
  else
    pinMode(PIR_PIN, INPUT_PULLUP); //set default incomming signal to HIGH
#endif
#if defined(BUZZER_PIN)
  pinMode(BUZZER_PIN,OUTPUT);
#endif
  bTelegramBotInitiated=true;
ESP_LOGV(TAG_MAIN,"SETUP END: %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
}
//****************************************************************//
//**********************   The LOOP   ****************************//
//****************************************************************//
void loop() {
  ESP_LOGV(TAG_MAIN,"START LOOP *****************************************************");
  Portal.handleClient();

#if defined(PIR_PIN)
  int vPIR = digitalRead(PIR_PIN);
  /*
  Serial.print("PIR VALUE:");
  ESP_LOGV(TAG_MAIN,vPIR );
  */
  if ( configItems.motDetectOn && vPIR==PIR_PIN_ON ) {
    String result= alertTelegram("PIR Motion Detected.");
    ESP_LOGV(TAG_MAIN,"result: %s ",result);
    bMotionDetected=true;
    delay(100);
  } else{
    bMotionDetected=false;
  }
#endif
#if defined(BUZZER_PIN)
  if (bMotionDetected && configItems.useBuzzer){
    // Active Buzzer
    ESP_LOGV(TAG_MAIN,"Buzzer ON");
    digitalWrite (BUZZER_PIN, BUZZER_PIN_ON); //turn buzzer on
    delay(1000);
  }else{
    //ESP_LOGV(TAG_MAIN,"Buzzer OFF");
    digitalWrite (BUZZER_PIN, !BUZZER_PIN_ON);  //turn buzzer off
  }
#endif
#if defined(I2C_DISPLAY_ADDR)
  line1=String(WiFi.SSID());
  line2="                    ";
  line3=WiFi.localIP().toString();
  if (displayEnabled){
    ESP_LOGV(TAG_MAIN,"displayEnabled:TRUE");
    display_Textlines( line1, line2 , line3 );
  }else{
    ESP_LOGV(TAG_MAIN,"displayEnabled:FALSE");
    display.clearDisplay();
    display.display();
  }
#endif
  //////////////////////////////////////////////
  // Computer Vision Motion Detection
  if (configItems.motionDetectVC ){
    ESP_LOGV(TAG_MAIN,"CheckMotion before.");
    haveMotion = checkMotion(haveMotion , (&configItems)->frameSize ,haveMotion );
    ESP_LOGV(TAG_MAIN,"CheckMotion After.");
    if (haveMotion) {
      alertTelegram("CV Motion detected");
    }
  }
  //////////////////////////////////////////////
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    ESP_LOGV(TAG_MAIN,"bot.getUpdates() !");    
    int numNewMessages = bot.getUpdates((bot.last_message_received) + 1);
    while(numNewMessages) {
      ESP_LOGV(TAG_MAIN,"got response#1");
      handleNewMessages(numNewMessages);
      ESP_LOGV(TAG_MAIN,"got response#2");
      numNewMessages = bot.getUpdates((bot.last_message_received) + 1);
      ESP_LOGV(TAG_MAIN,"got response#3");
    }
    if (bTakePhotoTick){
      alertTelegram("time-lapse tick!");
      bTakePhotoTick=false;
    }
    /////////////////////////////////////////////
    #ifdef CAMERA_MODEL_M5STACK_PSRAM
      rtc_date_t date;
      bmm8563_getTime(&date);
      ESP_LOGV(TAG_MAIN,"Time: %d/%d/%d %02d:%02d:%-2d", date.year, date.month, date.day, date.hour, date.minute, date.second);
      ESP_LOGV(TAG_MAIN,"volt: %d mv", bat_get_voltage());
      ESP_LOGV(TAG_MAIN,"ADC: %d mv", bat_get_adc_raw());
    #endif
    /////////////////////////////////////////////
    #if defined(PIR_PIN)
      if (configItems.useDeepSleep && configItems.motDetectOn) {
        bMotionDetected=true;
        esp_sleep_enable_ext0_wakeup((gpio_num_t)PIR_PIN,PIR_PIN_ON); //1 = High, 0 = Low
        bESPMayGoToSleep=true;
        //alertTelegram("ESP is going to sleep till PIR is active.",true );
      }
    #endif
    /////////////////////////////////////////////
    if (configItems.useDeepSleep && configItems.lapseTime >0){
      bTakePhotoTick=true;
      #ifdef CAMERA_MODEL_M5STACK_PSRAM
        // esp_deep_sleep((uint64_t) configItems.lapseTime*60*uS_TO_S_FACTOR);
        // X sec later will wake up
        ESP_LOGV(TAG_MAIN,"bmm8563_setTimerIRQ");
        bmm8563_setTimerIRQ(configItems.lapseTime*60);
      #endif
      ESP_LOGV(TAG_MAIN,"esp_sleep_enable_timer_wakeup");
      //esp_deep_sleep((uint64_t) configItems.lapseTime*60*uS_TO_S_FACTOR);
      esp_sleep_enable_timer_wakeup( (uint64_t) configItems.lapseTime*60*uS_TO_S_FACTOR);
      bESPMayGoToSleep=true;
      //alertTelegram("Setup ESP32 to sleep for every " + String(configItems.lapseTime) + " minutes",true);
    }
    /////////////////////////////////////////////
    if (configItems.useDeepSleep && bESPMayGoToSleep){
      Serial.flush();
      String extraMessage;
      if (bMotionDetected){
        extraMessage="till PIR is active.";
      }
      if (bTakePhotoTick){
        extraMessage="for the next " + String(configItems.lapseTime) + " minutes.";
      }
      alertTelegram("ESP is going to sleep "+extraMessage,false);
      #ifdef CAMERA_MODEL_M5STACK_PSRAM
        // disable bat output, will wake up after 5 sec, Sleep current is 1~2μA
        ESP_LOGV(TAG_MAIN,"bat_disable_output");
        bat_disable_output();
      #endif
      ESP_LOGV(TAG_MAIN,"esp_deep_sleep_start");
      esp_wifi_stop();
      esp_deep_sleep_start();
    }
    /////////////////////////////////////////////
    Bot_lasttime = millis();
  }
  ESP_LOGV(TAG_MAIN,"END LOOP *****************************************************");
}
//****************************************************************//

////////////////////////////////////////////////////////////////////////////
#ifdef CAMERA_MODEL_M5STACK_PSRAM
void led_breathe_test() {
  for (int16_t i = 0; i < 1024; i++) {
    led_brightness(i);
    vTaskDelay(pdMS_TO_TICKS(1));
  }

  for (int16_t i = 1023; i >= 0; i--) {
    led_brightness(i);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
#endif
////////////////////////////////////////////////////////////////////////////
esp_sleep_wakeup_cause_t print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : ESP_LOGV(TAG_MAIN,"Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : ESP_LOGV(TAG_MAIN,"Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : ESP_LOGV(TAG_MAIN,"Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : ESP_LOGV(TAG_MAIN,"Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : ESP_LOGV(TAG_MAIN,"Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
  return (wakeup_reason);
}
////////////////////////////////////////////////////////////////////////////
void tick(){
  bTakePhotoTick=true;
}
////////////////////////////////////////////////////////////////////////////
void applyConfigItem (config_item* ci) {
  sensor_t * s = esp_camera_sensor_get();
  //
  s->set_pixformat(s, PIXFORMAT_JPEG );
  s->set_framesize(s, ci->frameSize);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
  s->set_hmirror(s, ci->hMirror);
  s->set_vflip(s, ci->vFlip);
  //
  //s->set_wb_mode(s,ci->set_whitebal);
  s->set_brightness(s,ci->set_brightness);
  s->set_contrast(s,ci->set_contrast);
  s->set_saturation(s,ci->set_saturation);
  s->set_quality(s,ci->jpegQuality);
  //
  ESP_LOGV(TAG_MAIN,"* > s->status.scale:%d",s->status.scale);
  ESP_LOGV(TAG_MAIN,"* > s->status.binning:%d",s->status.binning);
  ESP_LOGV(TAG_MAIN,"* > s->status.quality:%d",s->status.quality);					//0 - 63
  ESP_LOGV(TAG_MAIN,"* > s->status.brightness:%d",s->status.brightness);			//-2 - 2
  ESP_LOGV(TAG_MAIN,"* > s->status.contrast:%d",s->status.contrast);				//-2 - 2
  ESP_LOGV(TAG_MAIN,"* > s->status.saturation:%d",s->status.saturation);			//-2 - 2
  ESP_LOGV(TAG_MAIN,"* > s->status.sharpness:%d",s->status.sharpness);				//-2 - 2
  ESP_LOGV(TAG_MAIN,"* > s->status.denoise:%d",s->status.denoise);
  ESP_LOGV(TAG_MAIN,"* > s->status.special_effect:%d",s->status.special_effect);	//0 - 6
  ESP_LOGV(TAG_MAIN,"* > s->status.wb_mode:%d",s->status.wb_mode); 					//0 - 4
  ESP_LOGV(TAG_MAIN,"* > s->status.awb:%d",s->status.awb);
  ESP_LOGV(TAG_MAIN,"* > s->status.awb_gain:%d",s->status.awb_gain);
  ESP_LOGV(TAG_MAIN,"* > s->status.aec:%d",s->status.aec);
  ESP_LOGV(TAG_MAIN,"* > s->status.aec2:%d",s->status.aec2);
  ESP_LOGV(TAG_MAIN,"* > s->status.ae_level:%d",s->status.ae_level);				//-2 - 2
  ESP_LOGV(TAG_MAIN,"* > s->status.aec_value:%d",s->status.aec_value);				//0 - 1200
  ESP_LOGV(TAG_MAIN,"* > s->status.agc:%d",s->status.agc);
  ESP_LOGV(TAG_MAIN,"* > s->status.agc_gain:%d",s->status.agc_gain);				//0 - 30
  ESP_LOGV(TAG_MAIN,"* > s->status.gainceiling:%d",s->status.gainceiling);			//0 - 6
  ESP_LOGV(TAG_MAIN,"* > s->status.bpc:%d",s->status.bpc);
  ESP_LOGV(TAG_MAIN,"* > s->status.wpc:%d",s->status.wpc);
  ESP_LOGV(TAG_MAIN,"* > s->status.raw_gma:%d",s->status.raw_gma);
  ESP_LOGV(TAG_MAIN,"* > s->status.lenc:%d",s->status.lenc);
  ESP_LOGV(TAG_MAIN,"* > s->status.hmirror:%d",s->status.hmirror);
  ESP_LOGV(TAG_MAIN,"* > s->status.vflip:%d",s->status.vflip);
  ESP_LOGV(TAG_MAIN,"* > s->status.dcw:%d",s->status.dcw);
  ESP_LOGV(TAG_MAIN,"* > s->status.colorbar:%d",s->status.colorbar);
  delay(500);
  //*/
  // non configurable params:
  /*
  set_special_effect()
    0 – No Effect
    1 – Negative
    2 – Grayscale
    3 – Red Tint
    4 – Green Tint
    5 – Blue Tint
    6 – Sepia
  set_wb_mode()
    0 – Auto
    1 – Sunny
    2 – Cloudy
    3 – Office
    4 – Home
  https://github.com/espressif/esp32-camera/blob/ec14f1d6f718571d8a7d2e537e03cebcc05e0ac8/driver/include/sensor.h
  typedef struct {
    framesize_t framesize;//0 - 10
    bool scale;
    bool binning;
    uint8_t quality;//0 - 63
    int8_t brightness;//-2 - 2
    int8_t contrast;//-2 - 2
    int8_t saturation;//-2 - 2
    int8_t sharpness;//-2 - 2
    uint8_t denoise;
    uint8_t special_effect;//0 - 6
    uint8_t wb_mode;//0 - 4
    uint8_t awb;
    uint8_t awb_gain;
    uint8_t aec;
    uint8_t aec2;
    int8_t ae_level;//-2 - 2
    uint16_t aec_value;//0 - 1200
    uint8_t agc;
    uint8_t agc_gain;//0 - 30
    uint8_t gainceiling;//0 - 6
    uint8_t bpc;
    uint8_t wpc;
    uint8_t raw_gma;
    uint8_t lenc;
    uint8_t hmirror;
    uint8_t vflip;
    uint8_t dcw;
    uint8_t colorbar;
} camera_status_t;
*/
    //s->set_gain_ctrl(s, 0); // auto gain off (1 or 0)
    //s->set_exposure_ctrl(s, 0); // auto exposure off (1 or 0)
    //s->set_agc_gain(s, 0); // set gain manually (0 - 30)
    //s->set_aec_value(s, 600); // set exposure manually (0-1200)
    // s->set_brightness(s, 0); // (-2 to 2) - set brightness
    // s->set_awb_gain(s, 0); // Auto White Balance?
    // s->set_lenc(s, 0); // lens correction? (1 or 0)
    // s->set_raw_gma(s, 1); // (1 or 0)?
    // s->set_quality(s, 10); // (0 - 63)
    // s->set_whitebal(s, 1); // white balance
    // s->set_wb_mode(s, 1); // white balance mode (0 to 4)
    // s->set_aec2(s, 0); // automatic exposure sensor? (0 or 1)
    // s->set_aec_value(s, 0); // automatic exposure correction? (0-1200)
    // s->set_saturation(s, 0); // (-2 to 2)
    // s->set_hmirror(s, 0); // (0 or 1) flip horizontally
    // s->set_gainceiling(s, GAINCEILING_32X); // Image gain (GAINCEILING_x2, x4, x8, x16, x32, x64 or x128)
    // s->set_contrast(s, 0); // (-2 to 2)
    // s->set_sharpness(s, 0); // (-2 to 2)
    // s->set_colorbar(s, 0); // (0 or 1) - testcard
    // s->set_special_effect(s, 2); // 0 – No Effect,1 – Negative,2 – Grayscale,3 – Red Tint,4 – Green Tint,5 – Blue Tint,6 – Sepia
    // s->set_ae_level(s, 0); // auto exposure levels (-2 to 2)
    // s->set_bpc(s, 0); // black pixel correction
    // s->set_wpc(s, 0); // white pixel correction
    // s->set_dcw(s, 1); // downsize enable? (1 or 0)?
  //
  tkTimeLapse.detach();
  if (ci->lapseTime >0){
    tkTimeLapse.attach(  ci->lapseTime*60 , tick );
  }
#if defined(I2C_DISPLAY_ADDR)
  if (ci->screenFlip){
    display.setRotation(2);
  }else{
    display.setRotation(0);
  }
  displayEnabled=ci->screenOn;
#endif
}
