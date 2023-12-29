#define THRESHOLD      30
struct BTN {
  uint8_t pin;
  bool lastState;
  int lastTime;
  void click(void (*callback)()) {
    uint8_t valor = touchRead(pin);

    bool state = (valor < THRESHOLD && valor > 0);
    int nowTime = millis();
    if (!state && state != lastState && (nowTime - lastTime > 500)) {
      callback();
      lastTime = nowTime;
    }
    lastState = state;
  };
};

BTN btnUp;
BTN btnDown;
BTN btnOk;


void setButtons() {

  //btnUp.begin(23);
  //btnUp.begin(22);

  btnUp.pin = T9;
  btnDown.pin = T5;
  btnOk.pin = T0;
}
