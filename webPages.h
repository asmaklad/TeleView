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
AutoConnectAux auxPageCapture;

char   dateTime[100];
time_t currentTimeMills ;
time_t timeB;
struct tm  *tmNowTM , *tmDiffTM;

///////////////////////////////////////////////////////////
void printLocalTime();
///////////////////////////////////////////////////////////
void populateResolutionsSelects(AutoConnectAux& aux){
  //aux.fetchElement();
  /*
  if(psramFound()){
    maxRes = FRAMESIZE_UXGA;
  } else {
    maxRes = FRAMESIZE_SVGA;
  }
  int maxRes=0;
  maxRes = FRAMESIZE_UXGA;
  */
  //////
  AutoConnectSelect& selectElementFS = aux["XframeSize"].as<AutoConnectSelect>();
  selectElementFS.empty();
  for (int n = 0; n <= maxRes; n++) {
    selectElementFS.add(String(resolutions[n][0]+":"+resolutions[n][1]));
  }
  //////
  AutoConnectSelect& selectElementTZ = aux["Xtimezone"].as<AutoConnectSelect>();
  selectElementTZ.empty();
  for (int n = 0; n < sizeof(TZ)/sizeof(Timezone_t) ; n++) {
    selectElementTZ.add( String(TZ[n].zone) );
  }
  //////
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
String onCapture(AutoConnectAux& aux, PageArgument& args) {
  Serial.print("onCapture:Portal.where()");
  Serial.println(Portal.where());
  Serial.print("onCapture:args.size()");
  Serial.println(args.size());
  return String("");
}
///////////////////////////////////////////////////////////
String onPage(AutoConnectAux& aux, PageArgument& args) {
  Serial.print("onPage:Portal.where()");
  Serial.println(Portal.where());
  Serial.print("onPage:args.size()");
  Serial.println(args.size());
  String result="Success";
  for (int i=0;i<args.args();i++){
    Serial.printf( "*> %s:%s\n", args.argName(i).c_str(), String(args.arg(i)).c_str() );
  }
#ifndef  I2C_DISPLAY_ADDR
    aux["XscreenOn"].as<AutoConnectCheckbox>().enable=false;
    aux["XscreenFlip"].as<AutoConnectCheckbox>().enable=false;
#endif
#ifndef  PIR_PIN
    aux["XmotDetectOn"].as<AutoConnectCheckbox>().enable=false;
#endif
#ifndef  SD_CARD_ON
    aux["XsaveToSD"].as<AutoConnectCheckbox>().enable=false;
    aux["XftpPass"].as<AutoConnectCheckbox>().enable=false;
    aux["XftpUser"].as<AutoConnectCheckbox>().enable=false;
    aux["XftpServerOn"].as<AutoConnectCheckbox>().enable=false;
    aux["XvideoToSDOn"].as<AutoConnectCheckbox>().enable=false;
#endif
#ifndef IS_THERE_A_FLASH
    aux["XuseFlash"].as<AutoConnectCheckbox>().enable=false;
#endif
#ifndef  BUZZER_PIN
    aux["XuseBuzzer"].as<AutoConnectCheckbox>().enable=false;
#endif

// not yet implemented:
aux["Xset_whitebal"].as<AutoConnectCheckbox>().enable=false;
//aux["Xset_saturation"].as<AutoConnectCheckbox>().enable=false;
//aux["Xset_contrast"].as<AutoConnectCheckbox>().enable=false;
//aux["Xset_brightness"].as<AutoConnectCheckbox>().enable=false;
if (!psramFound()){
    //TBD
    //aux["XFaceDetect"].as<AutoConnectCheckbox>().enable=false;
}
  // the source page is also /teleview and has some args with it, then we need to save it.
  if (Portal.where().equals("/teleView") && args.size()>0 ){
    result="Save Error";
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
      framsizeText="/"+framsizeText.substring(0,framsizeText.indexOf(":"));
      unsigned int matched=matchResolutionText(framsizeText);
      configItems.frameSize=(framesize_t) (matched) ;
    }
    configItems.timeZone=(args.hasArg("Xtimezone")?args.arg("Xtimezone"):"Europe/Berlin");
    configItems.hMirror=(args.hasArg("XhMirror")?true:false);
    configItems.useFlash=(args.hasArg("XuseFlash")?true:false);
    configItems.vFlip=(args.hasArg("XvFlip")?true:false);
    configItems.ftpPass=(args.hasArg("XftpPass")?args.arg("XftpPass"):"");
    configItems.ftpUser=(args.hasArg("XftpUser")?args.arg("XftpUser"):"");
    configItems.ftpServerOn=(args.hasArg("XftpServerOn")?true:false);
    configItems.videoToSDOn=(args.hasArg("XvideoToSDOn")?true:false);
    configItems.videoTime=(args.hasArg("XvideoTime")?args.arg("XvideoTime").toInt():0);
    configItems.sendVideoOn=(args.hasArg("XsendVideoOn")?true:false);

    configItems.set_whitebal=(args.hasArg("Xset_whitebal")?args.arg("Xset_whitebal").toInt():0);
    configItems.set_saturation=(args.hasArg("Xset_saturation")?args.arg("Xset_saturation").toInt():0);
    configItems.set_contrast=(args.hasArg("Xset_contrast")?args.arg("Xset_contrast").toInt():0);
    configItems.set_brightness=(args.hasArg("Xset_brightness")?args.arg("Xset_brightness").toInt():0);
    configItems.jpegQuality=(args.hasArg("XjpegQuality")?args.arg("XjpegQuality").toInt():0);

    configItems.sMTPTLS=(args.hasArg("XsMTPTLS")?true:false);
    configItems.sMTPPort=(args.hasArg("XsMTPPort")?args.arg("XsMTPPort").toInt():0);
    configItems.sMTPPassword=(args.hasArg("XsMTPPassword")?args.arg("XsMTPPassword"):"");
    configItems.sMTPUsername=(args.hasArg("XsMTPUsername")?args.arg("XsMTPUsername"):"");
    configItems.sMTPServer=(args.hasArg("XsMTPServer")?args.arg("XsMTPServer"):"");
    configItems.userEmail=(args.hasArg("XuserEmail")?args.arg("XuserEmail"):"");
    configItems.adminEmail=(args.hasArg("XadminEmail")?args.arg("XadminEmail"):"");
    configItems.sendEmail=(args.hasArg("XsendEmail")?true:false);

    configItems.motionDetectVC=(args.hasArg("XmotionDetectVC")?true:false);
    configItems.alertALL=(args.hasArg("XalertALL")?true:false);
    configItems.saveToSD=(args.hasArg("XsaveToSD")?true:false);
    configItems.useDeepSleep=(args.hasArg("XuseDeepSleep")?true:false);
    configItems.useBuzzer=(args.hasArg("XuseBuzzer")?true:false);
    configItems.screenFlip=(args.hasArg("XscreenFlip")?true:false);
    configItems.screenOn=(args.hasArg("XscreenOn")?true:false);
    configItems.motDetectOn=(args.hasArg("XmotDetectOn")?true:false);
    applyConfigItem(&configItems);
    saveConfiguration(&configItems);
    result="Saved";
  }else if (args.size()==0) {
    // no arguments wehere provided, then we need to load
    result="Load Error";
    aux.fetchElement();
    aux["bxToken"].as<AutoConnectInput>().value=configItems.botTTelegram;
    aux["XTelegramAdminChatId"].as<AutoConnectInput>().value=configItems.adminChatIds;
    aux["XTelegramUserChatId"].as<AutoConnectInput>().value=configItems.userChatIds;
    aux["XdeviceName"].as<AutoConnectInput>().value=configItems.deviceName;
    aux["XlapseTime"].as<AutoConnectInput>().value=configItems.lapseTime;
    aux["XuseFlash"].as<AutoConnectCheckbox>().checked=configItems.useFlash;
    aux["XhMirror"].as<AutoConnectCheckbox>().checked=configItems.hMirror;
    aux["XvFlip"].as<AutoConnectCheckbox>().checked=configItems.vFlip;
    aux["XftpPass"].as<AutoConnectInput>().value=configItems.ftpPass;
    aux["XftpUser"].as<AutoConnectInput>().value=configItems.ftpUser;
    aux["XftpServerOn"].as<AutoConnectCheckbox>().checked=configItems.ftpServerOn;
    aux["XvideoToSDOn"].as<AutoConnectCheckbox>().checked=configItems.videoToSDOn;
    aux["XvideoTime"].as<AutoConnectInput>().value=configItems.videoTime;
    aux["XsendVideoOn"].as<AutoConnectCheckbox>().checked=configItems.sendVideoOn;
    aux["Xset_whitebal"].as<AutoConnectInput>().value=configItems.set_whitebal;
    aux["Xset_saturation"].as<AutoConnectInput>().value=configItems.set_saturation;
    aux["Xset_contrast"].as<AutoConnectInput>().value=configItems.set_contrast;
    aux["Xset_brightness"].as<AutoConnectInput>().value=configItems.set_brightness;
    aux["XjpegQuality"].as<AutoConnectInput>().value=configItems.jpegQuality;
    aux["XsMTPTLS"].as<AutoConnectCheckbox>().checked=configItems.sMTPTLS;
    aux["XsMTPPort"].as<AutoConnectInput>().value=configItems.sMTPPort;
    aux["XsMTPPassword"].as<AutoConnectInput>().value=configItems.sMTPPassword;
    aux["XsMTPUsername"].as<AutoConnectInput>().value=configItems.sMTPUsername;
    aux["XsMTPServer"].as<AutoConnectInput>().value=configItems.sMTPServer;
    aux["XuserEmail"].as<AutoConnectInput>().value=configItems.userEmail;
    aux["XadminEmail"].as<AutoConnectInput>().value=configItems.adminEmail;
    aux["XsendEmail"].as<AutoConnectCheckbox>().checked=configItems.sendEmail;
    aux["XmotionDetectVC"].as<AutoConnectCheckbox>().checked=configItems.motionDetectVC;
    aux["XalertALL"].as<AutoConnectCheckbox>().checked=configItems.alertALL;
    aux["XsaveToSD"].as<AutoConnectCheckbox>().checked=configItems.saveToSD;
    aux["XuseDeepSleep"].as<AutoConnectCheckbox>().checked=configItems.useDeepSleep;
    aux["XuseBuzzer"].as<AutoConnectCheckbox>().checked=configItems.useBuzzer;
    aux["XscreenFlip"].as<AutoConnectCheckbox>().checked=configItems.screenFlip;
    aux["XscreenOn"].as<AutoConnectCheckbox>().checked=configItems.screenOn;
    aux["XmotDetectOn"].as<AutoConnectCheckbox>().checked=configItems.motDetectOn;
    aux["XframeSize"].as<AutoConnectSelect>().select (
        String(resolutions[configItems.frameSize][0]+":"+resolutions[configItems.frameSize][1])
    );
    Serial.println("XframeSize_select:"+String(resolutions[configItems.frameSize][0]+":"+resolutions[configItems.frameSize][1]));
    aux["XtimeZone"].as<AutoConnectSelect>().select (
        String(configItems.timeZone)
    );
    Serial.println("XtimeZone_select:"+String(configItems.timeZone));
    result="Loaded";
  } else{
    result="This Shouldn't happen";
  }
  aux["XResult"].value=result;
  return String("");
}
///////////////////////////////////////////////////////////
static const char AUX_CONFIGPAGE[] PROGMEM = R"(
{
  "title": "Options",
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
      "name": "seperator1",
      "type": "ACElement",
      "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    },
    {
      "name": "sectionBot",
      "type": "ACText",
      "value": "<h2>Telegram Bot Configuration</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "XdeviceName",
      "type": "ACInput",
      "label": "Device Name",
      "placeholder": "TeleView",
      "apply" : "text",
      "pattern": "^(.+)$",
      "global": true
    },
    {
      "name": "XlapseTime",
      "type": "ACInput",
      "label": "Lapse Time-min",
      "placeholder": "720",
      "pattern": "^([0-9]+)$",
      "global": true,
      "apply" : "number"
    },
    {
      "name": "bxToken",
      "type": "ACInput",
      "apply" : "text",
      "label": "Telegram Bot Token",
      "placeholder": "Bot:Token",
      "pattern": "^(.+)$",
      "global": true
    },
    {
      "name": "XTelegramAdminChatId",
      "type": "ACInput",
      "apply" : "text",
      "label": "Admin Chat ID",
      "pattern": "^([0-9]+)$",
      "global": true
    },
    {
      "name": "XTelegramUserChatId",
      "type": "ACInput",
      "apply" : "text",
      "label": "User Chat ID",
      "pattern": "^([0-9]+)$",
      "global": true
    },
    {
      "name": "seperator2",
      "type": "ACElement",
      "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    },
    {
      "name": "sectionBasic",
      "type": "ACText",
      "value": "<h2>Basic Configuration</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
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
    { "name": "XvFlip","type": "ACCheckbox","value": "","labelPosition": "AC_Infront" ,"label": "Vertical Flip","checked": false,"global": true},
    { "name": "XvideoToSDOn", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "Save Video to SD.", "checked": false, "global": true },
    { "name": "XvideoTime", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Duration of video to send in sec.","global": true , "placeholder": "videoTime",  "pattern": "^([+-]?([0-9]*))$"},
    { "name": "XsendVideoOn", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "Send Video on/off.", "checked": false, "global": true },
    { "name": "XalertALL", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "AlertAll ,MotionDetect & timelapse sent to all chatIds", "checked": false, "global": true },
    { "name": "XsaveToSD", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "Save Photos to SD ", "checked": false, "global": true },
    { "name": "XuseDeepSleep", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "Goto to deep sleep on MotionDetct or timeLapse", "checked": false, "global": true },
    { "name": "XuseBuzzer", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "Trigger buzzer on motion detect", "checked": false, "global": true },
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
      "label": "PIR Motion Detection Enabled",
      "checked": false,
      "global": true
    },
    { "name": "XmotionDetectVC", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "MotionDetect by Vision not PIR", "checked": false, "global": true },
    { "name": "Xset_whitebal", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Photo white balance mode 0-4","global": true , "placeholder": "set_whitebal",  "pattern": "^([+]?([0-4]*))$" ,"apply" : "number"},
    { "name": "Xset_saturation", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Photo saturation -2 to 2[0]","global": true , "placeholder": "set_saturation",  "pattern": "^([+-]?([0-9]*))$","apply" : "number"},
    { "name": "Xset_contrast", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Photo Contrast -2 to 2[0]","global": true , "placeholder": "set_contrast",  "pattern": "^([+-]?([0-9]*))$","apply" : "number"},
    { "name": "Xset_brightness", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Photo Brightness -2 to 2[0]","global": true , "placeholder": "set_brightness",  "pattern": "^([+-]?([0-9]*))$","apply" : "number"},
    { "name": "XjpegQuality", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Jpeg Quality 1-62[10-12]","global": true , "placeholder": "jpegQuality",  "pattern": "^([+-]?([0-9]*))$","apply" : "number"},
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
      "name": "seperatorFtp",
      "type": "ACElement",
      "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    },
    { "name": "XftpServerOn", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "start an FTP server for the SD.", "checked": false, "global": true },
    { "name": "XftpUser", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "FTP username.","global": true , "placeholder": "ftpUser",  "pattern": "^(.+)$" ,"apply": "text"},
    { "name": "XftpPass", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "FTP Password.","global": true , "placeholder": "ftpPass",  "pattern": "^(.+)$" ,"apply": "password"},
    {
      "name": "seperatorEmail",
      "type": "ACElement",
      "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    },
    {
      "name": "sectionEmail",
      "type": "ACText",
      "value": "<h2>Email Configuration</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "sectionEmail.1",
      "type": "ACText",
      "value": "<h3>dont use your real email, create a new one</h3>",
      "style": "text-align:center;color:#2fff4f;padding:10px;"
    },
    { "name": "XsendEmail", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "Alert by Email.", "checked": false, "global": true },
    { "name": "XsMTPUsername", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "SMTP Email username","global": true , "placeholder": "sMTPUsername",  "pattern": "^(.+)$" ,"apply": "text"},
    { "name": "XsMTPPassword", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "SMTP Email Password","global": true , "placeholder": "sMTPPassword",  "pattern": "^(.+)$" ,"apply": "password"},
    { "name": "XsMTPServer", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "SMTP Email Server","global": true , "placeholder": "sMTPServer",  "pattern": "^(.+)$" ,"apply": "text"},
    { "name": "XsMTPPort", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "SMTP Email Port","global": true , "placeholder": "sMTPPort",  "pattern": "^([+-]?([0-9]*))$" ,"apply": "number" },
    { "name": "XsMTPTLS", "type": "ACCheckbox", "value": "", "labelPosition": "AC_Infront" , "label": "SMTP TLS/SSL Required", "checked": false, "global": true },
    { "name": "XadminEmail", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Email 1","global": true , "placeholder": "adminEmail",  "pattern": "^(.+)$" ,"apply": "text" },
    { "name": "XuserEmail", "type": "ACInput", "value": "", "labelPosition": "AC_Infront" , "label": "Email 2","global": true , "placeholder": "userEmail",  "pattern": "^(.+)$" ,"apply": "text" },
    {
      "name": "seperator4",
      "type": "ACElement",
      "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
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
    },
    {
      "name": "XResult",
      "type": "ACText",
      "value": "XYZ",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    }
  ]
}
)";
///////////////////////////////////////////////////////////
static const char AUX_CAPTURE[] PROGMEM = R"(
{
  "title": "Capture",
  "uri": "/Xcapture",
  "menu": true,
  "element": [
    {
      "name": "CCaption",
      "type": "ACText",
      "value": "<h2>TeleView Capture</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "CCapture",
      "type": "ACElement",
      "value": "<img src=\"/capture.jpg\"/></img>"
    }
  ]
}
)";
///////////////////////////////////////////////////////////
void rootPage() {
  String  content =
    "<html>"
    " <head>"
    "   <meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "   <script type=\"text/javascript\">"
    "     setTimeout(\"location.reload()\", 20000);"
    "   </script>"
    " </head>"
    " <body>"
    "   <div align=\"center\" style=\"color:blue;margin:20px;\">"
    "     <table>"
    "       <tr><td>Device Name</td><td><h1>{{DEVICE_NAME}}</h1></td></tr>"
    //"       <tr><td>Started</td><td>{{START_TIME}}</td></tr>"
    "       <tr><td>Current</td><td>{{CURRENT_TIME}}</td></tr>"
    "       <tr><td>Uptime</td><td>{{UP_TIME}}</td></tr>"
    "       <tr><td>Pictures Taken</td><td>{{PICTURES_COUNT}}</td></tr>"
    "       <tr><td>Compiled</td><td>{{COMPILE_TIME}}</td></tr>"
    "       <tr><td>Have a look</td><td><a href=\"/capture\">/capture</a></td></tr>"
    "       <tr><td>To Configuire</td><td>" AUTOCONNECT_LINK(COG_24) "</td></tr>"
    "       <tr><td colspan=2><img src=\"/capture.jpg\"/></img></td></tr>"
    "     </table>"
    "   </div>"
    //"   <iframe src =\"/capture\" style=\"border:none;\" align=\"center\" width={{width}} height={{height}} > "
    //"   </iframe>"
    " </body>"
    "</html>";
  ///////
  timeB=time(NULL); // now
  tmNowTM = localtime(&timeB);
  sprintf(dateTime, "%02d/%02d/%02d, %02d:%02d:%02d\0",
    tmNowTM->tm_mday,
    tmNowTM->tm_mon+1,
    tmNowTM->tm_year+1900,
    tmNowTM->tm_hour,
    tmNowTM->tm_min,
    tmNowTM->tm_sec
  );
  content.replace("{{CURRENT_TIME}}", String(dateTime));
  ///////
  currentTimeMills=millis()/1000L;
  Serial.printf("timeB:%d\n",timeB);
  Serial.printf("currentTimeMills:%d\n",currentTimeMills);
  tmDiffTM = gmtime (&currentTimeMills);
  sprintf(dateTime, "%02d Years, %02d Months, %02d Days,  %02d Hrs :%02d Min :%02d Sec\0",
    tmDiffTM->tm_year -70,
    tmDiffTM->tm_mon ,
    tmDiffTM->tm_mday -1,
    tmDiffTM->tm_hour ,
    tmDiffTM->tm_min ,
    tmDiffTM->tm_sec
  );
  content.replace("{{UP_TIME}}", String(dateTime));
  ///////
  printLocalTime();
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
////////////////////////////////////////////////////////////////////////////
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("Current Time:");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
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

  
