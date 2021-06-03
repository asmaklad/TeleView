
#ifndef PERSIST_H
#define PERSIST_H

#include "esp_camera.h"

#include <Preferences.h>
#include <Arduino.h>  // for type definitions

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
  Serial.println("TESTX:"+text);
  for (int i=0;i<=maxRes;i++){
    Serial.println("compareTo:/"+String(resolutions[i][0]));
    if ((text.compareTo(String("/"+resolutions[i][0])))==0){
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
  String ftpPass;
  String ftpUser;
  boolean ftpServerOn;
  boolean videoToSDOn;
  long videoTime;
  boolean sendVideoOn;
  int set_whitebal;
  int set_saturation;
  int set_contrast;
  int set_brightness;
  int jpegQuality;
  boolean sMTPTLS;
  int sMTPPort;
  String sMTPPassword;
  String sMTPUsername;
  String sMTPServer;
  String userEmail;
  String adminEmail;
  boolean sendEmail;
  boolean motionDetectVC;
  boolean alertALL;
  boolean saveToSD;
  boolean useDeepSleep;
  boolean useBuzzer;
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
} configItems {
  .useFlash = true,
  .frameSize = FRAMESIZE_CIF,
  .hMirror = true,
  .vFlip = true,
  .ftpPass = "esp",
  .ftpUser = "esp",
  .ftpServerOn = false,
  .videoToSDOn = false,
  .videoTime = 0,
  .sendVideoOn = false,
  .set_whitebal = 1,
  .set_saturation = 0,
  .set_contrast = 0,
  .set_brightness = 0,
  .jpegQuality = 12,
  .sMTPTLS = true,
  .sMTPPort = 0,
  .sMTPPassword = "",
  .sMTPUsername = "",
  .sMTPServer = "",
  .userEmail = "",
  .adminEmail = "",
  .sendEmail = false,
  .motionDetectVC = false,
  .alertALL = false,
  .saveToSD = false,
  .useDeepSleep = false,
  .useBuzzer = false,
  .screenFlip = false,
  .screenOn=true,
  .motDetectOn=false,
  .deviceName = String("TeleView"),
  .botTTelegram = String("0X123456789"),
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
  if (!prefs.begin("settings",false)) // False=RW
  {
    Serial.println("failed find settings prefrences! returning default."); 
    prefs.end();
    return ;
  }else{
    Serial.println("Deleting all settings");
    prefs.clear();
    /*
    prefs.remove("useFlash");
    prefs.remove("frameSize");
    prefs.remove("hMirror");
    prefs.remove("vFlip");
    prefs.remove("ftpPass");
    prefs.remove("ftpUser");
    prefs.remove("ftpServerOn");
    prefs.remove("videoToSDOn");
    prefs.remove("videoTime");
    prefs.remove("sendVideoOn");
    prefs.remove("sendVideo");
    prefs.remove("set_whitebal");
    prefs.remove("set_saturation");
    prefs.remove("set_contrast");
    prefs.remove("set_brightness");
    prefs.remove("jpegQuality");
    prefs.remove("sMTPTLS");
    prefs.remove("sMTPPort");
    prefs.remove("sMTPPassword");
    prefs.remove("sMTPUsername");
    prefs.remove("sMTPServer");
    prefs.remove("userEmail");
    prefs.remove("adminEmail");
    prefs.remove("sendEmail");
    prefs.remove("motionDetectVC");
    prefs.remove("alertALL");
    prefs.remove("saveToSD");
    prefs.remove("useDeepSleep");
    prefs.remove("useBuzzer");
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
    */
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
    ci.ftpPass = prefs.getString("ftpPass",configItems.ftpPass);
    ci.ftpUser = prefs.getString("ftpUser",configItems.ftpUser);
    ci.ftpServerOn = prefs.getBool("ftpServerOn",configItems.ftpServerOn);
    ci.videoToSDOn = prefs.getBool("videoToSDOn",configItems.videoToSDOn);
    ci.videoTime = prefs.getInt("videoTime",configItems.videoTime);
    ci.sendVideoOn = prefs.getBool("sendVideoOn",configItems.sendVideoOn);
    ci.set_whitebal = prefs.getInt("set_whitebal",configItems.set_whitebal);
    ci.set_saturation = prefs.getInt("set_saturation",configItems.set_saturation);
    ci.set_contrast = prefs.getInt("set_contrast",configItems.set_contrast);
    ci.set_brightness = prefs.getInt("set_brightness",configItems.set_brightness);
    ci.jpegQuality = prefs.getInt("jpegQuality",configItems.jpegQuality);

    ci.sMTPTLS = prefs.getBool("sMTPTLS",configItems.sMTPTLS);
    ci.sMTPPort = prefs.getInt("sMTPPort",configItems.sMTPPort);
    ci.sMTPPassword = prefs.getString("sMTPPassword",configItems.sMTPPassword);
    ci.sMTPUsername = prefs.getString("sMTPUsername",configItems.sMTPUsername);
    ci.sMTPServer = prefs.getString("sMTPServer",configItems.sMTPServer);
    ci.userEmail = prefs.getString("userEmail",configItems.userEmail);
    ci.adminEmail = prefs.getString("adminEmail",configItems.adminEmail);
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
  Serial.println("saveConfiguration:EEPROM Write:start");
  boolean bDirty=false;
  if (!prefs.begin("settings",false)){
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
    if (!prefs.getString("ftpPass").equals(ci->ftpPass))
      { prefs.putString("ftpPass",ci->ftpPass); bDirty=true; }
    if (!prefs.getString("ftpUser").equals(ci->ftpUser))
      { prefs.putString("ftpUser",ci->ftpUser); bDirty=true; }
    if (prefs.getBool("ftpServerOn")!=ci->ftpServerOn)
      { prefs.putBool("ftpServerOn",ci->ftpServerOn); bDirty=true; }
    if (prefs.getBool("videoToSDOn")!=ci->videoToSDOn)
      { prefs.putBool("videoToSDOn",ci->videoToSDOn); bDirty=true; }
    if (prefs.getInt("videoTime")!=ci->videoTime)
      { prefs.putInt("videoTime",ci->videoTime); bDirty=true; }
    if (prefs.getBool("sendVideoOn")!=ci->sendVideoOn)
      { prefs.putBool("sendVideoOn",ci->sendVideoOn); bDirty=true; }

    if (prefs.getInt("set_whitebal")!=ci->set_whitebal)
      { prefs.putInt("set_whitebal",ci->set_whitebal); bDirty=true; }
    if (prefs.getInt("set_saturation")!=ci->set_saturation)
      { prefs.putInt("set_saturation",ci->set_saturation); bDirty=true; }
    if (prefs.getInt("set_contrast")!=ci->set_contrast)
      { prefs.putInt("set_contrast",ci->set_contrast); bDirty=true; }
    if (prefs.getInt("set_brightness")!=ci->set_brightness)
      { prefs.putInt("set_brightness",ci->set_brightness); bDirty=true; }
    if (prefs.getInt("jpegQuality")!=ci->jpegQuality)
      { prefs.putInt("jpegQuality",ci->jpegQuality); bDirty=true; }
    //
    if (prefs.getBool("sMTPTLS")!=ci->sMTPTLS)
      { prefs.putBool("sMTPTLS",ci->sMTPTLS); bDirty=true; }
    if (prefs.getInt("sMTPPort")!=ci->sMTPPort)
      { prefs.putInt("sMTPPort",ci->sMTPPort); bDirty=true; }
    if (!prefs.getString("sMTPPassword").equals(ci->sMTPPassword))
      { prefs.putString("sMTPPassword",ci->sMTPPassword); bDirty=true; }
    if (!prefs.getString("sMTPUsername").equals(ci->sMTPUsername))
      { prefs.putString("sMTPUsername",ci->sMTPUsername); bDirty=true; }
    if (!prefs.getString("sMTPServer").equals(ci->sMTPServer))
      { prefs.putString("sMTPServer",ci->sMTPServer); bDirty=true; }
    if (!prefs.getString("userEmail").equals(ci->userEmail))
      { prefs.putString("userEmail",ci->userEmail); bDirty=true; }
    if (!prefs.getString("adminEmail").equals(ci->adminEmail))
      { prefs.putString("adminEmail",ci->adminEmail); bDirty=true; }
    if (prefs.getBool("sendEmail")!=ci->sendEmail)
      { prefs.putBool("sendEmail",ci->sendEmail); bDirty=true; }
    //
    if (prefs.getBool("motionDetectVC")!=ci->motionDetectVC)
      { prefs.putBool("motionDetectVC",ci->motionDetectVC); bDirty=true; }
    if (prefs.getBool("alertALL")!=ci->alertALL)
      { prefs.putBool("alertALL",ci->alertALL); bDirty=true; }
    if (prefs.getBool("saveToSD")!=ci->saveToSD)
      { prefs.putBool("saveToSD",ci->saveToSD); bDirty=true; }
    if (prefs.getBool("useDeepSleep")!=ci->useDeepSleep)
      { prefs.putBool("useDeepSleep",ci->useDeepSleep); bDirty=true; }
    if (prefs.getBool("useBuzzer")!=ci->useBuzzer)
      { prefs.putBool("useBuzzer",ci->useBuzzer); bDirty=true; }
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
  Serial.println("saveConfiguration:EEPROM Write:End");
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
  struct tm *tm;
  time_t  t;
  char    dateTime[100];
  t = time(NULL);
  tm = localtime(&t);

  sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d\0",
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
  result += prefix+"ftpPass          "+sep+"";
  result += String(ci->ftpPass)  + suffix;
  result += prefix+"ftpUser          "+sep+"";
  result += String(ci->ftpUser)  + suffix;
  result += prefix+"ftpServerOn      "+sep+"";
  result += (ci->ftpServerOn ? String("true") : String("false"))  + suffix;
  result += prefix+"videoToSDOn      "+sep+"";
  result += (ci->videoToSDOn ? String("true") : String("false"))  + suffix;
#endif
#if defined(BUZZER_PIN)
  result += prefix+"useBuzzer        "+sep+"";
  result += (ci->useBuzzer ? String("true") : String("false"))  + suffix;
#endif
  result += prefix+"hMirror          "+sep+"";
  result += (ci->hMirror ? String("true") : String("false")) + suffix;
  result += prefix+"vFlip            "+sep+"";
  result += (ci->vFlip ? String("true") : String("false"))  + suffix;

  result += prefix+"videoTime   "+sep+"";
  result += String(ci->videoTime)  + suffix;
  result += prefix+"sendVideoOn   "+sep+"";
  result += (ci->sendVideoOn ? String("true") : String("false"))  + suffix;

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

  result += (ci->sendEmail ? String("true") : String("false"))  + suffix;
  result += prefix+"motionDetectCV   "+sep+"";
  result += (ci->motionDetectVC ? String("true") : String("false"))  + suffix;
  result += prefix+"alertALL         "+sep+"";
  result += (ci->alertALL ? String("true") : String("false"))  + suffix;
  result += prefix+"useDeepSleep     "+sep+"";
  result += (ci->useDeepSleep ? String("true") : String("false"))  + suffix;
  result += prefix+"webCaptureOn     "+sep+"";
  result += (ci->webCaptureOn ? String("true") : String("false")) + suffix;
  result += prefix+"frameSize        "+sep+"";
  Serial.println("printConfiguration#10");
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
  Serial.println("printConfiguration#11");
  /*
  struct tm *tm;
  time_t  t;
  char    dateTime[100];
  */
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
  Serial.println("printConfiguration#12");
  return (result);
}
////////////////////////////////////////////////////////////////////////////
#endif // #PERSIST_H
