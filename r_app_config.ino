// home app
String AppConfigMenuListName[2] = {
  F("back"),
  F("screen time")
};
void (*AppConfigMenuListVoid[2])() {
  callHome,
  callScreenTime
};
uint8_t AppConfigPosicaoMenu = 0;
class AppConfig: public Task {
    void run(void *data) {
      //menuOptions(AppConfigMenuListName,AppConfigPosicaoMenu, 2);
      Serial.println("Config");
      for (;;) {
        delay(10);

        btnUp.click([] {novaPosicaoMenu(1);});
        btnDown.click([] {novaPosicaoMenu(-1);});
        btnOk.click([] {novaPosicaoMenu(0);});
      }
    }

    static void novaPosicaoMenu(uint8_t novaPos) {
      AppConfigPosicaoMenu += novaPos;
      menuOptions(AppConfigMenuListName,AppConfigPosicaoMenu, 2);
      if (novaPos == 0) AppConfigMenuListVoid[AppConfigPosicaoMenu]();
      resetTimer();
    }
};

void callConfig() {
  pConfig->start();
  killAll(1);
}




// screentime
String AppScreenTimeMenuListName[1] = {
  "screen time " + String(screenTime)
};
class AppScreenTime: public Task {
    void run(void *data) {
      clearScreen();
      for (;;) {
        delay(10);

        btnUp.click([] {newScreenTime(1000);});
        btnDown.click([] {newScreenTime(-1000);});
        btnOk.click([] {newScreenTime(0);});

      }
    }

    static void newScreenTime(uint8_t newTime) {
      screenTime += newTime;
      menuOptions(AppScreenTimeMenuListName, 0, 1);
      if (newTime == 0) callHome();
      resetTimer();
    }
};

void callScreenTime() {
  pScreenTime->start();
  killAll(2);
}
