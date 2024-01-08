class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
      MyClient* pMyClient = new MyClient();
      pMyClient->setStackSize(10000);
      pMyClient->start(new BLEAddress(param->connect.remote_bda));
    };

    void onDisconnect(BLEServer* pServer) {
      goToSleep();
    }
};

class MainBLEServer: public Task {
    void run(void *data) {
      // Initialize device
      BLEDevice::init("Chronvs");
      BLEServer* pServer = BLEDevice::createServer();
      pServer->setCallbacks(new MyServerCallbacks());
      BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
      BLEDevice::setSecurityCallbacks(new MySecurity());

      // Advertising parameters:
      // Soliciting ANCS
      BLEAdvertising *pAdvertising = pServer->getAdvertising();
      BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
      oAdvertisementData.setFlags(0x01);
      _setServiceSolicitation(&oAdvertisementData, BLEUUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0"));
      pAdvertising->setAdvertisementData(oAdvertisementData);

      // Set security
      BLESecurity *pSecurity = new BLESecurity();
      pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
      pSecurity->setCapability(ESP_IO_CAP_OUT);
      pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

      //Start advertising
      pAdvertising->start();

      delay(portMAX_DELAY);
    }


    /**
       @brief Set the service solicitation (UUID)
       @param [in] uuid The UUID to set with the service solicitation data.  Size of UUID will be used.
    */
    void _setServiceSolicitation(BLEAdvertisementData *a, BLEUUID uuid)
    {
      char cdata[2];
      switch (uuid.bitSize()) {
        case 16: {
            // [Len] [0x14] [UUID16] data
            cdata[0] = 3;
            cdata[1] = ESP_BLE_AD_TYPE_SOL_SRV_UUID;  // 0x14
            a->addData(std::string(cdata, 2) + std::string((char *)&uuid.getNative()->uuid.uuid16, 2));
            break;
          }

        case 128: {
            // [Len] [0x15] [UUID128] data
            cdata[0] = 17;
            cdata[1] = ESP_BLE_AD_TYPE_128SOL_SRV_UUID;  // 0x15
            a->addData(std::string(cdata, 2) + std::string((char *)uuid.getNative()->uuid.uuid128, 16));
            break;
          }

        default:
          return;
      }
    } // setServiceSolicitationData
};

void SampleSecureServer(void)
{
  MainBLEServer* pMainBleServer = new MainBLEServer();
  pMainBleServer->setStackSize(20000);
  pMainBleServer->start();
}


/*

Station MAC: 34:94:54:D1:92:C4
SoftAP MAC: 34:94:54:D1:92:C5
Bluetooth MAC: 34:94:54:D1:92:C6
Ethernet MAC: 34:94:54:D1:92:C7

python ota.py "34:94:54:D1:92:C6" "firmware.bin"
 */
