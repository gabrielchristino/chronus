#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEClient.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_bt_main.h>
#include "Task.h"
#include <esp_log.h>

struct MESSAGE {
  uint8_t latestMessageID[4];
  String type;
  String title;
  String text;
  String dateTime;
  bool isComplete;
};

static char LOG_TAG[] = "ChonvsANCS";

void setup()
{
  /*#if defined(BOD_ENABLE)
    init_bod();
  #endif*/
  startScreen();
  //delay(1000);
  Serial.begin(115200);
  SampleSecureServer();
  setButtons();

//  beginOTA();
}
void loop()
{
  //handleOTA();
  touch();
}
