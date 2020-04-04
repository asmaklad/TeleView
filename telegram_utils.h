#ifndef TELEGRAM_UTILS_H
#define TELEGRAM_UTILS_H

#include "esp_camera.h"
#include "persist.h"
#include "webPages.h"

////////////////////////////////////////////////
WiFiClientSecure botClient;
UniversalTelegramBot bot(botClient);
camera_fb_t * fb;
size_t currentByte;

boolean bCameraInitiated=false;
boolean bTelegramBotInitiated=false;

////////////////////////////////////////////////
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
String keyboardJson = "" ;
// "[[\"/start\", \"/options\"],[\"/CIF\",\"/XGA\",\"/SXGA\",\"/UXGA\"],[\"/vflip\", \"/hmirror\"],[\"/sendPhoto\"]]" ;
boolean bSetLapseMode=false;

////////////////////////////////////////////////
String sendCapturedImage2Telegram(String chat_id);
void handleNewMessages(int numNewMessages);
bool isMoreDataAvailable();
uint8_t photoNextByte();
String formulateKeyboardJson();
////////////////////////////////////////////////
String formulateKeyboardJson(){
  String lkeyboardJson = "[";
  lkeyboardJson += "[\"/start\", \"/options\"]";
  lkeyboardJson += ",[\"/sendPhoto\"]";  
  lkeyboardJson += ",[\"/vflip\", \"/hmirror\", \"/setlapse\", \"/webCaptureOn\"]";
#if defined(I2C_DISPLAY_ADDR)
  lkeyboardJson += ",[\"/screenOn\",\"/screenFlip\"]";
#endif
#if defined(IS_THERE_A_FLASH)
  lkeyboardJson += ",[\"/useFlash\"]";
#endif
#if defined(PIR_PIN) 
  lkeyboardJson += ",[\"/motDetectOn\"]";
#endif  
  lkeyboardJson += ",[";
  String sep="";
  for (int i=0;i<12;i++){
    lkeyboardJson += sep+"\"/"+String(resolutions[i][0])+"\"";
    sep=",";
    if ( !((i+1)%6) ){
      sep="";
      lkeyboardJson +=  "],[";
    }
  }
  lkeyboardJson += "]";

  lkeyboardJson += "]";  
  Serial.println(lkeyboardJson);
  return(lkeyboardJson);
}

String sendCapturedImage2Telegram(String chat_id) {  
  Serial.println("sendChatAction#1");
  bot.sendChatAction(chat_id, "upload_photo");  
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
  timeOfLastPhoto=time(NULL);
  Serial.printf("len: %d, width: %d, height: %d\n",fb->len,fb->width,fb->height);
  if (!fb) {
      Serial.println("Camera Capture Failed");
      bot.sendMessage(chat_id, "Camera Capture Failed", "");
      return "FAIL";
  }  
  currentByte = 0;  
  int fblen= (int) fb->len;  
  Serial.printf("fb size %d \n",fblen);
  bot.sendChatAction(chat_id, "upload_document");
  String sent = bot.sendPhotoByBinary(chat_id, "image/jpeg", fblen, isMoreDataAvailable, photoNextByte);  
  Serial.println("PhotoSent:"+sent);
  PICTURES_COUNT++;
  esp_camera_fb_return(fb);
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
  bot.sendMessage(chat_id, "Photo sent","" );
  return("success");
}
////////////////////////////////////
bool isMoreDataAvailable() {
  int res=(fb->len) - currentByte;  
  return ( res );
}
////////////////////////////////////
uint8_t photoNextByte() {
  currentByte++;  
  return (fb->buf[currentByte - 1]);
}
////////////////////////////////////
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages:BEGIN");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);    
    Serial.println("ChatID: "+chat_id);    
    if (chat_id.equals(configItems.adminChatIds)) {
      String text = bot.messages[i].text;
      String from_name = bot.messages[i].from_name;
      if (from_name == "") 
        from_name = "Guest";
      Serial.println("TEXT:"+text);
      boolean bPrintOptions=true;
      if (bSetLapseMode) {
        bSetLapseMode=false;
        configItems.lapseTime=(int) text.toInt();
      }
      if (text == "/sendPhoto") {
        Serial.println("handleNewMessages/sendPhoto:BEGIN");
        if(bot.messages[i].type == "channel_post") {
          bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].chat_title + " " + bot.messages[i].text, "");
        } else {
          String restult= sendCapturedImage2Telegram(chat_id);
          Serial.println("restult: "+restult);        
        }        
        Serial.println("handleNewMessages/sendPhoto:END");
        bPrintOptions=false;
      }else if (text == "/hmirror") {
        configItems.hMirror = !configItems.hMirror;        
      }else if (text == "/vflip") {
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
      }else if(text == "/options") {
        bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the options below", "", keyboardJson, true);
      }else if(text == "/setlapse") {
        bot.sendMessage(chat_id, "Please insert Lapse Time in minutes:", "");
        bSetLapseMode=true;
        bPrintOptions=false;
      }else if (text == "/start") {
        String welcome = "```\n";   
        welcome += "*Command*    |*Description*\n";        
        welcome += "/start       | sends this message\n";
        welcome += "/options     | returns the reply keyboard\n";
        welcome += "/hmirror     | Camera Horizontal MIRROR\n";
        welcome += "/vflip       | Camera Vertical FLIP\n";
        welcome += "/setlapse    | Sets the periodical sending of photo in min (0 is disable)\n";
        welcome += "/webCaptureOn| enables and disbales the /capture.jpg url\n";
#if defined(IS_THERE_A_FLASH)
        welcome += "/useFlash    | Flash is Enabled\n";
#endif        
#if defined(I2C_DISPLAY_ADDR)        
        welcome += "/screenFlip  | Screen Flip\n";
        welcome += "/scrrenOn    | Screen Enabled\n";
#endif
#if defined(PIR_PIN)        
        welcome += "/motDetectOn | Motion Detection Enabled\n";
#endif
        welcome += "/sendPhoto   | Send a Photo from the Camera\n";
        welcome += "```";
        Serial.println(welcome);
        bot.sendMessage(chat_id, welcome, "Markdown");
        ///////////////////////////////////////
        String welcome2="```\n";
        String sep="";
        for(int i=0;i<12;i++){
          welcome2 += sep+"/"+resolutions[i][0]+" | "+resolutions[i][1];
          Serial.println(welcome2.length());
          sep="\n";
          if (welcome2.length()>4000){
            welcome2 += "```";
            Serial.println(welcome2);
            bot.sendMessage(chat_id, welcome2, "Markdown");
            welcome2="";
            sep="";
          }
        }
        welcome2 += "```";
        Serial.println(welcome2);
        bot.sendMessage(chat_id, welcome2, "Markdown");
        ////////////////////////////////////////
        bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the options belowX", "", keyboardJson, true);
      }else if (matchResolutionText(text)){
        configItems.frameSize=(framesize_t) (matchResolutionText(text)-1);        
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
        Serial.println(printConfiguration(&configItems,"\t"));
        bot.sendMessage(chat_id, printConfiguration(&configItems), "Markdown");
        bPrintOptions=false;
      }
    }else{
      bot.sendMessage(chat_id, 
       " \'" + bot.messages[i].text + "\' from "+ chat_id
       , "");
    }
  }
  Serial.println("handleNewMessages:END");
}


#endif //TELEGRAM_UTILS_H
