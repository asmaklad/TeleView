
#ifndef PERSIST_H
#define PERSIST_H

#include "esp_camera.h"

#include <Preferences.h>
#include <Arduino.h>  // for type definitions


static const char* TAG_PS = "PERSIST";
#include "esp_log.h"

Preferences prefs;

time_t timeOfLastPhoto=0;
/*
look in  resolution_info_t resolution[FRAMESIZE_INVALID]
in https://github.com/espressif/esp32-camera/blob/master/driver/sensor.c
in https://github.com/espressif/esp32-camera/blob/master/driver/include/sensor.h framesize_t
*/
String resolutions[][13]={ 
  // OV2640 2MP
  {"96X96",    "96x96" },				      //    FRAMESIZE_96X96,    // 96x96
  {"QQVGA",    "160x120" },           //    FRAMESIZE_QQVGA,    // 160x120
  {"QCIF",     "176x144" },           //    FRAMESIZE_QCIF,     // 176x144
  {"HQVGA",    "240x176" },           //    FRAMESIZE_HQVGA,    // 240x176
  {"240X240",  "240x240" },           //    FRAMESIZE_240X240,  // 240x240
  {"QVGA",     "320x240" },           //    FRAMESIZE_QVGA,     // 320x240
  {"CIF",      "400x296" },           //    FRAMESIZE_CIF,      // 400x296
  {"HVGA",     "480x320" },           //    FRAMESIZE_HVGA,     // 480x320
  {"VGA",      "640x480" },           //    FRAMESIZE_VGA,      // 640x480
  {"SVGA",     "800x600" },           //    FRAMESIZE_SVGA,     // 800x600
  {"XGA",      "1024x768" },          //    FRAMESIZE_XGA,      // 1024x768
  {"HD",       "1280x720" },          //    FRAMESIZE_HD,       // 1280x720
  {"SXGA",     "1280x1024" },         //    FRAMESIZE_SXGA,     // 1280x1024
  {"UXGA",     "1600x1200" },         //    FRAMESIZE_UXGA,     // 1600x1200
  // OV3660 3MP
  {"FHD",        "1920x1080" },       //    FRAMESIZE_FHD,     // 1600x1200
  {"PortraitHD", "720x1280" },        //    FRAMESIZE_P_HD,     // 1600x1200
  {"Portrait3MP","864x1536" },        //    FRAMESIZE_P_3MP,     // 1600x1200
  {"QXGA",       "2048x1536" }        //    FRAMESIZE_QXGA,     // 1600x1200  
};
///////////////////////////////////////////////////////////
typedef struct {
  const char* zone;
  const char* ntpServer;
  int8_t      tzoff;
} Timezone_t;
///////////////////////////////////////////////////////////
static const Timezone_t TZ[] = {
    { "Europe/London", "europe.pool.ntp.org", 0 },
    { "Europe/Berlin", "europe.pool.ntp.org", 1 },
    { "Europe/Helsinki", "europe.pool.ntp.org", 2 },
    { "Europe/Moscow", "europe.pool.ntp.org", 3 },
    { "Asia/Dubai", "asia.pool.ntp.org", 4 },
    { "Asia/Karachi", "asia.pool.ntp.org", 5 },
    { "Asia/Dhaka", "asia.pool.ntp.org", 6 },
    { "Asia/Jakarta", "asia.pool.ntp.org", 7 },
    { "Asia/Manila", "asia.pool.ntp.org", 8 },
    { "Asia/Tokyo", "asia.pool.ntp.org", 9 },
    { "Australia/Brisbane", "oceania.pool.ntp.org", 10 },
    { "Pacific/Noumea", "oceania.pool.ntp.org", 11 },
    { "Pacific/Auckland", "oceania.pool.ntp.org", 12 },
    { "Atlantic/Azores", "europe.pool.ntp.org", -1 },
    { "America/Noronha", "south-america.pool.ntp.org", -2 },
    { "America/Araguaina", "south-america.pool.ntp.org", -3 },
    { "America/Blanc-Sablon", "north-america.pool.ntp.org", -4},
    { "America/New_York", "north-america.pool.ntp.org", -5 },
    { "America/Chicago", "north-america.pool.ntp.org", -6 },
    { "America/Denver", "north-america.pool.ntp.org", -7 },
    { "America/Los_Angeles", "north-america.pool.ntp.org", -8 },
    { "America/Anchorage", "north-america.pool.ntp.org", -9 },
    { "Pacific/Honolulu", "north-america.pool.ntp.org", -10 },
    { "Pacific/Samoa", "oceania.pool.ntp.org", -11 }
  };
////////////////////////////////////////////////////////////////////////////
int matchResolutionText(String text){
  int result=-1;
  ESP_LOGV(TAG_PS,"TESTX:%s",text);
  for (int i=0;i<=maxRes;i++){
    ESP_LOGV( TAG_PS,"compareTo:/%s",resolutions[i][0] );
    if ((text.compareTo(String("/"+resolutions[i][0])))==0){
      result=i;
      break;
    }
  }
  if (result==-1){
    ESP_LOGV(TAG_PS,"matchResolutionText:No Match Found!");
  }
  return( result);
}
////////////////////////////////////////////////////////////////////////////
struct config_item {
  // [:ADD BOOLEAN HERE:]#1
  boolean useFlash;
  boolean hMirror;
  boolean vFlip;
  boolean sendEmail;
  boolean motionDetectVC;
  boolean alertALL;
  boolean saveToSD;
  boolean useDeepSleep;
  boolean useBuzzer;
  boolean screenFlip;
  boolean screenOn;
  boolean motDetectOn;
  boolean sMTPTLS;
  boolean webCaptureOn;
  // [:ADD INT HERE:]#1
  int cvIntervalSec;
  int cvChangePercent;
  int set_whitebal;
  int set_saturation;
  int set_contrast;
  int set_brightness;
  int jpegQuality;
  int sMTPPort;
  int lapseTime;
  framesize_t frameSize;
  // [:ADD STRING HERE:]#1
  String version;
  String sMTPPassword;
  String sMTPUsername;
  String sMTPServer;
  String userEmail;
  String adminEmail;
  String deviceName;
  String botTTelegram;
  String adminChatIds;
  String userChatIds;
  String timeZone;
} configItems {
  // [:ADD BOOLEAN HERE:]#2
  .useFlash = true,
  .hMirror = true, 
  .vFlip = true,
  .sendEmail = false,
  .motionDetectVC = false,
  .alertALL = false,
  .saveToSD = true,
  .useDeepSleep = false,
  .useBuzzer = true,
  .screenFlip = true,
  .screenOn=true,
  .motDetectOn=false,
  .sMTPTLS = false,
  .webCaptureOn=true,
  // [:ADD INT HERE:]#2
  .cvIntervalSec = 500,
  .cvChangePercent = 10,
  .set_whitebal = 1,
  .set_saturation = 0,
  .set_contrast = 0,
  .set_brightness = 0,
  .jpegQuality = 12,
  .sMTPPort = 0,
  .lapseTime=60,  
  .frameSize = FRAMESIZE_CIF,
  // [:ADD STRING HERE:]#2
  .version="v1.10",
  .sMTPPassword = "",
  .sMTPUsername = "",
  .sMTPServer = "",
  .userEmail = "",
  .adminEmail = "",  
  .deviceName = String("TeleView"),
  .botTTelegram = String("0123456789"),
  .adminChatIds = String("0123456789"),
  .userChatIds  = String("0123456789"),
  .timeZone="Europe/Berlin"  
};

////////////////////////////////////////////////////////////////////////////
config_item loadConfiguration();
boolean saveConfiguration(config_item* ci);
void deleteConfiguration();
String printConfiguration(config_item* ci,char* prefixC="",char* suffixC="\n",char* sep="|");
////////////////////////////////////////////////////////////////////////////
void deleteConfiguration(){
  if (!prefs.begin("settings",false)) // False=RW
  {
    ESP_LOGV(TAG_PS,"failed find settings prefrences! returning default."); 
    prefs.end();
    return ;
  }else{
    ESP_LOGV(TAG_PS,"Deleting all settings");
    prefs.clear();
    prefs.end();
  }
}
////////////////////////////////////////////////////////////////////////////
config_item loadConfiguration() {
  config_item ci ;
  if (!prefs.begin("settings",true))
  {
    // Write Default
    saveConfiguration (&configItems);    
    ESP_LOGV(TAG_PS,"failed find settings prefrences! returning default.");
    prefs.end();
    return(configItems);
  }else{
    ESP_LOGV(TAG_PS,"found settings prefrences.");
    // [:ADD BOOLEAN HERE:]#4
    ci.useFlash = prefs.getBool("useFlash",configItems.useFlash);
    ci.hMirror = prefs.getBool("hMirror",configItems.hMirror);
    ci.vFlip = prefs.getBool("vFlip",configItems.vFlip);
    ci.sMTPTLS = prefs.getBool("sMTPTLS",configItems.sMTPTLS);
    ci.sendEmail = prefs.getBool("sendEmail",configItems.sendEmail);
    ci.motionDetectVC = prefs.getBool("motionDetectVC",configItems.motionDetectVC);
    ci.alertALL = prefs.getBool("alertALL",configItems.alertALL);
    ci.saveToSD = prefs.getBool("saveToSD",configItems.saveToSD);
    ci.useDeepSleep = prefs.getBool("useDeepSleep",configItems.useDeepSleep);
    ci.useBuzzer = prefs.getBool("useBuzzer",configItems.useBuzzer);
    ci.screenFlip = prefs.getBool("screenFlip",configItems.screenFlip);
    ci.screenFlip = prefs.getBool("screenFlip",configItems.screenFlip);
    ci.screenOn = prefs.getBool("screenOn",configItems.screenOn);
    ci.motDetectOn = prefs.getBool("motDetectOn",configItems.motDetectOn);
    ci.webCaptureOn=prefs.getBool("webCaptureOn",configItems.webCaptureOn);
    // [:ADD INT HERE:]#4
    ci.cvIntervalSec = prefs.getInt("cvIntervalSec",configItems.cvIntervalSec);
    ci.cvChangePercent = prefs.getInt("cvChangePercent",configItems.cvChangePercent);
    ci.sMTPPort = prefs.getInt("sMTPPort",configItems.sMTPPort);
    ci.set_whitebal = prefs.getInt("set_whitebal",configItems.set_whitebal);
    ci.set_saturation = prefs.getInt("set_saturation",configItems.set_saturation);
    ci.set_contrast = prefs.getInt("set_contrast",configItems.set_contrast);
    ci.set_brightness = prefs.getInt("set_brightness",configItems.set_brightness);
    ci.jpegQuality = prefs.getInt("jpegQuality",configItems.jpegQuality);
    ci.lapseTime=prefs.getInt("lapseTime",configItems.lapseTime);
    ci.frameSize = (framesize_t) prefs.getUInt("frameSize",configItems.frameSize);    
    // [:ADD STRING HERE:]#5
    ci.sMTPPassword = prefs.getString("sMTPPassword",configItems.sMTPPassword);
    ci.sMTPUsername = prefs.getString("sMTPUsername",configItems.sMTPUsername);
    ci.sMTPServer = prefs.getString("sMTPServer",configItems.sMTPServer);
    ci.userEmail = prefs.getString("userEmail",configItems.userEmail);
    ci.adminEmail = prefs.getString("adminEmail",configItems.adminEmail);
    ci.deviceName=prefs.getString("deviceName",configItems.deviceName);
    ci.botTTelegram=prefs.getString("botTTelegram",configItems.botTTelegram);
    ci.adminChatIds=prefs.getString("adminChatIds",configItems.adminChatIds);
    ci.userChatIds=prefs.getString("userChatIds",configItems.userChatIds);
    ci.timeZone=prefs.getString("timeZone",configItems.timeZone);
    ci.version=prefs.getString("version",configItems.version);
    
    prefs.end();
  }
  return(ci);
}
////////////////////////////////////////////////////////////////////////////
boolean saveConfiguration(config_item* ci) {
  ESP_LOGV(TAG_PS,"saveConfiguration:EEPROM Write:start");
  boolean bDirty=false;
  if (!prefs.begin("settings",false)){  //false=RW , true=RO
    ESP_LOGV(TAG_PS,"ERROR: failed to load settings for RW."); 
    prefs.end();
    return (false);
  }else{
    // [:ADD BOOLEAN HERE:]#5
    prefs.putBool("useFlash",ci->useFlash);
    prefs.putBool("hMirror",ci->hMirror);
    prefs.putBool("vFlip",ci->vFlip);
    prefs.putBool("sendEmail",ci->sendEmail);
    prefs.putBool("motionDetectVC",ci->motionDetectVC);
    prefs.putBool("alertALL",ci->alertALL);
    prefs.putBool("saveToSD",ci->saveToSD);
    prefs.putBool("useDeepSleep",ci->useDeepSleep);
    prefs.putBool("useBuzzer",ci->useBuzzer);
    prefs.putBool("screenFlip",ci->screenFlip);
    prefs.putBool("screenOn",ci->screenOn);
    prefs.putBool("motDetectOn",ci->motDetectOn);
    prefs.putBool("webCaptureOn",ci->webCaptureOn);
    prefs.putBool("sMTPTLS",ci->sMTPTLS);
    // [:ADD INT HERE:]#5
    prefs.putInt("cvIntervalSec",ci->cvIntervalSec);
    prefs.putInt("cvChangePercent",ci->cvChangePercent);
    prefs.putInt("set_whitebal",ci->set_whitebal);
    prefs.putInt("set_saturation",ci->set_saturation); 
    prefs.putInt("set_contrast",ci->set_contrast);
    prefs.putInt("set_brightness",ci->set_brightness);
    prefs.putInt("jpegQuality",ci->jpegQuality);
    prefs.putInt("sMTPPort",ci->sMTPPort);
    prefs.putInt("lapseTime",ci->lapseTime);
    prefs.putUInt("frameSize", (unsigned int) (ci->frameSize) ); 
    // [:ADD STRING HERE:]#5
    prefs.putString("version",ci->version); 
    prefs.putString("sMTPPassword",ci->sMTPPassword); 
    prefs.putString("sMTPUsername",ci->sMTPUsername); 
    prefs.putString("sMTPServer",ci->sMTPServer);
    prefs.putString("userEmail",ci->userEmail);
    prefs.putString("adminEmail",ci->adminEmail);
    prefs.putString("deviceName",ci->deviceName); 
    prefs.putString("botTTelegram",ci->botTTelegram);
    prefs.putString("adminChatIds",ci->adminChatIds);
    prefs.putString("userChatIds",ci->userChatIds);
    prefs.putString("timeZone",ci->timeZone);
    prefs.end();
  }
  prefs.end();
  ESP_LOGV(TAG_PS,"saveConfiguration:EEPROM Write:End");
  return(bDirty);
}

////////////////////////////////////////////////////////////////////////////
String printConfiguration(config_item* ci,char* prefixC,char* suffixC,char* sep) {
  String result = "";
  String prefix=String(prefixC);
  String suffix=String(suffixC);
  //////
  result += prefix+"Local IP:"+sep+"";
  result += "<a href='http://" + WiFi.localIP().toString() +"'>" + WiFi.localIP().toString() +"</a>" + suffix;
  result += prefix+"Local URL:"+sep+"";
  result += "<a href='http://" + ci->deviceName +".local'>" + ci->deviceName +".local</a>" + suffix;
  result += "<pre>\n";
  
  result += prefix+" *Attribute*     "+sep+" *Value* "+suffix;
  result += prefix+"Device Name      "+sep+"";
  result += ci->deviceName + suffix;
  result += prefix+"Version          "+sep+"";
  result += ci->version + suffix;
  result += prefix+"WIFI SSID        "+sep+"";
  result += WiFi.SSID() + suffix;
  
  result += prefix+"PSRAM ?          "+sep+"";
  result += (psramFound() ? String("true") : String("false")) +suffix;
  result += prefix+"PSRAM SIZE       "+sep+"";
  result += ESP.getPsramSize() +suffix;
  result += prefix+"Sketch MD5 "+sep+"";
  result += ESP.getSketchMD5() +suffix;
  result += prefix+"compileDate      "+sep+"";
  result += compileDate +suffix;
  result += prefix+"compileTime      "+sep+"";
  
  result += compileTime +suffix;
  result += prefix+"compileCompiler  "+sep+"";
  result += compileCompiler +suffix;
  
  result += prefix+"Chip Model       "+sep+"";
  result += ESP.getChipModel()  +suffix;
  
  result += prefix+"Chip Revision    "+sep+"";
  result += ESP.getChipRevision()  +suffix;
  
  result += prefix+"Chip Cores       "+sep+"";
  result += ESP.getChipCores()  +suffix;
  //////////////////////////////////////////////
  struct tm *tm;
  time_t  t;
  char    dateTime[100];
  t = time(NULL);
  tm = localtime(&t);

  sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d",
    tm->tm_year + 1900, tm->tm_mon+1 , tm->tm_mday,
    tm->tm_hour, tm->tm_min, tm->tm_sec);

  result += prefix+"Current Time     "+sep+"";
  result += String(dateTime)  +suffix;
  //////////////////////////////////////////////
  
#if defined(IS_THERE_A_FLASH)
  result += prefix+"useFlash         "+sep+"";
  result += (ci->useFlash ? String("true") : String("false")) +suffix;
#endif
#if defined(I2C_DISPLAY_ADDR)
  result += prefix+"screenOn         "+sep+"";
  result += (ci->screenOn ? String("true") : String("false")) +suffix;
  result += prefix+"screenFlip       "+sep+"";
  result += (ci->screenFlip ? String("true") : String("false")) +suffix;
#endif
#if defined(PIR_PIN)
  result += prefix+"motDetectOn      "+sep+"";
  result += (ci->motDetectOn ? String("true") : String("false")) + suffix;
#endif
#if defined(SD_CARD_ON)
  result += prefix+"saveToSD         "+sep+"";
  result += (ci->saveToSD ? String("true") : String("false"))  + suffix;
#endif
#if defined(BUZZER_PIN)
  result += prefix+"useBuzzer        "+sep+"";
  result += (ci->useBuzzer ? String("true") : String("false"))  + suffix;
#endif
  result += prefix+"hMirror          "+sep+"";
  result += (ci->hMirror ? String("true") : String("false")) + suffix;
  result += prefix+"vFlip            "+sep+"";
  result += (ci->vFlip ? String("true") : String("false"))  + suffix;
  //result += prefix+"set_whitebal     "+sep+"";
  //result += String(ci->set_whitebal)  + suffix;
  result += prefix+"set_saturation   "+sep+"";
  result += String(ci->set_saturation)  + suffix;
  result += prefix+"set_contrast     "+sep+"";
  result += String(ci->set_contrast)  + suffix;
  result += prefix+"set_brightness   "+sep+"";
  result += String(ci->set_brightness)  + suffix;
  result += prefix+"jpegQuality      "+sep+"";
  result += String(ci->jpegQuality)  + suffix;
  result += prefix+"sMTPTLS          "+sep+"";
  result += (ci->sMTPTLS ? String("true") : String("false"))  + suffix;
  result += prefix+"sMTPPort         "+sep+"";
  result += String(ci->sMTPPort)  + suffix;
  //result += prefix+"sMTPPassword     "+sep+"";
  //result += String(ci->sMTPPassword)  + suffix;
  result += prefix+"sMTPUsername     "+sep+"";
  result += String(ci->sMTPUsername)  + suffix;
  result += prefix+"sMTPServer       "+sep+"";
  result += String(ci->sMTPServer)  + suffix;
  result += prefix+"userEmail        "+sep+"";
  result += String(ci->userEmail)  + suffix;
  result += prefix+"adminEmail       "+sep+"";
  result += String(ci->adminEmail)  + suffix;
  result += prefix+"sendEmail        "+sep+"";
  // [:ADD BOOLEAN HERE:]#6
  // [:ADD INT HERE:]#6
  // [:ADD STRING HERE:]#6
  result += (ci->sendEmail ? String("true") : String("false"))  + suffix;
  result += prefix+"motionDetectCV   "+sep+"";
  result += (ci->motionDetectVC ? String("true") : String("false"))  + suffix;
  result += prefix+"cvIntervalSec    "+sep+"";
  result += String(ci->cvIntervalSec)  +" ms"+ suffix;
  result += prefix+"cvChangePercent  "+sep+"";
  result += String(ci->cvChangePercent)  +" %"+ suffix;
  result += prefix+"alertALL         "+sep+"";
  result += (ci->alertALL ? String("true") : String("false"))  + suffix;
  result += prefix+"useDeepSleep     "+sep+"";
  result += (ci->useDeepSleep ? String("true") : String("false"))  + suffix;
  result += prefix+"webCaptureOn     "+sep+"";
  result += (ci->webCaptureOn ? String("true") : String("false")) + suffix;
  result += prefix+"frameSize        "+sep+"";
  
  ESP_LOGV(TAG_PS,"printConfiguration#10");
  result += String((unsigned int) ci->frameSize) + ",";
  ESP_LOGV(TAG_PS,"printConfiguration:ci->frameSize: %d", ci->frameSize);

  ci->frameSize = (ci->frameSize<0? (framesize_t) 0:ci->frameSize);
  result += resolutions[((unsigned int) ci->frameSize)][0] + ",";
  result += resolutions[((int) ci->frameSize)][1] +suffix;
  //result += prefix+"botTTelegram   "+sep+"";
  //result += ci->botTTelegram+suffix;
  result += prefix+"adminChatIds     "+sep+"";
  result += ci->adminChatIds+suffix;
  result += prefix+"userChatIds      "+sep+"";
  result += ci->userChatIds+suffix;
  result += prefix+"lapseTime        "+sep+"";
  result += String(ci->lapseTime)+suffix;
  result += prefix+"timeZone         "+sep+"";
  result += ci->timeZone+suffix;
  ESP_LOGV(TAG_PS,"printConfiguration#11");
  /*
  struct tm *tm;
  time_t  t;
  char    dateTime[100];
  */
  result += prefix+"Last Photo taken "+sep+"";
  if (timeOfLastPhoto>0){
    t = time(NULL)-timeOfLastPhoto;
    tm = localtime(&t);
    sprintf(dateTime, "%02d Y %02d Mon %02d D,%02d H:%02d Min:%02d S",
      tm->tm_year + 1900-1970, tm->tm_mon , tm->tm_mday-1,
      tm->tm_hour, tm->tm_min, tm->tm_sec);
    result += String(dateTime)+suffix;
  }else{
    result += "UNKNOWN" +suffix;
  }
  result += "</pre>";
  ESP_LOGV(TAG_PS,"printConfiguration#12");
  return (result);
}
////////////////////////////////////////////////////////////////////////////
#endif // #PERSIST_H