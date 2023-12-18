#define THRESHOLD      40
struct BTN {
  uint8_t pin;
  bool lastState;
  int lastTime;
  void click(void (*callback)()) {
    bool state = (touchRead(pin) < THRESHOLD);
    int nowTime = millis();
    if (!state && state != lastState && (nowTime - lastTime > 500)) {
      callback();
      lastTime = nowTime;
    }
    lastState = state;
  };
  void clickBle(void (*callback)(BLERemoteCharacteristic*), BLERemoteCharacteristic*BLE) {
    bool state = (touchRead(pin) < THRESHOLD);
    int nowTime = millis();
    if (!state && state != lastState && (nowTime - lastTime > 500)) {
      //ESP_LOGI(LOG_TAG, "botao");
      callback(BLE);
      lastTime = nowTime;
    }
    lastState = state;
  };
};

BTN btnUp;
BTN btnDown;
BTN btnTouch;


void setButtons() {

  //btnUp.begin(23);
  //btnUp.begin(22);

  btnUp.pin = T9;
  btnDown.pin = T5;
  btnTouch.pin = T0;
}

class runButtons: public Task {
    void run(void* data) {
    }
};
