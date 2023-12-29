#include <stack>
#include <map>

static BLEUUID ancsServiceUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0");
static BLEUUID notificationSourceCharacteristicUUID("9FBF120D-6301-42D9-8C58-25E699A21DBD");
static BLEUUID controlPointCharacteristicUUID("69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9");
static BLEUUID dataSourceCharacteristicUUID("22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB");

uint8_t latestMessageID[4];
boolean pendingNotification = false;
boolean incomingCall = false;


std::map<uint32_t, MESSAGE*> notificationList;

BLERemoteCharacteristic* pDataSourceCharacteristic;

BLERemoteCharacteristic* pControlPointCharacteristic;

class MySecurity : public BLESecurityCallbacks {

    uint32_t onPassKeyRequest() {
      return 123456;
    }

    void onPassKeyNotify(uint32_t pass_key) {
    }

    bool onSecurityRequest() {
      return true;
    }

    bool onConfirmPIN(unsigned int) {
      return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
      if (cmpl.success) {
        uint16_t length;
        esp_ble_gap_get_whitelist_size(&length);
      }
    }
};

static void acceptCallF(BLERemoteCharacteristic* pControlPointCharacteristic) {
  const uint8_t vResponse[] = {0x02,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x00};
  pControlPointCharacteristic->writeValue((uint8_t*)vResponse, 6, true);
}

static void endCall(BLERemoteCharacteristic* pControlPointCharacteristic) {
  const uint8_t vResponse[] = {0x02,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x01};
  pControlPointCharacteristic->writeValue((uint8_t*)vResponse, 6, true);
  incomingCall = false;
}

std::map<uint32_t, MESSAGE*>::iterator it(uint32_t uuid) {
  return notificationList.find(uuid);
}

bool hasNotifications() {
  return notificationList.size() > 0;
}

void removeNotification(uint32_t uuid) {
  if (hasNotifications()) notificationList.erase(it(uuid));
}

std::map<uint32_t, MESSAGE*>::iterator getLastNotification() {
  std::map<uint32_t, MESSAGE*>::iterator it = --notificationList.end();
  for (uint8_t i = 0; i < 4 ; i++) latestMessageID[i] = it->second->latestMessageID[i];
  return it;
}

class OnNotificationButtons: public Task {
    void run(void *data) {
      Serial.println("btn notifi");
      for (;;) {
        delay(100);
        //Serial.println("ble");
        btnUp.click([]{acceptCallF(pControlPointCharacteristic);});
        btnDown.click([]{endCall(pControlPointCharacteristic);});
        btnOk.click(showLastNotification);
      }
    }
};

void showLastNotification() {
  if (hasNotifications()) {
    std::map<uint32_t, MESSAGE*>::iterator it = getLastNotification();
    //displayNotification(it->second);
    
    if (pDisplayNotification) pDisplayNotification->stop();
    pDisplayNotification = new DisplayNotification();
    pDisplayNotification->setStackSize(1500);
    pDisplayNotification->start(it->second);

    
    pButtonNotification = new OnNotificationButtons();
    pButtonNotification->setStackSize(1500);
    pButtonNotification->start();
    killAll(4);

  } else {
    clearScreen();
    delay(500);
    callHome();
  }
}

MESSAGE *getNotification(uint32_t uuid) {
  return (it(uuid) != notificationList.end()) ? it(uuid)->second : new MESSAGE();
}

void addNotification(uint32_t uuid, MESSAGE *notification) {
  if (it(uuid)->second == notification) {
    removeNotification(uuid);
  }

  notificationList.insert(std::pair<uint32_t, MESSAGE*>(uuid, notification));
}

static void dataSourceNotifyCallback(
  BLERemoteCharacteristic* pDataSourceCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  std::string message;

  uint32_t messageId = pData[4];
  messageId = messageId << 8 | pData[3];
  messageId = messageId << 16 | pData[2];
  messageId = messageId << 24 | pData[1];

  for (int i = 8; i < length; i++)
  {
    message += (char)pData[i];
  }

  MESSAGE *notification = getNotification(messageId);

  if (!notification->isComplete) {
    switch (pData[5])
    {
      case 0x0:
        notification->type = message.c_str();
        break;
      case 0x1:
        notification->title = message.c_str();
        break;
      case 0x3:
        notification->text = message.c_str();
        break;
      case 0x5:
        notification->dateTime = message.c_str();
        break;
    }
  }
  if (!notification->isComplete && String(notification->title) != "" && String(notification->text) != "" && String(notification->dateTime) != "") {
    notification->isComplete = true;

    showLastNotification();

    if (notification->title == "ChronusSet") {
      String text = notification->text.c_str();
      if (text.indexOf("ChronusSetDateTime") > -1) setHora(notification->dateTime);
      if (text.indexOf("ScreenTime") > -1) screenTime = text.substring(text.indexOf("ScreenTime") + 11).toInt();
    }
  }

  addNotification(messageId, notification);
}
static void NotificationSourceNotifyCallback(
  BLERemoteCharacteristic* pNotificationSourceCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify)
{
  uint32_t messageId;
  messageId = pData[7];
  messageId = messageId << 8 | pData[6];
  messageId = messageId << 16 | pData[5];
  messageId = messageId << 24 | pData[4];

  if (pData[0] == 0)
  {
    MESSAGE *notification = getNotification(messageId);

    for (uint8_t i = 0; i < 4 ; i++) {
      latestMessageID[i] = pData[i + 4];
      notification->latestMessageID[i] = pData[i + 4];
    }

    addNotification(messageId, notification);
  }
  else if (pData[0] == 1)
  {
    if (pData[2] == 1) {
      incomingCall = true;
    }
  }
  else if (pData[0] == 2)
  {
    removeNotification(messageId);
    showLastNotification();

    if (pData[2] == 1) {
      incomingCall = false;
    }
  }
  pendingNotification = true;
}

/**
   Become a BLE client to a remote BLE server.  We are passed in the address of the BLE server
   as the input parameter when the task is created.
*/
class MyClient: public Task {
    void run(void* data) {

      BLEAddress* pAddress = (BLEAddress*)data;
      BLEClient*  pClient  = BLEDevice::createClient();
      BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
      BLEDevice::setSecurityCallbacks(new MySecurity());

      BLESecurity *pSecurity = new BLESecurity();
      pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
      pSecurity->setCapability(ESP_IO_CAP_IO);
      pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
      // Connect to the remove BLE Server.
      pClient->connect(*pAddress);

      /** BEGIN ANCS SERVICE **/
      // Obtain a reference to the service we are after in the remote BLE server.
      BLERemoteService* pAncsService = pClient->getService(ancsServiceUUID);
      if (pAncsService == nullptr) {
        return;
      }
      // Obtain a reference to the characteristic in the service of the remote BLE server.
      BLERemoteCharacteristic* pNotificationSourceCharacteristic = pAncsService->getCharacteristic(notificationSourceCharacteristicUUID);
      if (pNotificationSourceCharacteristic == nullptr) {
        return;
      }
      // Obtain a reference to the characteristic in the service of the remote BLE server.
      pControlPointCharacteristic = pAncsService->getCharacteristic(controlPointCharacteristicUUID);
      if (pControlPointCharacteristic == nullptr) {
        //ESP_LOGI(LOG_TAG, "Failed to find our characteristic UUID: %s", controlPointCharacteristicUUID.toString().c_str());
        return;
      }
      // Obtain a reference to the characteristic in the service of the remote BLE server.
      pDataSourceCharacteristic = pAncsService->getCharacteristic(dataSourceCharacteristicUUID);
      if (pDataSourceCharacteristic == nullptr) {
        //ESP_LOGI(LOG_TAG, "Failed to find our characteristic UUID: %s", dataSourceCharacteristicUUID.toString().c_str());
        return;
      }
      const uint8_t v[] = {0x1, 0x0};
      pDataSourceCharacteristic->registerForNotify(dataSourceNotifyCallback);
      pDataSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v, 2, true);
      pNotificationSourceCharacteristic->registerForNotify(NotificationSourceNotifyCallback);
      pNotificationSourceCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v, 2, true);
      /** END ANCS SERVICE **/

      while (1) {


        if (pendingNotification || incomingCall) {
          // CommandID: CommandIDGetNotificationAttributes
          // 32bit uid
          // AttributeID
          // //Serial.println("Requesting details...");
          const uint8_t vIdentifier[] = {0x0,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x0};
          pControlPointCharacteristic->writeValue((uint8_t*)vIdentifier, 6, true);
          const uint8_t vTitle[] = {0x0,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x1, 0x0, 0x10};
          pControlPointCharacteristic->writeValue((uint8_t*)vTitle, 8, true);
          const uint8_t vMessage[] = {0x0,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x3, 0x0, 0x10};
          pControlPointCharacteristic->writeValue((uint8_t*)vMessage, 8, true);
          const uint8_t vDate[] = {0x0,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x5};
          pControlPointCharacteristic->writeValue((uint8_t*)vDate, 6, true);

          pendingNotification = false;
        }

        delay(100); //does not work without small delay
      }
    } // run
}; // MyClient
