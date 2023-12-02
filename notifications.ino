#include <stack>
#include <map>

static BLEUUID ancsServiceUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0");
static BLEUUID notificationSourceCharacteristicUUID("9FBF120D-6301-42D9-8C58-25E699A21DBD");
static BLEUUID controlPointCharacteristicUUID("69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9");
static BLEUUID dataSourceCharacteristicUUID("22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB");

uint8_t latestMessageID[4];
boolean pendingNotification = false;
boolean incomingCall = false;
uint8_t acceptCall = 0;

std::stack<MESSAGE> listMessage;

std::map<uint32_t, MESSAGE*> notificationList;

BLERemoteCharacteristic* pDataSourceCharacteristic;

BLERemoteCharacteristic* pControlPointCharacteristic;

class MySecurity : public BLESecurityCallbacks {

    uint32_t onPassKeyRequest() {
      //ESP_LOGI(LOG_TAG, "PassKeyRequest");
      return 123456;
    }

    void onPassKeyNotify(uint32_t pass_key) {
      //ESP_LOGI(LOG_TAG, "On passkey Notify number:%d", pass_key);
    }

    bool onSecurityRequest() {
      //ESP_LOGI(LOG_TAG, "On Security Request");
      return true;
    }

    bool onConfirmPIN(unsigned int) {
      //ESP_LOGI(LOG_TAG, "On Confrimed Pin Request");
      return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
      //ESP_LOGI(LOG_TAG, "Starting BLE work!");
      if (cmpl.success) {
        uint16_t length;
        esp_ble_gap_get_whitelist_size(&length);
        //ESP_LOGI(LOG_TAG, "size: %d", length);
      }
    }
};

    static void acceptCallF(BLERemoteCharacteristic* pControlPointCharacteristic) {
      const uint8_t vResponse[] = {0x02,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x00};
      pControlPointCharacteristic->writeValue((uint8_t*)vResponse, 6, true);

      acceptCall = 0;
      //incomingCall = false;
    }

    static void endCall(BLERemoteCharacteristic* pControlPointCharacteristic) {
      const uint8_t vResponse[] = {0x02,   latestMessageID[0], latestMessageID[1], latestMessageID[2], latestMessageID[3],   0x01};
      pControlPointCharacteristic->writeValue((uint8_t*)vResponse, 6, true);

      acceptCall = 0;
      incomingCall = false;
    }

std::map<uint32_t, MESSAGE*>::iterator it(uint32_t uuid) {
  return notificationList.find(uuid);
}

void removeNotification(uint32_t uuid) {
  if (notificationList.size() > 0) notificationList.erase(it(uuid));
}

std::map<uint32_t, MESSAGE*>::iterator getLastNotification() {
  std::map<uint32_t, MESSAGE*>::iterator it = --notificationList.end();

  latestMessageID[0] = it->second->latestMessageID[0];
  latestMessageID[1] = it->second->latestMessageID[1];
  latestMessageID[2] = it->second->latestMessageID[2];
  latestMessageID[3] = it->second->latestMessageID[3];

  return it;
}

/*void filterNotification() {
  std::map<uint32_t, MESSAGE*>::iterator it = getLastNotification();
  if(it->second->title.indexOf("ChronusSet")> -1) {
    endCall(pControlPointCharacteristic);
  }
}*/

void showLastNotification() {
  if (notificationList.size() > 0) {
    std::map<uint32_t, MESSAGE*>::iterator it = getLastNotification();
    displayNotification(it->second);

  } else {
    clearScreen();
  }
}

/*void removeLastNotification() {
  //ESP_LOGI(LOG_TAG, "deleting last %d", notificationList.size());
  if (notificationList.size() > 0) notificationList.erase(--notificationList.end());
  showLastNotification();
  //ESP_LOGI(LOG_TAG, "deleted last %d", notificationList.size());
}*/

MESSAGE *getNotification(uint32_t uuid) {
  //ESP_LOGI(LOG_TAG, "%d", uuid);
  return (it(uuid) != notificationList.end()) ? it(uuid)->second : new MESSAGE();
}

void addNotification(uint32_t uuid, MESSAGE *notification) {

  if (it(uuid)->second == notification) {
    //ESP_LOGI(LOG_TAG, "erase");
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

  //ESP_LOGI(LOG_TAG, "%d %s", pData[5], message.c_str());


  MESSAGE *notification = getNotification(messageId);
  //ESP_LOGI(LOG_TAG, "before: %s %s %s %d", String(notification->type), String(notification->title), String(notification->text), notification->isComplete);

  //ESP_LOGI(LOG_TAG, "wait - %d", messageId);
  if (!notification->isComplete) {
    switch (pData[5])
    {
      case 0x0:
        notification->type = message.c_str();
        //ESP_LOGI(LOG_TAG, "type: %s", message.c_str());

        break;
      case 0x1:

        notification->title = message.c_str();

        //ESP_LOGI(LOG_TAG, "title: %s", message.c_str());
        break;
      case 0x3:
        notification->text = message.c_str();

        //ESP_LOGI(LOG_TAG, "message: %s", message.c_str());

        break;
      case 0x5:
        notification->dateTime = message.c_str();
        setHora(notification->dateTime);

        //ESP_LOGI(LOG_TAG, "dateTime: %s", message.c_str());

        break;
    }
    //ESP_LOGI(LOG_TAG, "after: %s %s %s %d", String(notification->type), String(notification->title), String(notification->text), notification->isComplete);
  }
  addNotification(messageId, notification);
  if (!notification->isComplete && String(notification->title) != "" && String(notification->text) != "" && String(notification->dateTime) != "") {
    //ESP_LOGI(LOG_TAG, "complete: %d", messageId);

    notification->isComplete = true;

    showLastNotification();
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
    //ESP_LOGI(LOG_TAG, "New notification! %d", messageId);

    MESSAGE *notification = getNotification(messageId);

    latestMessageID[0] = pData[4];
    latestMessageID[1] = pData[5];
    latestMessageID[2] = pData[6];
    latestMessageID[3] = pData[7];

    notification->latestMessageID[0] = pData[4];
    notification->latestMessageID[1] = pData[5];
    notification->latestMessageID[2] = pData[6];
    notification->latestMessageID[3] = pData[7];

    addNotification(messageId, notification);
  }
  else if (pData[0] == 1)
  {
    //ESP_LOGI(LOG_TAG, "Notification Modified");
    if (pData[2] == 1) {
      incomingCall = true;
    }
    //ESP_LOGI(LOG_TAG, "Call Changed!");
  }
  else if (pData[0] == 2)
  {

    //MESSAGE *notification = getNotification(messageId);

    //ESP_LOGI(LOG_TAG, "Notification Removed! %s", notification->text);

    //if (it(messageId) != notificationList.end())
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
        //ESP_LOGI(LOG_TAG, "Failed to find our service UUID: %s", ancsServiceUUID.toString().c_str());
        return;
      }
      // Obtain a reference to the characteristic in the service of the remote BLE server.
      BLERemoteCharacteristic* pNotificationSourceCharacteristic = pAncsService->getCharacteristic(notificationSourceCharacteristicUUID);
      if (pNotificationSourceCharacteristic == nullptr) {
        //ESP_LOGI(LOG_TAG, "Failed to find our characteristic UUID: %s", notificationSourceCharacteristicUUID.toString().c_str());
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
        if (notificationList.size() > 0) btnUp.clickBle(acceptCallF, pControlPointCharacteristic);
        if (notificationList.size() > 0) btnDown.clickBle(endCall, pControlPointCharacteristic);
        if (notificationList.size() > 0) {btnTouch.touched(screenOn);}else{btnTouch.touched(getHora);}

        if (pendingNotification || incomingCall) {
          // CommandID: CommandIDGetNotificationAttributes
          // 32bit uid
          // AttributeID
          // Serial.println("Requesting details...");
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
