#ifndef WEBPAGES_H
#define WEBPAGES_H

#include <AutoConnect.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include "esp_camera.h"
#include "persist.h"

static const char* TAG_WEB = "WEBPAGE";
#include "esp_log.h"

WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig acConfig;
AutoConnectAux auxPageConfig;
AutoConnectAux auxPageConfigEmail;
AutoConnectAux auxPageCapture;
AutoConnectAux auxPageStream;

char   dateTime[100];
time_t currentTimeMills ;
time_t timeB;
struct tm  *tmNowTM , *tmDiffTM;

void printLocalTime();
////////////////////////////////////////////////////////////////////////////////
// for the streaming server
#define PART_BOUNDARY "ef2ac69f9149220e889abc22b81d1401"

static uint8_t ucParameterToPass=10;
TaskHandle_t xHandle = NULL;
int tasksRunning=0;

void call_loop(void * param){
   loop();
   vTaskDelete( NULL );  
   tasksRunning--;
}

static void stream_handler(){
  if (!configItems.webCaptureOn){
    return;    
  }
  ESP_LOGV(TAG_WEB,"STARTING Stream Handler"); 
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;  
  WiFiClient client = Server.client();
  client.flush();
  if (!client) {
    res = ESP_FAIL;
    ESP_LOGE(TAG_WEB,"WifiClient not found!");
    return;
  }    
  client.println("HTTP/1.0 200 OK" );
  client.println("Connection: keep-alive");
  client.println("Content-Type: multipart/x-mixed-replace;boundary=" PART_BOUNDARY);
  client.println();
  client.println( "--" PART_BOUNDARY);
  sensor_t * s = esp_camera_sensor_get();
  s->set_pixformat(s, PIXFORMAT_JPEG );
  
  unsigned long i =0;
  while(client.connected()){
    if ( i % 25==0 ){
      // perform other tasks every 25 frames
      loop();
    }
    ESP_LOGV(TAG_WEB,"Streaming...");
    fb = esp_camera_fb_get();
    if (!fb) {
      ESP_LOGE(TAG_WEB,"Camera capture failed");
      res = ESP_FAIL;
      return;
    }
    // send a frame (1 JPEG anf follow it by the boundry string)
    client.println( "Content-Type: image/jpeg" );
    //client.println( "" );
    client.printf ( "Content-Length: %u",fb->len );
    client.println("");
    client.println("");
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    size_t sentB =0;
    Serial.print(">");
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        Serial.print("_");
        client.write(fbBuf, 1024);
        fbBuf += 1024;
        sentB += 1024;
      } else if (fbLen%1024>0) {
        Serial.print("+");
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
        sentB += remainder;
      }
      if(client.connected()){
        Serial.print("*");
      }else{
        Serial.print("X");
        client.stop();
        return;
      }
    } // end Buffer loop
    client.flush();
    client.println("");
    client.println("");
    client.println( "--" PART_BOUNDARY);
    Serial.println("<");
    ESP_LOGI(TAG_WEB,"MJPG: %uB",(uint32_t)(fb->len));
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
    }
    ESP_LOGV(TAG_WEB,"Streming LOOP: %3d",i);
    //handle_telegram(NULL);    // call the main loop function and process any telegram messages and normal code events    
    //ESP_LOGV(TAG_WEB,"TASK HADLE Status: %d",eTaskGetState(xHandle));
    // if (tasksRunning==0){
    //   
    //   tasksRunning++;
    //   xTaskCreate(
    //     call_loop        // Task function.
    //     ,"call_loop"     // name of task. 
    //     ,10000                 // Stack size of task 
    //     ,NULL                  // parameter of the task 
    //     ,1                     // priority of the task 
    //     ,&xHandle              // Task handle to keep track of created task 
    //     //,tskNO_AFFINITY        //xCoreID  which core to run this on ( run on whichever core available)
    //   );
    //   ESP_LOGV(TAG_WEB,"Finished Main Loop");
    // }else{
    //   ESP_LOGV(TAG_WEB,"Skipping Main Loop");
    // }
    i++;
  } // end Endless loop
  
  if(res != ESP_OK)
    return;
  client.flush();   
}
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
  ESP_LOGV(TAG_WEB,"populateResolutionsSelects#4");
}
///////////////////////////////////////////////////////////
bool captivePortalStarted(IPAddress ip){
  ESP_LOGV(TAG_WEB,"C.P. started, IP: %s", WiFi.localIP().toString().c_str() );
  ESP_LOGV(TAG_WEB,"acConfig.title: %s", acConfig.title);
  ESP_LOGV(TAG_WEB,"acConfig.apid: %s", acConfig.apid);
  ESP_LOGV(TAG_WEB,"acConfig.psk: %s",acConfig.psk);
  ESP_LOGV(TAG_WEB,"acConfig.hostName: %s",acConfig.hostName);
  ESP_LOGV(TAG_WEB,"acConfig.bootUri: %s",acConfig.bootUri);
  ESP_LOGV(TAG_WEB,"acConfig.homeUri: %s",acConfig.homeUri);
  ESP_LOGV(TAG_WEB,"acConfig.dns1: %s",acConfig.dns1);
  ESP_LOGV(TAG_WEB,"acConfig.dns2: %s",acConfig.dns2);
  ESP_LOGV(TAG_WEB,"acConfig.apip: %s",acConfig.apip);
  ESP_LOGV(TAG_WEB,"acConfig.gateway: %s",acConfig.gateway);
  ESP_LOGV(TAG_WEB,"acConfig.netmask: %s",acConfig.netmask );
  return(true);
}
///////////////////////////////////////////////////////////
String onCapture(AutoConnectAux& aux, PageArgument& args) {
  ESP_LOGV(TAG_WEB,"Portal.where(): %s", Portal.where());
  ESP_LOGV(TAG_WEB,"args.size() %d",args.size());
  return String("");
}
///////////////////////////////////////////////////////////
String onStream(AutoConnectAux& aux, PageArgument& args) {
  ESP_LOGV(TAG_WEB,"Portal.where(): %s", Portal.where());
  ESP_LOGV(TAG_WEB,"args.size() %d",args.size());
  return String("");
}
///////////////////////////////////////////////////////////
String onPage(AutoConnectAux& aux, PageArgument& args) {
  ESP_LOGV(TAG_WEB,"onPage:Portal.where(): %s",Portal.where());
  ESP_LOGV(TAG_WEB,"onPage:args.size(): %d",args.size());
  String result="Success";
  for (int i=0;i<args.args();i++){
    ESP_LOGV(TAG_WEB, "*> %s:%s", args.argName(i).c_str(), String(args.arg(i)).c_str() );
  }
  /*
  AutoConnectElementVT& elements = aux.getElements();
  for (AutoConnectElement& elm : elements) {
    if (elm.typeOf() == AC_Text) {
      AutoConnectText& element = reinterpret_cast<AutoConnectText&>(elm);
      ESP_LOGV(TAG_WEB, "*> %s:%s:%s\n", element.name, element.name, element.value );
    }
    if (elm.typeOf() == AC_Input) {
      AutoConnectInput& element = reinterpret_cast<AutoConnectInput&>(elm);
      ESP_LOGV(TAG_WEB, "*> %s:%s:%s\n", element.name, element.label, element.value );
    }
    if (elm.typeOf() == AC_Select) {
      AutoConnectSelect& element = reinterpret_cast<AutoConnectSelect&>(elm);
      ESP_LOGV(TAG_WEB, "*> %s:%s:%s\n", element.name, element.label, element.selected );
    }
    if (elm.typeOf() == AC_Checkbox) {
      AutoConnectCheckbox& element = reinterpret_cast<AutoConnectCheckbox&>(elm);
      ESP_LOGV(TAG_WEB, "*> %s:%s:%d\n", element.name, element.label, element.checked );
    }
  } 
  */ 
  aux["XCAMERA_MODEL"].as<AutoConnectText>().value= CAMERA_MODEL ;
  #ifndef  I2C_DISPLAY_ADDR
    aux["XscreenOn"].as<AutoConnectCheckbox>().enable=false;
    aux["XscreenFlip"].as<AutoConnectCheckbox>().enable=false;
  #endif
  #ifndef  PIR_PIN
    aux["XmotDetectOn"].as<AutoConnectCheckbox>().enable=false;
  #endif
  #ifndef  SD_CARD_ON
    aux["XsaveToSD"].as<AutoConnectCheckbox>().enable=false;
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
    // [:ADD STRING HERE:]#1
    

    // [:ADD INT HERE:]#1
    configItems.cvIntervalSec=(args.hasArg("XcvIntervalSec")?args.arg("XcvIntervalSec").toInt():0);
    configItems.cvChangePercent=(args.hasArg("XcvChangePercent")?args.arg("XcvChangePercent").toInt():0);
    configItems.timeZone=(args.hasArg("Xtimezone")?args.arg("Xtimezone"):"Europe/Berlin");
    configItems.set_whitebal=(args.hasArg("Xset_whitebal")?args.arg("Xset_whitebal").toInt():0);
    configItems.set_saturation=(args.hasArg("Xset_saturation")?args.arg("Xset_saturation").toInt():0);
    configItems.set_contrast=(args.hasArg("Xset_contrast")?args.arg("Xset_contrast").toInt():0);
    configItems.set_brightness=(args.hasArg("Xset_brightness")?args.arg("Xset_brightness").toInt():0);
    configItems.jpegQuality=(args.hasArg("XjpegQuality")?args.arg("XjpegQuality").toInt():0);
    
    // [:ADD BOOLEAN HERE:]#1    
    configItems.hMirror       =aux["XhMirror"].as<AutoConnectCheckbox>().checked;
    configItems.useFlash      =aux["XuseFlash"].as<AutoConnectCheckbox>().checked;
    configItems.vFlip         =aux["XvFlip"].as<AutoConnectCheckbox>().checked;
    configItems.motionDetectVC=aux["XmotionDetectVC"].as<AutoConnectCheckbox>().checked;
    configItems.alertALL      =aux["XalertALL"].as<AutoConnectCheckbox>().checked;
    configItems.saveToSD      =aux["XsaveToSD"].as<AutoConnectCheckbox>().checked;
    configItems.useDeepSleep  =aux["XuseDeepSleep"].as<AutoConnectCheckbox>().checked;
    configItems.useBuzzer     =aux["XuseBuzzer"].as<AutoConnectCheckbox>().checked;
    configItems.motDetectOn   =aux["XmotDetectOn"].as<AutoConnectCheckbox>().checked;    
    configItems.screenFlip    =aux["XscreenFlip"].as<AutoConnectCheckbox>().checked;
    configItems.screenOn      =aux["XscreenOn"].as<AutoConnectCheckbox>().checked;
        
    ///////////////////////////////////////////////
    ESP_LOGV(TAG_WEB,"saveConfiguration(&configItems);");
    boolean bDirty=saveConfiguration(&configItems);
    ESP_LOGV(TAG_WEB,"applyConfigItem(&configItems);");
    applyConfigItem(&configItems);  
    result="Saved";
  }else if (args.size()==0) { 
    // no arguments wehere provided, then we need to load from configItems into the page
    result="Load Error";
    aux.fetchElement();
    // [:ADD STRING HERE:]#2 
    aux["Xversion"].as<AutoConnectInput>().value=configItems.version;
    aux["bxToken"].as<AutoConnectInput>().value=configItems.botTTelegram;
    aux["XTelegramAdminChatId"].as<AutoConnectInput>().value=configItems.adminChatIds;
    aux["XTelegramUserChatId"].as<AutoConnectInput>().value=configItems.userChatIds;
    aux["XdeviceName"].as<AutoConnectInput>().value=configItems.deviceName;


    // [:ADD BOOLEAN HERE:]#2
    aux["XuseFlash"].as<AutoConnectCheckbox>().checked=configItems.useFlash;
    aux["XhMirror"].as<AutoConnectCheckbox>().checked=configItems.hMirror;
    aux["XvFlip"].as<AutoConnectCheckbox>().checked=configItems.vFlip;    
    aux["XmotionDetectVC"].as<AutoConnectCheckbox>().checked=configItems.motionDetectVC;
    aux["XalertALL"].as<AutoConnectCheckbox>().checked=configItems.alertALL;
    aux["XsaveToSD"].as<AutoConnectCheckbox>().checked=configItems.saveToSD;
    aux["XuseDeepSleep"].as<AutoConnectCheckbox>().checked=configItems.useDeepSleep;
    aux["XuseBuzzer"].as<AutoConnectCheckbox>().checked=configItems.useBuzzer;
    aux["XscreenFlip"].as<AutoConnectCheckbox>().checked=configItems.screenFlip;
    aux["XscreenOn"].as<AutoConnectCheckbox>().checked=configItems.screenOn;
    aux["XmotDetectOn"].as<AutoConnectCheckbox>().checked=configItems.motDetectOn;

    // [:ADD INT HERE:]#2
    aux["XcvIntervalSec"].as<AutoConnectInput>().value=configItems.cvIntervalSec;
    aux["XcvChangePercent"].as<AutoConnectInput>().value=configItems.cvChangePercent;
    aux["XlapseTime"].as<AutoConnectInput>().value=configItems.lapseTime;
    aux["Xset_whitebal"].as<AutoConnectInput>().value=configItems.set_whitebal;
    aux["Xset_saturation"].as<AutoConnectInput>().value=configItems.set_saturation;
    aux["Xset_contrast"].as<AutoConnectInput>().value=configItems.set_contrast;
    aux["Xset_brightness"].as<AutoConnectInput>().value=configItems.set_brightness;
    aux["XjpegQuality"].as<AutoConnectInput>().value=configItems.jpegQuality;
    
    aux["XframeSize"].as<AutoConnectSelect>().select (
        String(resolutions[configItems.frameSize][0]+":"+resolutions[configItems.frameSize][1])
    );
    ESP_LOGV(TAG_WEB,"XframeSize_select: %s, %s",
        resolutions[configItems.frameSize][0],
        resolutions[configItems.frameSize][1]
    );
    aux["XtimeZone"].as<AutoConnectSelect>().select (
        String(configItems.timeZone)
    );
    ESP_LOGV(TAG_WEB,"XtimeZone_select: %s",configItems.timeZone);
    result="Loaded";
  } else{
    result="This Shouldn't happen";
  }
  aux["XResult"].value=result;
  return String("");
}
///////////////////////////////////////////////////////////
String onPageEmail(AutoConnectAux& aux, PageArgument& args) {
  ESP_LOGV(TAG_WEB,"onPageEmail:Portal.where(): %s",Portal.where());
  ESP_LOGV(TAG_WEB,"onPageEmail:args.size(): %d",args.size());
  String result="Success";
  for (int i=0;i<args.args();i++){
    ESP_LOGV(TAG_WEB, "*> %s:%s", args.argName(i).c_str(), String(args.arg(i)).c_str() );
  }
  // the source page is also /teleview and has some args with it, then we need to save it.
  if (Portal.where().equals("/teleViewEmail") && args.size()>0 ){
    result="Save Error";
    configItems.sMTPPort=(args.hasArg("XsMTPPort")?args.arg("XsMTPPort").toInt():0);
    configItems.sMTPPassword=(args.hasArg("XsMTPPassword")?args.arg("XsMTPPassword"):"");
    configItems.sMTPUsername=(args.hasArg("XsMTPUsername")?args.arg("XsMTPUsername"):"");
    configItems.sMTPServer=(args.hasArg("XsMTPServer")?args.arg("XsMTPServer"):"");
    configItems.userEmail=(args.hasArg("XuserEmail")?args.arg("XuserEmail"):"");
    configItems.adminEmail=(args.hasArg("XadminEmail")?args.arg("XadminEmail"):"");

    configItems.sendEmail     =aux["XsendEmail"].as<AutoConnectCheckbox>().checked;
    configItems.sMTPTLS       =aux["XsMTPTLS"].as<AutoConnectCheckbox>().checked;
        
    ///////////////////////////////////////////////
    ESP_LOGV(TAG_WEB,"saveConfiguration(&configItems);");
    boolean bDirty=saveConfiguration(&configItems);
    ESP_LOGV(TAG_WEB,"applyConfigItem(&configItems);");
    applyConfigItem(&configItems);  
    result="Saved";
  }else if (args.size()==0) { 
    // no arguments wehere provided, then we need to load from configItems into the page
    result="Load Error";
    aux.fetchElement();    
    aux["XsMTPPassword"].as<AutoConnectInput>().value=configItems.sMTPPassword;
    aux["XsMTPUsername"].as<AutoConnectInput>().value=configItems.sMTPUsername;
    aux["XsMTPServer"].as<AutoConnectInput>().value=configItems.sMTPServer;
    aux["XuserEmail"].as<AutoConnectInput>().value=configItems.userEmail;
    aux["XadminEmail"].as<AutoConnectInput>().value=configItems.adminEmail;

    aux["XsMTPTLS"].as<AutoConnectCheckbox>().checked=configItems.sMTPTLS;
    aux["XsendEmail"].as<AutoConnectCheckbox>().checked=configItems.sendEmail;
    
    aux["XsMTPPort"].as<AutoConnectInput>().value=configItems.sMTPPort;

    result="Loaded";
  } else{
    result="This Shouldn't happen";
  }
  aux["XResult"].value=result;
  return String("");
}

///////////////////////////////////////////////////////////
/*
The ArduinoJson and Autoconnect libs have limitations about how big the Json Object should be.
It seems that adding any more elements to the AUX_CONFIGPAGE would trigger : 
[AC] Deserialize:NoMemory
Maby the solution for having more configuration is to isolate the elements into more than 1 configuration page

// [:ADD INT HERE:]#3
// [:ADD STRING HERE:]#3
// [:ADD BOOLEAN HERE:]#3
*/
static const char AUX_CONFIGPAGE[] PROGMEM = R"({
  "title": "Options",
  "uri": "/teleView",
  "menu": true,
  "element": [{
        "name": "caption",
        "type": "ACText",
        "value": "<h2>TeleView Configuration</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },{
        "name": "Xversion",
        "type": "ACText",
        "value": "Version"
    },{
        "name": "content",
        "type": "ACText",
        "value": "</br>Please use this page to configure your Teleview settings</br>"
    },{
        "name": "XCAMERA_MODEL",
        "type": "ACText",
        "value": "<CAMERA_MODEL>"
    },{
        "name": "seperator1",
        "type": "ACElement",
        "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    },{
        "name": "sectionBot",
        "type": "ACText",
        "value": "<h2>Telegram Bot Configuration</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },{
        "name": "sectionBot",
        "type": "ACText",
        "value": "<h2>Telegram Bot Configuration</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
    }, {
        "name": "XdeviceName",
        "type": "ACInput",
        "label": "Device Name",
        "placeholder": "TeleView",
        "apply": "text",
        "pattern": "^(.+)$",
        "global": true
    }, {
        "name": "XlapseTime",
        "type": "ACInput",
        "label": "Lapse Time-min",
        "placeholder": "720",
        "pattern": "^([0-9]+)$",
        "global": true,
        "apply": "number"
    }, {
        "name": "bxToken",
        "type": "ACInput",
        "apply": "text",
        "label": "Telegram Bot Token",
        "placeholder": "Bot:Token",
        "pattern": "^(.+)$",
        "global": true
    }, {
        "name": "XTelegramAdminChatId",
        "type": "ACInput",
        "apply": "text",
        "label": "Admin Chat ID",
        "pattern": "^([0-9]+)$",
        "global": true
    }, {
        "name": "XTelegramUserChatId",
        "type": "ACInput",
        "apply": "text",
        "label": "User Chat ID",
        "pattern": "^([0-9]+)$",
        "global": true
    }, {
        "name": "seperator2",
        "type": "ACElement",
        "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    }, {
        "name": "sectionBasic",
        "type": "ACText",
        "value": "<h2>Basic Configuration</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
    }, {
        "name": "XuseFlash",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Use Falsh when snapping a photo",
        "checked": false,
        "global": true
    }, {
        "name": "XhMirror",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Horizontal Mirror",
        "checked": false,
        "global": true
    }, {
        "name": "XvFlip",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Vertical Flip",
        "checked": false,
        "global": true
    }, {
        "name": "XalertALL",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "AlertAll ,MotionDetect & timelapse sent to all chatIds",
        "checked": false,
        "global": true
    }, {
        "name": "XsaveToSD",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Save Photos to SD ",
        "checked": false,
        "global": true
    }, {
        "name": "XuseDeepSleep",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Deep-Sleep when PIR motionDetct or timeLapse",
        "checked": false,
        "global": true
    }, {
        "name": "XuseBuzzer",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Trigger buzzer on motion detect",
        "checked": false,
        "global": true
    }, {
        "name": "XscreenFlip",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Screen Flip",
        "checked": false,
        "global": true
    }, {
        "name": "XscreenOn",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "OLED Display is On",
        "checked": false,
        "global": true
    }, {
        "name": "Xtimezone",
        "type": "ACSelect",
        "label": "Select TZ name",
        "option": [],
        "selected": 0,
        "global": true
    }, {
        "name": "XframeSize",
        "type": "ACSelect",
        "label": "Select Resolution",
        "option": [],
        "selected": 0,
        "global": true
    }, {
        "name": "seperator5",
        "type": "ACElement",
        "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    }, {
        "name": "Xset_whitebal",
        "type": "ACInput",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Photo white balance mode 0-4",
        "global": true,
        "placeholder": "set_whitebal",
        "pattern": "^([+]?([0-4]*))$",
        "apply": "number"
    }, {
        "name": "Xset_saturation",
        "type": "ACInput",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Photo saturation -2 to 2[0]",
        "global": true,
        "placeholder": "set_saturation",
        "pattern": "^([+-]?([0-9]*))$",
        "apply": "number"
    }, {
        "name": "Xset_contrast",
        "type": "ACInput",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Photo Contrast -2 to 2[0]",
        "global": true,
        "placeholder": "set_contrast",
        "pattern": "^([+-]?([0-9]*))$",
        "apply": "number"
    }, {
        "name": "Xset_brightness",
        "type": "ACInput",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Photo Brightness -2 to 2[0]",
        "global": true,
        "placeholder": "set_brightness",
        "pattern": "^([+-]?([0-9]*))$",
        "apply": "number"
    }, {
        "name": "XjpegQuality",
        "type": "ACInput",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "Jpeg Quality 1-62[10-12]",
        "global": true,
        "placeholder": "jpegQuality",
        "pattern": "^([+-]?([0-9]*))$",
        "apply": "number"
    }, {
        "name": "seperator4",
        "type": "ACElement",
        "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    }, {
        "name": "XmotDetectOn",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "PIR Motion Detection Enabled",
        "checked": false,
        "global": true
    }, {
        "name": "XmotionDetectVC",
        "type": "ACCheckbox",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "MotionDetect by Vision not PIR",
        "checked": false,
        "global": true
    }, {
        "name": "XcvIntervalSec",
        "type": "ACInput",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "CV Motion Detect interval between each 2 consecutive photo capture (ms):",
        "global": true,
        "placeholder": "cvIntervalSec",
        "pattern": "^([+-]?([0-9]*))$"
    }, {
        "name": "XcvChangePercent",
        "type": "ACInput",
        "value": "",
        "labelPosition": "AC_Infront",
        "label": "CV Motion Detect Percent change in number of DarkPixels (%):",
        "global": true,
        "placeholder": "cvChangePercent",
        "pattern": "^([+-]?([0-9]*))$"
    }, {
        "name": "seperator4",
        "type": "ACElement",
        "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
    }, {
        "name": "okSubmit",
        "type": "ACSubmit",
        "value": "OK",
        "uri": "/teleView"
    }, {
        "name": "deleteSubmit",
        "type": "ACSubmit",
        "value": "Delete Evertything",
        "uri": "/delete"
    }, {
        "name": "cancel",
        "type": "ACSubmit",
        "value": "Cancel",
        "uri": "/_ac"
    }, {
        "name": "XResult",
        "type": "ACText",
        "value": "XYZ",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
    }
  ]
})";
///////////////////////////////////////////////////////////
static const char AUX_CONFIG_EMAIL_PAGE[] PROGMEM = R"({
"title": "Email",
"uri": "/teleViewEmail",
"menu": true,
"element": [
{
  "name": "sectionEmail",
  "type": "ACText",
  "value": "<h2>Email Configuration</h2>",
  "style": "text-align:center;color:#2f4f4f;padding:10px;"
}, {
  "name": "sectionEmail.1",
  "type": "ACText",
  "value": "<h3>dont use your real email, create a new one</h3>",
  "style": "text-align:center;color:#2fff4f;padding:10px;"
}, {
  "name": "XsendEmail",
  "type": "ACCheckbox",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "Alert by Email.",
  "checked": false,
  "global": true
}, {
  "name": "XsMTPUsername",
  "type": "ACInput",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "SMTP Email username",
  "global": true,
  "placeholder": "sMTPUsername",
  "pattern": "^(.+)$",
  "apply": "text"
}, {
  "name": "XsMTPPassword",
  "type": "ACInput",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "SMTP Email Password",
  "global": true,
  "placeholder": "sMTPPassword",
  "pattern": "^(.+)$",
  "apply": "password"
}, {
  "name": "XsMTPServer",
  "type": "ACInput",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "SMTP Email Server",
  "global": true,
  "placeholder": "sMTPServer",
  "pattern": "^(.+)$",
  "apply": "text"
}, {
  "name": "XsMTPPort",
  "type": "ACInput",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "SMTP Email Port",
  "global": true,
  "placeholder": "sMTPPort",
  "pattern": "^([+-]?([0-9]*))$",
  "apply": "number"
}, {
  "name": "XsMTPTLS",
  "type": "ACCheckbox",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "SMTP TLS/SSL Required",
  "checked": false,
  "global": true
}, {
  "name": "XadminEmail",
  "type": "ACInput",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "Email 1",
  "global": true,
  "placeholder": "adminEmail",
  "pattern": "^(.+)$",
  "apply": "text"
}, {
  "name": "XuserEmail",
  "type": "ACInput",
  "value": "",
  "labelPosition": "AC_Infront",
  "label": "Email 2",
  "global": true,
  "placeholder": "userEmail",
  "pattern": "^(.+)$",
  "apply": "text"
}, {
  "name": "seperator4",
  "type": "ACElement",
  "value": "<hr style='height:1px;border-width:0;color:gray;background-color:#52a6ed'>"
}, {
  "name": "okSubmit",
  "type": "ACSubmit",
  "value": "OK",
  "uri": "/teleViewEmail"
}, {
  "name": "cancel",
  "type": "ACSubmit",
  "value": "Cancel",
  "uri": "/_ac"
}, {
  "name": "XResult",
  "type": "ACText",
  "value": "XYZ",
  "style": "text-align:center;color:#2f4f4f;padding:10px;"
}  
]
})";
///////////////////////////////////////////////////////////
static const char AUX_CAPTURE[] PROGMEM = R"({
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
      "value": "<img src=\"/capture\"/></img>"
    }
  ]
})";
///////////////////////////////////////////////////////////
static const char AUX_STREAM[] PROGMEM = R"({
  "title": "Stream",
  "uri": "/Xstream",
  "menu": true,
  "element": [
    {
      "name": "CCaption",
      "type": "ACText",
      "value": "<h2>TeleView Stream</h2>",
      "style": "text-align:center;color:#2f4f4f;padding:10px;"
    },
    {
      "name": "CCstream",
      "type": "ACElement",
      "value": "<img src=\"/stream\"/></img>"
    }
  ]
})";
///////////////////////////////////////////////////////////
// Redirects from root to the /Xcapture page.
void rootPage() {
  //WiFiWebServer&  webServer = portal.host();
  //Server.send(200,"text/html",content);
  Server.sendHeader("Location", String("http://") + Server.client().localIP().toString() + String("/Xcapture"));
  Server.send(302, "text/plain", "");
  Server.client().flush();
  Server.client().stop();
}
///////////////////////////////////////////////////////////
void capture2Page() {
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
  sprintf(dateTime, "%02d/%02d/%02d, %02d:%02d:%02d",
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
  ESP_LOGV(TAG_WEB,"timeB:%d",(int)timeB);
  ESP_LOGV(TAG_WEB,"currentTimeMills:%d",(int)currentTimeMills);
  tmDiffTM = gmtime (&currentTimeMills);
  sprintf(dateTime, "%02d Years, %02d Months, %02d Days,  %02d Hrs :%02d Min :%02d Sec",
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
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    ESP_LOGE(TAG_WEB,"Failed to obtain time");
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
    fb = esp_camera_fb_get();
    if (!fb) {
      ESP_LOGE(TAG_WEB,"Camera capture failed");
      return;
    } else {
      Server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
      Server.sendHeader("Connection","close");
      Server.sendHeader("Access-Control-Allow-Origin", "*");
      Server.send_P(200,"image/jpeg",(const char *)fb->buf,fb->len);
      ESP_LOGV(TAG_WEB,"STILL-MJPG: %uB ",(uint32_t)(fb->len));
      esp_camera_fb_return(fb);
      fb = NULL;
    }
  }
}
///////////////////////////////////////////////////////////
void capturePageJpeg(){
  if (configItems.webCaptureOn){
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();
    if (!fb) {
      ESP_LOGE(TAG_WEB,"Camera capture failed");
      return;
    } else {
      Server.sendHeader("Content-Disposition", "attachment; filename=capture.jpg");
      Server.sendHeader("Connection","close");
      Server.sendHeader("Access-Control-Allow-Origin", "*");
      Server.send_P(200,"image/jpeg",(const char *)fb->buf,fb->len);
      ESP_LOGV(TAG_WEB,"STILL-MJPG: %uB\n",(uint32_t)(fb->len));
      esp_camera_fb_return(fb);
      fb = NULL;
    }
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
