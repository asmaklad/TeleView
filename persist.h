
#ifndef PERSIST_H
#define PERSIST_H

#include "esp_camera.h"

#include <Preferences.h>
#include <Arduino.h>  // for type definitions

Preferences prefs;

time_t timeOfLastPhoto=0;

String resolutions[][13]={ 
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
  {"UXGA",     "1600x1200" }          //    FRAMESIZE_UXGA,     // 1600x1200
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
  Serial.println("TESTX:"+text);
  int maxRes=FRAMESIZE_UXGA;
  for (int i=0;i<=maxRes;i++){
    Serial.println("compareTo:"+String(resolutions[i][0]));
    if ((text.compareTo(String(resolutions[i][0])))==0){
      result=i;
      break;
    }
  }
  if (result==-1){
    Serial.println("matchResolutionText:No Match Found!");
  }
  return( result);
}
////////////////////////////////////////////////////////////////////////////
struct config_item {
  boolean useFlash;
  framesize_t frameSize;
  boolean hMirror;
  boolean vFlip;
  boolean screenFlip;
  boolean screenOn;
  boolean motDetectOn;
  String deviceName;
  String botTTelegram;
  String adminChatIds;
  String userChatIds;
  int lapseTime;
  String timeZone;
  boolean webCaptureOn;
  // TODO
  /* 
   *  useDeepSleep : ON/OFF uses Deep sleep whenever MotionDetect or TimeLapse is enabled
   *    Usefull when battery operated.
   *  buzzer : ON/OFF  on motion Detect.
   *  jpeg_Qualtiy: value 
   *  WhiteBalance: value 
   *  Brightness: value 
   *  save to SD: ON/OFF whenever SD available 
   *  Alert all : ON/OFF for MotionDetect/timelapse to all chatId's not just Admin
   *    user_chat_id : can's issue commands to the bot.
   *  /sendVideo ?
   *  /sendAudio ?
   *  motion detect by Image not PIR  ON/OFF
   *    face detect : ON/OFF and face recognize then add to Photo Caption.
   *  send-email: email,server,token..etc
   *  motionDetect Active times: define times where motion detection is enabled.
   *  send photo at: sunrise, noon, afternoon, late afternoon, sunset. with offset time option.
   *    Requires Geo Location info (send location telegram message to the bot), 
   *    Requires NTP syncing. uses time zone ...etc.
   *  integrate MQTT: on all events ,send an MQTT message
  */
} configItems {
  .useFlash = true,
  .frameSize = FRAMESIZE_CIF,
  .hMirror = true,
  .vFlip = true,
  .screenFlip = true,
  .screenOn=true,
  .motDetectOn=true,
  .deviceName = String("TeleView"),
  .botTTelegram = String("0123456789"),
  .adminChatIds = String("0123456789"),
  .userChatIds  = String("0123456789"),
  .lapseTime=60,
  .timeZone="Europe/Berlin",
  .webCaptureOn=true
} ;
////////////////////////////////////////////////////////////////////////////
config_item loadConfiguration();
boolean saveConfiguration(config_item* ci);
void deleteConfiguration();
String printConfiguration(config_item* ci,char* prefixC="",char* suffixC="\n",char* sep="|");
////////////////////////////////////////////////////////////////////////////
void deleteConfiguration(){
  if (!prefs.begin("settings",true))
  {
    Serial.println("failed find settings prefrences! returning default."); 
    prefs.end();
    return ;
  }else{
    Serial.println("Deleting all settings");
    prefs.clear();
    prefs.remove("useFlash");
    prefs.remove("frameSize");
    prefs.remove("hMirror");
    prefs.remove("vFlip");
    prefs.remove("screenFlip");
    prefs.remove("screenFlip");
    prefs.remove("screenOn");
    prefs.remove("motDetectOn");
    prefs.remove("webCaptureOn");
    ////////////////////////
    prefs.remove("deviceName");
    prefs.remove("botTTelegram");
    prefs.remove("adminChatIds");
    prefs.remove("userChatIds");
    prefs.remove("lapseTime");
    prefs.remove("timeZone");
    prefs.end();
  }
}
////////////////////////////////////////////////////////////////////////////
config_item loadConfiguration() {
  config_item ci ;
  if (!prefs.begin("settings",true))
  {
    Serial.println("failed find settings prefrences! returning default."); 
    prefs.end();
    return(configItems);
  }else{
    Serial.println("found settings prefrences.");    
    //////////////////////////////////////////////
    ci.useFlash = prefs.getBool("useFlash",configItems.useFlash);
    ci.frameSize = (framesize_t) prefs.getUInt("frameSize",configItems.frameSize);
    ci.hMirror = prefs.getBool("hMirror",configItems.hMirror);
    ci.vFlip = prefs.getBool("vFlip",configItems.vFlip);
    ci.screenFlip = prefs.getBool("screenFlip",configItems.screenFlip);
    ci.screenFlip = prefs.getBool("screenFlip",configItems.screenFlip);
    ci.screenOn = prefs.getBool("screenOn",configItems.screenOn);
    ci.motDetectOn = prefs.getBool("motDetectOn",configItems.motDetectOn);
    ci.lapseTime=prefs.getInt("lapseTime",configItems.lapseTime);
    ci.webCaptureOn=prefs.getBool("webCaptureOn",configItems.webCaptureOn);  
    ////////////////////////
    ci.deviceName=prefs.getString("deviceName",configItems.deviceName);
    ci.botTTelegram=prefs.getString("botTTelegram",configItems.botTTelegram);
    ci.adminChatIds=prefs.getString("adminChatIds",configItems.adminChatIds);
    ci.userChatIds=prefs.getString("userChatIds",configItems.userChatIds);
    ci.timeZone=prefs.getString("timeZone",configItems.timeZone);
    ////////////////////////////////////////////
    prefs.end();
  }
  return(ci);
}
////////////////////////////////////////////////////////////////////////////
boolean saveConfiguration(config_item* ci) {
  Serial.println("EEPROM Write:start");
  boolean bDirty=false;
  if (!prefs.begin("settings")){
    Serial.println("failed find settings prefrences! returning default."); 
    prefs.end();
    return (false);
  }else{
    if (prefs.getBool("useFlash")!=ci->useFlash)
      { prefs.putBool("useFlash",ci->useFlash); bDirty=true; }
    if (((framesize_t)prefs.getUInt("frameSize"))!=ci->frameSize)
      { prefs.putUInt("frameSize", (unsigned int) (ci->frameSize) ); bDirty=true; }
    if (prefs.getBool("hMirror")!=ci->hMirror)
      { prefs.putBool("hMirror",ci->hMirror); bDirty=true; }
    if (prefs.getBool("vFlip")!=ci->vFlip)
      { prefs.putBool("vFlip",ci->vFlip); bDirty=true; }
    if (prefs.getBool("screenFlip")!=ci->screenFlip)
      { prefs.putBool("screenFlip",ci->screenFlip); bDirty=true; }
    if (prefs.getBool("screenOn")!=ci->screenOn)
      { prefs.putBool("screenOn",ci->screenOn); bDirty=true; }
    if (prefs.getBool("motDetectOn")!=ci->motDetectOn)
      { prefs.putBool("motDetectOn",ci->motDetectOn); bDirty=true; }
    if (prefs.getBool("webCaptureOn")!=ci->webCaptureOn)
      { prefs.putBool("webCaptureOn",ci->webCaptureOn); bDirty=true; }
    if (prefs.getInt("lapseTime")!=ci->lapseTime)
      { prefs.putInt("lapseTime",ci->lapseTime); bDirty=true; }
      
    ///////////////////////////////
    if (!prefs.getString("deviceName").equals(ci->deviceName)){
      prefs.putString("deviceName",ci->deviceName);
      bDirty=true; 
    }
    if (!prefs.getString("botTTelegram").equals(ci->botTTelegram)){
      prefs.putString("botTTelegram",ci->botTTelegram);
      bDirty=true; 
    }
    if (!prefs.getString("adminChatIds").equals(ci->adminChatIds)){
      prefs.putString("adminChatIds",ci->adminChatIds);
      bDirty=true; 
    }
    if (!prefs.getString("userChatIds").equals(ci->userChatIds)){
      prefs.putString("userChatIds",ci->userChatIds);
      bDirty=true; 
    }
    if (!prefs.getString("timeZone").equals(ci->timeZone)){
      prefs.putString("timeZone",ci->timeZone);
      bDirty=true; 
    }
  }
  prefs.end();
  Serial.println("EEPROM Write:End");
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
  result += prefix+"hMirror          "+sep+"";
  result += (ci->hMirror ? String("true") : String("false")) + suffix;
  result += prefix+"vFlip            "+sep+"";
  result += (ci->vFlip ? String("true") : String("false"))  + suffix;
  result += prefix+"webCaptureOn     "+sep+"";
  result += (ci->webCaptureOn ? String("true") : String("false")) + suffix;
  result += prefix+"frameSize        "+sep+"";
  result += String((unsigned int) ci->frameSize) + ",";
  Serial.print ("printConfiguration:ci->frameSize:");
  Serial.println (ci->frameSize);
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
  struct tm *tm;
  time_t  t;
  char    dateTime[100];
  result += prefix+"Last Photo taken "+sep+"";
  if (timeOfLastPhoto>0){
    t = time(NULL)-timeOfLastPhoto;
    tm = localtime(&t);
    sprintf(dateTime, "%02d Y %02d Mon %02d D,%02d H:%02d Min:%02d S\0",
      tm->tm_year + 1900-1970, tm->tm_mon , tm->tm_mday-1,
      tm->tm_hour, tm->tm_min, tm->tm_sec);
    result += String(dateTime)+suffix;
  }else{
    result += "UNKNOWN" +suffix;
  }
  result += "</pre>";
  return (result);
}
////////////////////////////////////////////////////////////////////////////
#endif // #PERSIST_H
