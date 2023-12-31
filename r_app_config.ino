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
void AppConfig(void *data) {
  Serial.println("Config");
  menuOptions(AppConfigMenuListName,0, 2);
  for (;;) {
    delay(10);

    btnUp.click([] {AppConfigNovaPosicaoMenu(1);});
    btnDown.click([] {AppConfigNovaPosicaoMenu(-1);});
    btnOk.click([] {AppConfigNovaPosicaoMenu(0);});
  }
}

static void AppConfigNovaPosicaoMenu(uint8_t novaPos) {
  AppConfigPosicaoMenu += novaPos;
  menuOptions(AppConfigMenuListName, AppConfigPosicaoMenu, 2);
  if (novaPos == 0) {AppConfigMenuListVoid[AppConfigPosicaoMenu]();vTaskDelete(NULL);}
  resetTimer();
}

void callConfig() {
  xTaskCreate( AppConfig, "AppConfig", 4000, NULL, 1, &pConfig );
}




// screentime
String AppScreenTimeMenuListName[1] = {
  "screen time " + String(screenTime)
};
void AppScreenTime(void *data) {
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
  if (newTime == 0) {callHome();vTaskDelete(NULL);}
  resetTimer();
}

void callScreenTime() {
  xTaskCreate( AppScreenTime, "AppScreenTime", 4000, NULL, 1, &pAppScreenTime );
}
