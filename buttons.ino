
struct BTN {
  uint8_t pin;
  bool lastState;
  int lastTime;
  void click(void (*callback)()) {
    bool state = digitalRead(pin);
    int nowTime = millis();
    if (!state && state != lastState && (nowTime - lastTime > 500)) {
      callback();
      lastTime = nowTime;
    }
    lastState = state;
  };
  void clickBle(void (*callback)(BLERemoteCharacteristic*), BLERemoteCharacteristic*BLE) {
    bool state = digitalRead(pin);
    int nowTime = millis();
    if (!state && state != lastState && (nowTime - lastTime > 500)) {
      ESP_LOGI(LOG_TAG, "botao");
      callback(BLE);
      lastTime = nowTime;
    }
    lastState = state;
  };
  void touched(void (*callback)()) {
    if (touchRead(pin) < 50) {
    Serial.println("touch");
      callback();
    }
  };
};
BTN btnUp;
BTN btnDown;
BTN btnTouch;


void setButtons() {
  pinMode(23, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);

  //btnUp.begin(23);
  //btnUp.begin(22);

  btnUp.pin = 23;
  btnDown.pin = 22;
  btnTouch.pin = T0;
}

void touch() {
  //touch0 = touchRead(T0);

  //Serial.print(digitalRead(23));
  //Serial.print(digitalRead(22));
  //Serial.println(touch0);
}
