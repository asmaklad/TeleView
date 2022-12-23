#ifndef MOTION_DETECT_H
#define MOTION_DETECT_H


/*
add this to identify and control the logging per module
static const char* TAG = "MOTIONDETECT";
* macros:
  ESP_LOGE - error
  ESP_LOGW - warning
  ESP_LOGI - info
  ESP_LOGD - debug
  ESP_LOGV - verbose
* Ex.: ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);
* Additionally there is an _EARLY_ variant for each of these macros (e.g. ESP_EARLY_LOGE ).
* To override default verbosity level at file or component scope
  #define LOG_LOCAL_LEVEL ESP_ESP_LOGVERBOSE
* LOG LEVELS enum:
  ESP_LOG_NONE    // No log output
  ESP_LOG_ERROR   // Critical errors, software module can not recover on its own
  ESP_LOG_WARN    // Error conditions from which recovery measures have been taken
  ESP_LOG_INFO    // Information messages which describe normal flow of events
  ESP_LOG_DEBUG   // Extra information which is not necessary for normal use (values, pointers, sizes, etc).
  ESP_ESP_LOGVERBOSE // Bigger chunks of debugging information, or frequent messages which can potentially flood the output.
* To configure logging output per module at runtime, add calls to esp_log_level_set function:
  esp_log_level_set("*", ESP_LOG_ERROR);        // set all components to ERROR level
  esp_log_level_set("wifi", ESP_LOG_WARN);      // enable WARN logs from WiFi stack
  esp_log_level_set("dhcpc", ESP_LOG_INFO);     // enable INFO logs from DHCP client  
* to log HEX/CHAR and Binary Data:
  ESP_LOG_BUFFER_HEX_LEVEL(tag, buffer, buff_len, level)  // Log a buffer of hex bytes at specified level, seprated into 16 bytes each line.
  ESP_LOG_BUFFER_CHAR_LEVEL(tag, buffer, buff_len, level) // Log a buffer of characters at specified level, seprated into 16 bytes each line. Buffer should contain only printable characters.
  ESP_LOG_BUFFER_HEXDUMP(tag, buffer, buff_len, level)    // Dump a buffer to the log at specified level.
* ESP32 Arduino also supports five levels but its macros are all lower-case like so:
    log_e – error (lowest)
    log_w – warning
    log_i – info
    log_d – debug
    ESP_LOGV – verbose (highest)
  The ESP32 Arduino Core supports but redefined the ESP-IDF macros. 
  It simply routes them to their Arduino counter part. Hence, ESP_LOGE is routed to log_e.
  Please note that their macros don’t enforce the use of a tag for each message to group them.
*/
// Declarations

static const char* TAG_MD = "MOTIONDETECT";
#include "esp_log.h"
#include "sensor.h"
#include "esp_camera.h"

unsigned long lastSnap=0;
unsigned long timeBetweenSnaps=500;

struct frameData {
 size_t frameSize =1;  
 size_t darkPixelsCount=1;
 size_t lightPixelsCount=1;
 size_t grayPixelsCount=1;
 uint8_t     maxValue=0;
 uint8_t     minValue=255;
} frameDataCurrent, frameDataPrev;

char strBuffMD[1000];

void printFrameData( struct frameData fd , char strBuffMD[] );
bool checkMotion(bool prevResult, framesize_t origFrameSize, bool prevChechResult );
float calcFrameDataDiff (int pixelsCountCurrent,int pixelsCountPrev, int totalPixels);
////////////////////////////////////////

void printFrameData( struct frameData fd , char strBuffMD[]) {
  sprintf(strBuffMD,
      "maxValue: %3d, minValue: %3d, darkPixels: %5d, lightPixels: %5d, grayPixels: %5d, frameSize: %5d",
      fd.maxValue,
      fd.minValue,
      fd.darkPixelsCount,
      fd.lightPixelsCount,
      fd.grayPixelsCount,
      fd.frameSize
  );  
}
////////////////////////////////////////
float calcFrameDataDiff (int pixelsCountCurrent,int pixelsCountPrev, int totalPixels){
  int diff = pixelsCountCurrent - pixelsCountPrev ;
  int absInt = abs(diff);
  float diffPercent = (float) 100.0* absInt/totalPixels ;
  ESP_LOGV(TAG_MD,"PixelsCount diff: %d, PixelsCount absInt: %d, PixelsCount diffPercent: %f, cvChangePercent: %d\%,cvIntervalSec: %d ",
        diff,
        absInt,
        diffPercent,
        configItems.cvChangePercent,
        configItems.cvIntervalSec);
  //Serial.printf("\t %d\n",configItems.cvChangePercent);
  return(diffPercent);
}
////////////////////////////////////////
bool checkMotion(bool prevResult, framesize_t origFrameSize  ,bool prevChechResult){
  bool result=false;
  
  if ( (millis()-lastSnap) <  configItems.cvIntervalSec ){
    return(false);
  }
  //*//
  // sensor adjest
  sensor_t * s = esp_camera_sensor_get();
  /*
  1. GRAYSCALE means values from 0 to 255 for each pixel
  2. RGB565 means that a 16 bit value is per pixel and is comprised from 5 bits red, 6 bits green and 5 bits blue.
  3. RGB888 means that there are 3 bytes per pixel, one for red, one for green and one for blue. Each vary between 0 and 255.
  4. YUV means that each pixel has it's Y channel as a separate byte (0 to 255) and each two adjacent pixels share their U and V values.
  6. JPEG means that the image is encoded into JPEG format either by the camera itself or in software.
  */
  s->set_pixformat(s, PIXFORMAT_GRAYSCALE );
  s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_special_effect(s,3);
  s->set_colorbar(s, 1);
  //*/
  camera_fb_t* frame = esp_camera_fb_get();
  if (!frame) {
      ESP_LOGV(TAG_MD,"Failed capture");
      delay(3000);
      return(false);
  }
  ESP_LOGV(TAG_MD, "WIDTH:%d,HEIGHT:%d",frame->width,frame->height);
  
  // do the BIG thing START %%%%%%%%%%%%%%%%%%%%
  // IDEA 2: compare number of dark pixes between the two GRAYSLACE pictures
  
  esp_camera_fb_return(frame);

  frameDataCurrent.frameSize = frame->len;
  size_t darkPixels=0;
  size_t lightPixels=0;
  size_t grayPixels=0;
  uint8_t  maxValue=0;
  uint8_t  minValue=255;
  for (size_t i=0;i<frame->len;i++){
    maxValue=( (frame->buf[i] > maxValue) ?frame->buf[i]:maxValue );
    minValue=( (frame->buf[i] < minValue) ?frame->buf[i]:minValue );
    //if ( frame->buf[i] > maxValue) maxValue=frame->buf[i];
    /*/ debug Efforts
    Serial.printf("[%d]: %d ",i,frame->buf[i]);
    if ( (i % 8) ==0){
      ESP_LOGV(TAG_MD,("");      
    }
    //*/
    int lightMargin=85;
    if ( frame->buf[i] < lightMargin ) {
      lightPixels++;
    } else if ( frame->buf[i] > 255-lightMargin  ) {
      darkPixels++;
    } else {
      grayPixels++;
    }
  }
  frameDataCurrent.darkPixelsCount = darkPixels;
  frameDataCurrent.lightPixelsCount = lightPixels;
  frameDataCurrent.grayPixelsCount = grayPixels;
  frameDataCurrent.maxValue = maxValue;

  // night and Day 
  if (darkPixels > lightPixels )  {
    ESP_LOGV(TAG_MD,"it is Night time !");
  } else {
    ESP_LOGV(TAG_MD,"it is Day time !");
  }
  
  printFrameData(frameDataCurrent,strBuffMD);
  ESP_LOGV(TAG_MD, "frameDataCurrent:%s",strBuffMD);
  printFrameData(frameDataPrev,strBuffMD);
  ESP_LOGV(TAG_MD, "frameDataPrev   :%s",strBuffMD);

  ESP_LOGV(TAG_MD,"diffPercent_dark:");
  float diffPercent_dark  =calcFrameDataDiff(frameDataCurrent.darkPixelsCount,frameDataPrev.darkPixelsCount,frameDataCurrent.frameSize);
  ESP_LOGV(TAG_MD,"diffPercent_light:");
  float diffPercent_light =calcFrameDataDiff(frameDataCurrent.lightPixelsCount,frameDataPrev.lightPixelsCount,frameDataCurrent.frameSize);
  ESP_LOGV(TAG_MD,"diffPercent_gray:");
  float diffPercent_gray  =calcFrameDataDiff(frameDataCurrent.grayPixelsCount,frameDataPrev.grayPixelsCount,frameDataCurrent.frameSize);
           
  // if diff between two frames more than 10%
  if ( diffPercent_dark   >= (float) configItems.cvChangePercent || 
       diffPercent_light  >= (float) configItems.cvChangePercent ||
       diffPercent_gray   >= (float) configItems.cvChangePercent )  {
    result=true;
    ESP_LOGV(TAG_MD,"Change Detected !!");
  } else {
    result=false;
  }
  // do the BIG thing END  %%%%%%%%%%%%%%%%%%%%
  // copy Current into Prev ( present always becomes past)
  memcpy(&frameDataPrev,&frameDataCurrent, sizeof(frameData));
  //*//
  // return to origional state
  s->set_pixformat(s, PIXFORMAT_JPEG );
  s->set_framesize(s, origFrameSize); 
  s->set_special_effect(s,0);
  lastSnap=millis();
  //*/  
  //don't send two snapshots only one.
  //but make already a snapshot.
  if (prevChechResult){
    return (false);
  }
  return(result);
}



////////////////////////////////////////
#endif //CAMERA_PINS_H
