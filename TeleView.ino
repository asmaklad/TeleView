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


// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM        // Board definition Boards->ESP32 Arduino->"M5Stack Timer-CAM"
                                          //  Don't use the  Boards->M5Stack Arduino ->"M5Stack Timer CAM"
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER         // Board definition "AI Thinker ESP32-CAM"
//#define CAMERA_MODEL_TTGO_T1_CAMERA     // Board definition "ESP32 WROVER Module" or "TTGO T1"
                                        // to Have OTA Working:
                                        // tools->Patition Schema-> Minimal SPIFFS(1.9MB with OTA/190KB SPIFFS)
//#define CAMERA_MODEL_M5CAM              // Board Difinition  "AI Thinker ESP32-CAM"
//////////////////////////////////////                                          // and set Tools-> Partiton Scheme --> Huge App (3MB No OTA/1MB SPIFF)
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

//////////////////////////////////////
#include "persist.h"
boolean applyConfigItem (config_item* ci);
#include "webPages.h"
#include "ElequentVision.h"

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

char* ntpServer = "pool.ntp.org";
long  gmtOffset_sec = 0;
int   daylightOffset_sec = 0;

bool bTakePhotoTick=false;
boolean bMotionDetected=false;
boolean bESPMayGoToSleep=false;
Ticker tkTimeLapse;
int consequentChangedFrames=0;
int count_ftp = 0;
int count_ftp2 = 0;

// struct tm *startTM; you find it in webPages.h

esp_sleep_wakeup_cause_t print_wakeup_reason();

//****************************************************************//
#ifdef SD_CARD_ON
  #include "ESP32FtpServer.h"
  // MicroSD
  #include "driver/sdmmc_host.h"
  #include "driver/sdmmc_defs.h"
  #include "sdmmc_cmd.h"
  #include "esp_vfs_fat.h"
  #include <SD_MMC.h>
  FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP32FtpServer.h to see ftp verbose on serial
  TaskHandle_t FtpTask;
  int diskspeed = 0;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //
  // FtpTask runs on cpu 0 to respond to ftp
  //
  void codeForFtpTask( void * parameter )
  {
    uint32_t ulNotifiedValue;
    Serial.print("ftp, core ");  Serial.print(xPortGetCoreID());
    Serial.print(", priority = "); Serial.println(uxTaskPriorityGet(NULL));

    for (;;) {
      ftpSrv.handleFTP();
      count_ftp++;
      delay(1);

    }
  }
  //****************************************************************//
  static esp_err_t init_sdcard()
  {

    //pinMode(12, PULLUP);
    pinMode(13, PULLUP);
    //pinMode(4, OUTPUT);

    if(!SD_MMC.begin()){
      Serial.println("SD Card Mount Failed");
    }else{
      Serial.print("SD_MMC Begin: ");
    }
    //pinMode(13, PULLDOWN);
    //pinMode(13, INPUT_PULLDOWN);
  }
#endif



//****************************************************************//
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disbale the burnout reset
  Serial.begin(115200);
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
  Serial.print("Compile Date:");
  Serial.println(compileDate);
  Serial.print("Compile Time:");
  Serial.println(compileTime);
  Serial.print("Compile Compiler:");
  Serial.println(compileCompiler);
  Serial.println();
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
  Serial.print("frame_size: ");
  Serial.println( String(resolutions[config.frame_size][0]+":"+resolutions[config.frame_size][1]) );
  Serial.print("jpeg_quality: ");
  Serial.println(config.jpeg_quality);
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
  bCameraInitiated=true;
  ////////////////////////////
  configItems.frameSize=config.frame_size;
  keyboardJson=formulateKeyboardJson();
  configItems=loadConfiguration();
  applyConfigItem(&configItems);
  Serial.println(printConfiguration(&configItems,""));
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
  Portal.host().on("/",rootPage);
  Portal.host().on("/delete",deletePage);
  Portal.host().on("/capture",capturePage);
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
  //MDNS
  if (!MDNS.begin(configItems.deviceName.c_str())) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  // Add service to MDNS-SD
  // With applying AutoConnect, the MDNS service must be started after
  // establishing a WiFi connection.
  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS responder started");
  /////////////////////////////
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }else{
    Serial.println("Portal not startd");
  }
  Serial.println("HTTP server started");

  Serial.println(WiFi.SSID());
  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);
  //
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("WiFi MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  //
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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
      Serial.println("Failed to obtain time");
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
  botClient.setInsecure();
  //botClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  bot.updateToken(configItems.botTTelegram);
  //bot.sendMessage(configItems.adminChatIds, "I am Alive!!", "");
  bot.sendMessageWithReplyKeyboard(configItems.adminChatIds, "I am Alive!!", "Markdown", formulateKeyboardJson(), true);
  if (configItems.alertALL && configItems.userChatIds.toDouble()>0){
    bot.sendMessageWithReplyKeyboard(configItems.userChatIds, "I am Alive!!", "Markdown", formulateKeyboardJson(), true);
  }
  Serial.println("I am Alive!!");
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

#ifdef SD_CARD_ON
  //plm print_ram();  delay(2000);
  //****************************************************************//

  if (configItems.ftpServerOn) {
    Serial.println("Starting ftp ...");
    init_sdcard();
    ftpSrv.begin(configItems.ftpUser, configItems.ftpPass);
    xTaskCreatePinnedToCore(
      codeForFtpTask,
      "FtpTask",
      4096,       // heap
      NULL,
      4,          // prio higher than 1
      &FtpTask,
      0);         // on cpu 0

    delay(20);
  }

#endif
}
//****************************************************************//
//**********************   The LOOP   ****************************//
//****************************************************************//
void loop() {

  Portal.handleClient();
#if defined(PIR_PIN)
  int vPIR = digitalRead(PIR_PIN);
  /*
  Serial.print("PIR VALUE:");
  Serial.println(vPIR );
  */
  if ( configItems.motDetectOn && vPIR==PIR_PIN_ON ) {
    String result= alertTelegram("PIR Motion Detected.");
    Serial.println("result: "+result);
    bMotionDetected=true;
    delay(100);
  } else{
    bMotionDetected=false;
  }
#endif
#if defined(BUZZER_PIN)
  if (bMotionDetected && configItems.useBuzzer){
    // Active Buzzer
    Serial.println("Buzzer ON");
    digitalWrite (BUZZER_PIN, BUZZER_PIN_ON); //turn buzzer on
    delay(1000);
  }else{
    //Serial.println("Buzzer OFF");
    digitalWrite (BUZZER_PIN, !BUZZER_PIN_ON);  //turn buzzer off
  }
#endif
#if defined(I2C_DISPLAY_ADDR)
  line1=String(WiFi.SSID());
  line2="                    ";
  line3=WiFi.localIP().toString();
  if (displayEnabled){
    display_Textlines( line1, line2 , line3 );
  }else{
    display.clearDisplay();
    display.display();
  }
#endif
  //////////////////////////////////////////////
  // Computer Vision Motion Detection
  if (configItems.motionDetectVC){
    // https://github.com/eloquentarduino/EloquentArduino/blob/1.1.8/examples/ESP32CameraNaiveMotionDetection/ESP32CameraNaiveMotionDetection.ino
    // https://eloquentarduino.github.io/2020/05/easier-faster-pure-video-esp32-cam-motion-detection/?utm_source=old_version
    Serial.println(setup_camera(FRAMESIZE_QVGA) ? "OK" : "ERR INIT");
    if (!capture_still()) {
        Serial.println("Failed capture");
        delay(3000);
        return;
    }
    // restore origional camera status
    applyConfigItem(&configItems);
    if (motion_detect()) {
        consequentChangedFrames++;
        if (consequentChangedFrames>2) { 
          alertTelegram("CV Motion detected");
          bMotionDetected=true;
          consequentChangedFrames=0;
        }
        delay(100);
    }else{
      consequentChangedFrames=0;
    }
    update_frame();
  }
  //////////////////////////////////////////////
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates((bot.last_message_received) + 1);
    while(numNewMessages) {
      Serial.println("got response#1");
      handleNewMessages(numNewMessages);
      Serial.println("got response#2");
      numNewMessages = bot.getUpdates((bot.last_message_received) + 1);
      Serial.println("got response#3");
    }
    if (bTakePhotoTick){
      alertTelegram("time-lapse tick!");
      bTakePhotoTick=false;
    }
    /////////////////////////////////////////////
    #ifdef CAMERA_MODEL_M5STACK_PSRAM
      rtc_date_t date;
      bmm8563_getTime(&date);
      Serial.printf("Time: %d/%d/%d %02d:%02d:%-2d\r\n", date.year, date.month, date.day, date.hour, date.minute, date.second);
      Serial.printf("volt: %d mv\r\n", bat_get_voltage());
      Serial.printf("ADC: %d mv\r\n", bat_get_adc_raw());
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
        Serial.println("bmm8563_setTimerIRQ");
        bmm8563_setTimerIRQ(configItems.lapseTime*60);
      #endif
      Serial.println("esp_sleep_enable_timer_wakeup");
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
        Serial.println("bat_disable_output");
        bat_disable_output();
      #endif
      Serial.println("esp_deep_sleep_start");
      esp_wifi_stop();
      esp_deep_sleep_start();
    }
    /////////////////////////////////////////////
    Bot_lasttime = millis();
  }
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
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
  return (wakeup_reason);
}

////////////////////////////////////////////////////////////////////////////
void tick(){
  bTakePhotoTick=true;
}
////////////////////////////////////////////////////////////////////////////
boolean applyConfigItem (config_item* ci) {
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
  Serial.printf("* > s->status.scale:%d\n",s->status.scale);
  Serial.printf("* > s->status.binning:%d\n",s->status.binning);
  Serial.printf("* > s->status.quality:%d\n",s->status.quality);					//0 - 63
  Serial.printf("* > s->status.brightness:%d\n",s->status.brightness);			//-2 - 2
  Serial.printf("* > s->status.contrast:%d\n",s->status.contrast);				//-2 - 2
  Serial.printf("* > s->status.saturation:%d\n",s->status.saturation);			//-2 - 2
  Serial.printf("* > s->status.sharpness:%d\n",s->status.sharpness);				//-2 - 2
  Serial.printf("* > s->status.denoise:%d\n",s->status.denoise);
  Serial.printf("* > s->status.special_effect:%d\n",s->status.special_effect);	//0 - 6
  Serial.printf("* > s->status.wb_mode:%d\n",s->status.wb_mode); 					//0 - 4
  Serial.printf("* > s->status.awb:%d\n",s->status.awb);
  Serial.printf("* > s->status.awb_gain:%d\n",s->status.awb_gain);
  Serial.printf("* > s->status.aec:%d\n",s->status.aec);
  Serial.printf("* > s->status.aec2:%d\n",s->status.aec2);
  Serial.printf("* > s->status.ae_level:%d\n",s->status.ae_level);				//-2 - 2
  Serial.printf("* > s->status.aec_value:%d\n",s->status.aec_value);				//0 - 1200
  Serial.printf("* > s->status.agc:%d\n",s->status.agc);
  Serial.printf("* > s->status.agc_gain:%d\n",s->status.agc_gain);				//0 - 30
  Serial.printf("* > s->status.gainceiling:%d\n",s->status.gainceiling);			//0 - 6
  Serial.printf("* > s->status.bpc:%d\n",s->status.bpc);
  Serial.printf("* > s->status.wpc:%d\n",s->status.wpc);
  Serial.printf("* > s->status.raw_gma:%d\n",s->status.raw_gma);
  Serial.printf("* > s->status.lenc:%d\n",s->status.lenc);
  Serial.printf("* > s->status.hmirror:%d\n",s->status.hmirror);
  Serial.printf("* > s->status.vflip:%d\n",s->status.vflip);
  Serial.printf("* > s->status.dcw:%d\n",s->status.dcw);
  Serial.printf("* > s->status.colorbar:%d\n",s->status.colorbar);
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
    // s->set_special_effect(s, 0);
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
