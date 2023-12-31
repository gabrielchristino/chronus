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

TaskHandle_t pApp ;

TaskHandle_t pHome ;
void AppHome (void *data);

TaskHandle_t pConfig ;
void AppConfig(void *data);

TaskHandle_t pAppScreenTime ;
void AppScreenTime(void *data);

TaskHandle_t pRelogio ;
void GetHora (void *data);

TaskHandle_t pDisplayNotification ;
void DisplayNotification (void *data);

TaskHandle_t pButtonNotification ;
void OnNotificationButtons (void *data);

void setup()
{
  
  wakeUpReason();
}
void loop()
{
  //handleOTA();
}
