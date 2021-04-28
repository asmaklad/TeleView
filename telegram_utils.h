#ifndef TELEGRAM_UTILS_H
#define TELEGRAM_UTILS_H

#define TELEGRAM_DEBUG 1
#include <UniversalTelegramBot.h>
#include "esp_camera.h"
#include "persist.h"
#include "webPages.h"

////////////////////////////////////////////////
WiFiClientSecure botClient;
UniversalTelegramBot bot("",botClient);
camera_fb_t *fb = NULL;
size_t currentByte;

boolean bCameraInitiated=false;
boolean bTelegramBotInitiated=false;

boolean bInlineKeyboardResolution=false;
boolean bInlineKeyboardExtraOptions=false;


////////////////////////////////////////////////
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
String keyboardJson = "" ;
// "[[\"/start\", \"/options\"],[\"/CIF\",\"/XGA\",\"/SXGA\",\"/UXGA\"],[\"/vflip\", \"/hmirror\"],[\"/sendPhoto\"]]" ;
boolean bSetLapseMode=false;

////////////////////////////////////////////////
//String sendCapturedImage2Telegram(String chat_id);
String sendCapturedImage2Telegram2(String chat_id);
void handleNewMessages(int numNewMessages);
String formulateKeyboardJson();

bool isMoreDataAvailable();
byte *getNextBuffer();
int getNextBufferLen();
bool dataAvailable = false;
//uint8_t photoNextByte();
////////////////////////////////////////////////
int Counter_isMoreDataAvailable=0;
int Counter_getNextBuffer=0;
int Counter_getNextBufferLen=0;

///////////////////////////////////////////////
String formulateKeyboardJson(){
  
  String lkeyboardJson = "[";
  lkeyboardJson += " [\"/start\", \"/options\"]";
  lkeyboardJson += ",[\"/sendPhoto\"";
  lkeyboardJson +=     ",\"/changeRes\"]";
  lkeyboardJson += ",[\"/moreSettings\"";
  lkeyboardJson +=     ",\"/setlapse\"]";
  lkeyboardJson += "]";
  Serial.print("formulateKeyboardJson:");
  Serial.println(lkeyboardJson);
  return(lkeyboardJson);
}
///////////////////////////////////////////////
String formulateResolutionInlineKeyBoard(){
  String lkeyboardJson = "[";
  String sep="";
  int maxRes=0;
  if(psramFound()){
    maxRes = FRAMESIZE_UXGA;
  } else {
    maxRes = FRAMESIZE_SVGA;
  }
  for (int i=0;i<maxRes+2;i++){
    String checkMark="";
    if (configItems.frameSize == i){
      checkMark="\u2705";
    }
    lkeyboardJson +=  sep+"[{";
    lkeyboardJson += "\"text\":\""+
            checkMark+
            String(resolutions[i][0])+":"+
            String(resolutions[i][1])+
            "\",\"callback_data\":\""+String(resolutions[i][0])+"\"";
    lkeyboardJson +=  "}]";
    sep=",";
  }
  lkeyboardJson += "]";
  Serial.print("formulateResolutionInlineKeyBoard:");
  Serial.println(lkeyboardJson);
  return(lkeyboardJson);
}
///////////////////////////////////////////////
String formulateOptionsInlineKeyBoard(){

  // get more unicodes from here https://apps.timwhitlock.info/emoji/tables/unicode
  String keyboardJson = "["; // start Json
  #if defined(IS_THERE_A_FLASH)
    keyboardJson += "[{ \"text\" : \"The Flash is ";
    keyboardJson += (configItems.useFlash?"ON\u2705":"OFF\u274C");
    keyboardJson += "\", \"callback_data\" : \"/useFlash\" }],";
  #endif

  #if defined(I2C_DISPLAY_ADDR)
    keyboardJson += "[{ \"text\" : \"Screen is ";
    keyboardJson += (configItems.screenOn?"ON\u2705":"OFF\u274C");
    keyboardJson += "\", \"callback_data\" : \"/screenOn\" }],";

    keyboardJson += "[{ \"text\" : \"screen Flip is ";
    keyboardJson += (configItems.screenFlip?"ON\u2705":"OFF\u274C");
    keyboardJson += "\", \"callback_data\" : \"/screenFlip\" }],";
  #endif

  #if defined(PIR_PIN)
    keyboardJson += "[{ \"text\" : \"Motion Detect is ";
    keyboardJson += (configItems.motDetectOn?"ON\u2705":"OFF\u274C");
    keyboardJson += "\", \"callback_data\" : \"/motDetectOn\" }],";
  #endif

  keyboardJson += "[{ \"text\" : \"Camera Flip is ";
  keyboardJson += (configItems.vFlip?"ON\u2705":"OFF\u274C");
  keyboardJson += "\", \"callback_data\" : \"/vFlip\" }],";

  keyboardJson += "[{ \"text\" : \"Camera Mirror is ";
  keyboardJson += (configItems.hMirror?"ON\u2705":"OFF\u274C");
  keyboardJson += "\", \"callback_data\" : \"/hMirror\" }],";

  keyboardJson += "[{ \"text\" : \"Web Capture is ";
  keyboardJson += (configItems.webCaptureOn?"ON\u2705":"OFF\274C");
  keyboardJson += "\", \"callback_data\" : \"/webCaptureOn\" }]";

  keyboardJson += "]";

  Serial.print("formulateOptionsInlineKeyBoard:");
  Serial.println(keyboardJson);
  return(keyboardJson);
}

////////////////////////////////////
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages:BEGIN");
  Serial.println(String(numNewMessages));
  boolean bPrintOptions=true;
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    int message_id = bot.messages[i].message_id;
    Serial.println("ChatID: "+chat_id);
    if (chat_id.equals(configItems.adminChatIds) || chat_id.equals(configItems.userChatIds)) {
      // If the type is a "callback_query", a inline keyboard button was pressed
      if (bot.messages[i].type ==  F("callback_query")) {
        String text = bot.messages[i].text;
        bPrintOptions=false;
        Serial.print("Call back button pressed with text: ");
        Serial.println(text);
        //////////////////////
        //if resolution change
        if ( bInlineKeyboardResolution ){
          if (matchResolutionText(text)){
            configItems.frameSize=(framesize_t) (matchResolutionText(text));
          }
          bot.sendMessageWithInlineKeyboard(chat_id,
           "Select the resolution", 
           "Markdown", 
           formulateResolutionInlineKeyBoard(),
           message_id
           );
        }
        if ( bInlineKeyboardExtraOptions ){
          if (text == "/hMirror") {
            configItems.hMirror = !configItems.hMirror;
          }else if (text == "/vFlip") {
            configItems.vFlip = !configItems.vFlip;
          }else if (text == "/useFlash") {
            configItems.useFlash = !configItems.useFlash;
          }else if (text == "/screenFlip") {
            configItems.screenFlip = !configItems.screenFlip;
          }else if (text == "/screenOn") {
            configItems.screenOn = !configItems.screenOn;
          }else if (text == "/motDetectOn") {
            configItems.motDetectOn = !configItems.motDetectOn;
          }else if (text == "/webCaptureOn") {
            configItems.webCaptureOn = !configItems.webCaptureOn;
          }
          bot.sendMessageWithInlineKeyboard(chat_id, 
            "Change settings:",
            "Markdown", 
            formulateOptionsInlineKeyBoard(),
            message_id
            );
        }
        //////////////////////
        Serial.println("saveConfiguration(configItems);");
        boolean bDirty=saveConfiguration(&configItems);
        if (bDirty){
          Serial.println("applyConfigItem(&configItems);");
          applyConfigItem(&configItems);
        }

      } else {
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
        if (from_name == "") 
          from_name = "Guest";
        Serial.println("TEXT:"+text);
        if (bSetLapseMode) {
          bSetLapseMode=false;
          configItems.lapseTime=(int) text.toInt();
        }
        if (text == "/sendPhoto") {
          Serial.println("handleNewMessages/sendPhoto:BEGIN");
          if(bot.messages[i].type == "channel_post") {
            bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].chat_title + " " + bot.messages[i].text, "");
          } else {
            String result= sendCapturedImage2Telegram2(chat_id);
            Serial.println("result: "+result);
          }
          Serial.println("handleNewMessages/sendPhoto:END");
          bPrintOptions=false;
        }else if(text == "/options") {
          bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the options below", "", formulateKeyboardJson(), true);
        }else if(text == "/changeRes") {
          bPrintOptions=false;
          bInlineKeyboardExtraOptions=false;
          bInlineKeyboardResolution=true;
          bot.sendMessageWithInlineKeyboard(chat_id, "Select the resolution", "", formulateResolutionInlineKeyBoard());
        }else if(text == "/setlapse") {
          bot.sendMessage(chat_id, "Please insert Lapse Time in minutes:", "");
          bSetLapseMode=true;
          bPrintOptions=false;
        }else if(text == "/moreSettings") {
          bPrintOptions=false;
          bInlineKeyboardResolution=false;
          bInlineKeyboardExtraOptions=true;
          bot.sendMessageWithInlineKeyboard(chat_id, "Change settings:", "", formulateOptionsInlineKeyBoard());
        }else if(text == "/restartESP") {
          //ESP.restart();
        }else if (text == "/start") {
          String welcome = "```\n";
          welcome += "*Command*    |*Description*\n";
          welcome += "-----|-----\n";
          welcome += "/start       | sends this message\n";
          welcome += "/options     | returns the reply keyboard\n";
          welcome += "/hMirror     | Camera horizontal MIRROR\n";
          welcome += "/vFlip       | Camera vertical FLIP\n";
          welcome += "/setlapse    | Sets the periodical sending of photo in min (0 is disable)\n";
          welcome += "/webCaptureOn| enables and disbales the /capture.jpg url\n";
  #if defined(IS_THERE_A_FLASH)
          welcome += "/useFlash    | Flash is enabled\n";
  #endif
  #if defined(I2C_DISPLAY_ADDR)
          welcome += "/screenFlip  | Screen flip\n";
          welcome += "/scrrenOn    | Screen enabled\n";
  #endif
  #if defined(PIR_PIN)
          welcome += "/motDetectOn | Motion detection enabled\n";
  #endif
          welcome += "/sendPhoto   | Send a Photo from the camera\n";
          welcome += "/changeRes   | Change the resoltion of the camera\n";
          welcome += "/moreSettings| Access more settings\n";
          welcome += "```";
          Serial.println(welcome);
          bot.sendMessage(chat_id, welcome, "Markdown");
          ////////////////////////////////////////
          bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the options below:", "", formulateKeyboardJson(), true);
        }
      }
      ///////////////////////////////////
      Serial.println("saveConfiguration(configItems);");
      boolean bDirty=saveConfiguration(&configItems);
      if (bDirty){
        Serial.println("applyConfigItem(&configItems);");
        applyConfigItem(&configItems);
      }
      if(bPrintOptions){
        Serial.println("printConfiguration(&configItems);");
        Serial.println(printConfiguration(&configItems,"|","|\n","|"));
        bot.sendMessage(chat_id, printConfiguration(&configItems,"|","|\n","|"), "MarkDown");
        bPrintOptions=false;
        }
    } else {
      bot.sendMessage(chat_id, 
      " \'" + bot.messages[i].text + "\' from "+ chat_id
      , "");
    }
  }
  Serial.println("handleNewMessages:END");
}

///////////////////////////////////////////////

String sendCapturedImage2Telegram2(String chat_id) {
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";

  camera_fb_t * fb = NULL;

#if defined(FLASH_LAMP_PIN)
  if (configItems.useFlash){
    Serial.println("Switching Flash-lamp ON");
    digitalWrite(FLASH_LAMP_PIN, HIGH);
    delay(250);
    Serial.println("The Flash-lamp is ON");
  }
#endif

#if defined(USE_OLED_AS_FLASH)
  if (configItems.useFlash){
    Serial.println("Switching Flash-OLED ON");
    display_AllWhite();
    delay(250);
    Serial.println("Flash-OLED is ON");
  }
#endif

  Serial.println("Capture Photo");
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }
  int fb_width=fb->width;
  int fb_height=fb->height;
  Serial.println("sendChatAction#1");
  bot.sendChatAction(chat_id, "upload_photo");
  // reset the client connection
  if (botClient.connected()) {
    #ifdef TELEGRAM_DEBUG
        Serial.println(F("Closing client"));
    #endif
    botClient.stop();
  }
  // Connect with api.telegram.org if not already connected
  if (!botClient.connected()) {
    #ifdef TELEGRAM_DEBUG
        Serial.println(F("[BOT Client]Connecting to server"));
    #endif
    if (!botClient.connect(TELEGRAM_HOST, TELEGRAM_SSL_PORT)) {
      #ifdef TELEGRAM_DEBUG
        Serial.println(F("[BOT Client]Conection error"));
      #endif
    }
  }

  String head = "--Taiwan\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chat_id + "\r\n--Taiwan\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
  String tail = "\r\n--Taiwan--\r\n";

  uint16_t imageLen = fb->len;
  uint16_t extraLen = head.length() + tail.length();
  uint16_t totalLen = imageLen + extraLen;

  botClient.println("POST /bot"+configItems.botTTelegram+"/sendPhoto HTTP/1.1");
  botClient.println("Host: " + String(myDomain));
  botClient.println("Content-Length: " + String(totalLen));
  botClient.println("Content-Type: multipart/form-data; boundary=Taiwan");
  botClient.println("Connection: keep-alive");
  botClient.println();
  botClient.print(head);

  uint8_t *fbBuf = fb->buf;
  size_t fbLen = fb->len;
  for (size_t n=0;n<fbLen;n=n+1024) {
    if (n+1024<fbLen) {
      botClient.write(fbBuf, 1024);
      fbBuf += 1024;
    }
    else if (fbLen%1024>0) {
      size_t remainder = fbLen%1024;
      botClient.write(fbBuf, remainder);
    }
  }

  botClient.print(tail);
  esp_camera_fb_return(fb);

  int waitTime = 10000;   // timeout 10 seconds
  long startTime = millis();
  boolean state = false;

  while ((startTime + waitTime) > millis())
  {
    Serial.print(".");
    delay(100);
    while (botClient.available())
    {
        char c = botClient.read();
        if (state==true) getBody += String(c);
        if (c == '\n')
        {
          if (getAll.length()==0) state=true;
          getAll = "";
        }
        else if (c != '\r')
          getAll += String(c);
        startTime = millis();
     }
     if (getBody.length()>0) break;
  }
  Serial.println(getBody);
  Serial.println();
  PICTURES_COUNT++;
  botClient.stop();
#if defined(FLASH_LAMP_PIN)
  if (configItems.useFlash){
    delay(10);
    digitalWrite(FLASH_LAMP_PIN, LOW); 
    Serial.println("Flash-lamp OFF");
  }
#endif

#if defined(USE_OLED_AS_FLASH)
  display_Clear();
#endif
  bot.sendMessage(chat_id, String("Photo sent:"+String(fb_width)+"x"+String(fb_height))+","+String(fbLen)+"b","" );
  return("success");
}


#endif //TELEGRAM_UTILS_H
