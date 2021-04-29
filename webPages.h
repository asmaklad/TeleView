#ifndef WEBPAGES_H
#define WEBPAGES_H

#include <AutoConnect.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include "esp_camera.h"
#include "persist.h"

WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig acConfig;
AutoConnectAux auxPageConfig;
///////////////////////////////////////////////////////////
void populateResolutionsSelects(AutoConnectAux& aux){
  //aux.fetchElement();
  AutoConnectSelect& selectElementFS = aux["XframeSize"].as<AutoConnectSelect>();
  selectElementFS.empty();
  int maxRes=0;
  if(psramFound()){
    maxRes = FRAMESIZE_UXGA;
  } else {
    maxRes = FRAMESIZE_SVGA;
  }
  Serial.print("populateResolutionsSelects:maxRes: ");
  Serial.println (maxRes);
  for (int n = 0; n <= maxRes; n++) {
    selectElementFS.add(String(resolutions[n][0]+":"+resolutions[n][1]));
  }
  AutoConnectSelect& selectElementTZ = aux["Xtimezone"].as<AutoConnectSelect>();
  selectElementTZ.empty();
  for (int n = 0; n < sizeof(TZ)/sizeof(Timezone_t) ; n++) {
    selectElementTZ.add( String(TZ[n].zone) );
  }
  Serial.println("populateResolutionsSelects#4");
}
///////////////////////////////////////////////////////////
bool captivePortalStarted(IPAddress ip){
  Serial.println("C.P. started, IP:" + WiFi.localIP().toString());
  Serial.print("acConfig.title:");
  Serial.println(acConfig.title);
  Serial.print("acConfig.apid:");
  Serial.println(acConfig.apid);
  Serial.print("acConfig.psk:");
  Serial.println(acConfig.psk);
  Serial.print("acConfig.hostName:");
  Serial.println(acConfig.hostName);
  Serial.print("acConfig.bootUri:");
  Serial.println(acConfig.bootUri);
  Serial.print("acConfig.homeUri:");
  Serial.println(acConfig.homeUri);
  Serial.print("acConfig.dns1:");
  Serial.println(acConfig.dns1);
  Serial.print("acConfig.dns2:");
  Serial.println(acConfig.dns2);
  Serial.print("acConfig.apip :");
  Serial.println(acConfig.apip);
  Serial.print("acConfig.gateway:");
  Serial.println(acConfig.gateway);
  Serial.print("acConfig.netmask:");
  Serial.println(acConfig.netmask );
  return(true);
}
///////////////////////////////////////////////////////////
String onPage(AutoConnectAux& aux, PageArgument& args) {
  Serial.print("Portal.where()");
  Serial.println(Portal.where());
  Serial.print("args.size()");
  Serial.println(args.size());
  for (int i=0;i<args.args();i++){
    Serial.print(args.argName(i));
    Serial.print(":");
    Serial.print(args.arg(i));
    Serial.println();
  }
#ifndef  I2C_DISPLAY_ADDR
    aux["XscreenOn"].as<AutoConnectCheckbox>().enable=false;
    aux["XscreenFlip"].as<AutoConnectCheckbox>().enable=false;
#endif
#ifndef  PIR_PIN
    aux["XmotDetectOn"].as<AutoConnectCheckbox>().enable=false;
#endif
#ifndef IS_THERE_A_FLASH
  aux["XuseFlash"].as<AutoConnectCheckbox>().enable=false;
#endif    
  if (Portal.where().equals("/teleView")){
    if (args.hasArg("bxToken"))
      configItems.botTTelegram=args.arg("bxToken");
    if (args.hasArg("XTelegramAdminChatId"))
      configItems.adminChatIds=args.arg("XTelegramAdminChatId");
    if (args.hasArg("XTelegramUserChatId"))
      configItems.userChatIds=args.arg("XTelegramUserChatId");
    if (args.hasArg("XdeviceName"))
      configItems.deviceName=args.arg("XdeviceName");
    if (args.hasArg("XlapseTime"))
      configItems.lapseTime=args.arg("XlapseTime").toInt();
    if (args.hasArg("XframeSize")){
      String framsizeText=args.arg("XframeSize");
      framsizeText=framsizeText.substring(0,framsizeText.indexOf(":"));
      unsigned int matched=matchResolutionText(framsizeText);
      configItems.frameSize=(framesize_t) (matched) ;
    }
    configItems.hMirror=(args.hasArg("XhMirror")?true:false);
    configItems.useFlash=(args.hasArg("XuseFlash")?true:false);
    configItems.vFlip=(args.hasArg("XvFlip")?true:false);
    configItems.screenFlip=(args.hasArg("XscreenFlip")?true:false);
    configItems.screenOn=(args.hasArg("XscreenOn")?true:false);
    configItems.motDetectOn=(args.hasArg("XmotDetectOn")?true:false);    
    applyConfigItem(&configItems);
    saveConfiguration(&configItems);
  }else{
    aux.fetchElement();
    aux["bxToken"].as<AutoConnectInput>().value=configItems.botTTelegram;
    aux["XTelegramAdminChatId"].as<AutoConnectInput>().value=configItems.adminChatIds;
    aux["XTelegramUserChatId"].as<AutoConnectInput>().value=configItems.userChatIds;
    aux["XdeviceName"].as<AutoConnectInput>().value=configItems.deviceName;
    aux["XlapseTime"].as<AutoConnectInput>().value=configItems.lapseTime;
    
    aux["XuseFlash"].as<AutoConnectCheckbox>().checked=configItems.useFlash;
    aux["XhMirror"].as<AutoConnectCheckbox>().checked=configItems.hMirror;
    aux["XvFlip"].as<AutoConnectCheckbox>().checked=configItems.vFlip;
    aux["XscreenFlip"].as<AutoConnectCheckbox>().checked=configItems.screenFlip;
    aux["XscreenOn"].as<AutoConnectCheckbox>().checked=configItems.screenOn;
    aux["XmotDetectOn"].as<AutoConnectCheckbox>().checked=configItems.motDetectOn;
    aux["XframeSize"].as<AutoConnectSelect>().select (
        String(resolutions[configItems.frameSize][0]+":"+resolutions[configItems.frameSize][1]) 
    );
    Serial.println("XframeSize_select:"+String(resolutions[configItems.frameSize][0]+":"+resolutions[configItems.frameSize][1]));
  }
  return String();
}
///////////////////////////////////////////////////////////
static const char AUX_CONFIGPAGE[] PROGMEM = R"(
{
  "title": "Teleview",
  "uri": "/teleView",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "<h2>TeleView Configuration</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "content",
      "type": "ACText",
      "value": "Please use this page to configure your Teleview settings"
    },
    {
      "name": "XdeviceName",
      "type": "ACInput",
      "label": "Device Name",
      "placeholder": "TeleView",
      "pattern": "%s",
      "global": true
    },
    {
      "name": "XlapseTime",
      "type": "ACInput",
      "label": "Lapse Time-min",
      "placeholder": "720",
      "pattern": "%d",
      "global": true
    },
    {
      "name": "bxToken",
      "type": "ACInput",
      "label": "Telegram Bot Token",
      "placeholder": "Bot:Token",
      "pattern": "%s",
      "global": true
    },
    {
      "name": "XTelegramAdminChatId",
      "type": "ACInput",
      "label": "Admin Chat ID",
      "pattern": "^([0-9]+)$",
      "global": true
    },
    {
      "name": "XTelegramUserChatId",
      "type": "ACInput",
      "label": "User Chat ID",
      "pattern": "^([0-9]+)$",
      "global": true
    },
    {
      "name": "XuseFlash",
      "type": "ACCheckbox",
      "value": "",
      "labelPosition": "AC_Infront",
      "label": "Use Falsh when snapping a photo",
      "checked": false,
      "global": true
    },
    {
      "name": "XhMirror",
      "type": "ACCheckbox",
      "value": "",
      "labelPosition": "AC_Infront" ,
      "label": "Horizontal Mirror",
      "checked": false,
      "global": true
    },
    {
      "name": "XvFlip",
      "type": "ACCheckbox",
      "value": "",
      "labelPosition": "AC_Infront" ,
      "label": "Vertical Flip",
      "checked": false,
      "global": true
    },
    {
      "name": "XscreenFlip",
      "type": "ACCheckbox",
      "value": "",
      "labelPosition": "AC_Infront" ,
      "label": "Screen Flip",
      "checked": false,
      "global": true
    },
    {
      "name": "XscreenOn",
      "type": "ACCheckbox",
      "value": "",
      "labelPosition": "AC_Infront" ,
      "label": "OLED Display is On",
      "checked": false,
      "global": true
    },
    {
      "name": "XmotDetectOn",
      "type": "ACCheckbox",
      "value": "",
      "labelPosition": "AC_Infront" ,
      "label": "Motion Detection Enabled",
      "checked": false,
      "global": true
    },
    {
      "name": "Xtimezone",
      "type": "ACSelect",
      "label": "Select TZ name",
      "option": [],
      "selected": 0,
      "global": true
    },
    {
      "name": "XframeSize",
      "type": "ACSelect",
      "label": "Select Resolution",
      "option": [],
      "selected": 0,
      "global": true
    },
    {
      "name": "okSubmit",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/teleView"
    },
    {
      "name": "deleteSubmit",
      "type": "ACSubmit",
      "value": "Delete Evertything",
      "uri": "/delete"
    },
    {
      "name": "cancel",
      "type": "ACSubmit",
      "value": "Cancel",
      "uri": "/_ac"
    }
  ]
}
)";
///////////////////////////////////////////////////////////
void rootPage() {
  String  content =
    "<html>"
    " <head>"
    "   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "   <script type=\"text/javascript\">"
    "     setTimeout(\"location.reload()\", 60000);"
    "   </script>"
    " </head>"
    " <body>"
    "   <h2 align=\"center\" style=\"color:blue;margin:20px;\">Device Name: {{DEVICE_NAME}}</h2>"    
    "   <h3 align=\"center\" style=\"color:gray;margin:10px;\">Uptime: {{DateTime}}</h3>"    
    "   <h3 align=\"center\" style=\"color:gray;margin:10px;\">Pictures Taken: {{PICTURES_COUNT}}</h3>"
    "       <h3 align=\"center\" style=\"color:gray;margin:10px;\">"    
    "           compiled: {{COMPILE_TIME}} <br/>"
    "           <a href=\"/capture\">Have a look!</a> "
    "       </h3>"
    "   <p></p><p style=\"padding-top:15px;text-align:center\">To Configuire:" AUTOCONNECT_LINK(COG_24) "</p>"
    //"   <iframe src =\"/capture\" style=\"border:none;\" align=\"center\" width={{width}} height={{height}} > "
    "       <h3 align=\"center\" style=\"color:gray;margin:10px;\">"
    "         <br/><img src=\"/capture.jpg\"/><br/>"
    "       </h3>"
    //"   </iframe>"        
    "   <p style=\"text-align:center;\">Reload the page to update the capture.</p>"    
    " </body>"
    "</html>";
  
  struct tm *tm;
  time_t  t;
  char    dateTime[100];
  t = time(NULL);
  tm = localtime(&t);
  
  sprintf(dateTime, "%02d Years -%02d Months -%02d Days  %02d Hrs :%02d Min :%02d Sec\0",
    tm->tm_year + 1900-1970, tm->tm_mon , tm->tm_mday-1,
    tm->tm_hour, tm->tm_min, tm->tm_sec);
  content.replace("{{DateTime}}", String(dateTime));
  String res=String(resolutions[configItems.frameSize][1]);
  String Width=res.substring(0,res.indexOf("x"));
  String Height=res.substring(res.indexOf("x")+1);
  content.replace("{{width}}",Width  );
  content.replace("{{height}}",Height);
  content.replace("{{COMPILE_TIME}}",compileDate+" "+compileTime+","+compileCompiler);
  content.replace("{{DEVICE_NAME}}",configItems.deviceName);
  content.replace("{{PICTURES_COUNT}}",String(PICTURES_COUNT));
  
  Server.send(200,"text/html",content);
}
///////////////////////////////////////////////////////////
void deletePage() {
  deleteConfiguration();  
  Server.sendHeader("Location","/");
  Server.send(303);
  //
  ESP.restart();
}
///////////////////////////////////////////////////////////
void capturePage(){
  if (configItems.webCaptureOn){
    camera_fb_t * fb = NULL;
    WiFiClient client = Server.client();
    if (!client) {
      return;
    }
    client.flush();
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    } else {
      Server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
      Server.sendHeader("Connection","close");
      Server.sendHeader("Access-Control-Allow-Origin", "*");
      Server.send_P(200,"image/jpeg",(const char *)fb->buf,fb->len);
      Serial.printf("STILL-MJPG: %uB\n",(uint32_t)(fb->len));
      esp_camera_fb_return(fb);
      fb = NULL;
    }
    client.println(configItems.deviceName);
    client.flush();
    client.stop();
  }
}
///////////////////////////////////////////////////////////
void capturePageJpeg(){
  if (configItems.webCaptureOn){
    camera_fb_t * fb = NULL;
    WiFiClient client = Server.client();
    if (!client) {
      return;
    }
    client.flush();
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    } else {
      Server.sendHeader("Content-Disposition", "attachment; filename=capture.jpg");
      Server.sendHeader("Connection","close");
      Server.sendHeader("Access-Control-Allow-Origin", "*");
      Server.send_P(200,"image/jpeg",(const char *)fb->buf,fb->len);
      Serial.printf("STILL-MJPG: %uB\n",(uint32_t)(fb->len));
      esp_camera_fb_return(fb);
      fb = NULL;
    }
    client.println(configItems.deviceName);
    client.flush();
    client.stop();
  }
}
///////////////////////////////////////////////////////////
/*
typedef enum {
  PIXFORMAT_RGB565,    // 2BPP/RGB565
  PIXFORMAT_YUV422,    // 2BPP/YUV422
  PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
  PIXFORMAT_JPEG,      // JPEG/COMPRESSED
  PIXFORMAT_RGB888,    // 3BPP/RGB888
  PIXFORMAT_RAW,       // RAW
  PIXFORMAT_RGB444,    // 3BP2P/RGB444
  PIXFORMAT_RGB555,    // 3BP2P/RGB555
} pixformat_t;
*/
///////////////////////////////////////////////////////////
/*
typedef enum {
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QQVGA2,   // 128x160
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_CIF,      // 400x296
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    FRAMESIZE_QXGA,     // 2048*1536
    FRAMESIZE_INVALID
} framesize_t;
 */


#endif //WEBPAGES

  
