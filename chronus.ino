#define FS_NO_GLOBALS


#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEClient.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_bt_main.h>
#include "Task.h"
#include <esp_log.h>
#include <JPEGDecoder.h>
#include <FS.h>
//#include "SPIFFS.h" // ESP32 only

struct MESSAGE {
  uint8_t latestMessageID[4];
  String type;
  String title;
  String text;
  String dateTime;
  bool isComplete;
};

static char LOG_TAG[] = "ChonvsANCS";

Task* pApp ;
Task* pHome ;
Task* pConfig ;
Task* pScreenTime ;
Task* pRelogio ;
Task* pDisplayNotification ;
Task* pButtonNotification ;

void setup()
{
  
  wakeUpReason();
}
void loop()
{
  //handleOTA();
}

void killAll(uint8_t id) {
  if (id != 0) if (pHome) pHome->stop();
  if (id != 1) if (pConfig) pConfig->stop();
  if (id != 2) if (pScreenTime) pScreenTime->stop();
  if (id != 3) if (pRelogio) pRelogio->stop();
  if (id != 4) if (pDisplayNotification) pDisplayNotification->stop();
  if (id != 4) if (pButtonNotification) pButtonNotification->stop();
}
