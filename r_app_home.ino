// home app
String AppHomeMenuListName[3] = {
  "",
  F("Settings"),
  F("Other")
};
void (*AppHomeMenuListVoid[3])() {
  callGetHora,
  callConfig,
  callConfig,
};
uint8_t AppHomePosicaoMenu = 0;
void AppHome (void *data) {
  clearScreen();
  Serial.println("ola");
  for (;;) {
    delay(10);

    btnUp.click([] {AppHomeNovaPosicaoMenu(1);});
    btnDown.click([] {AppHomeNovaPosicaoMenu(-1);});
    btnOk.click([] {AppHomeNovaPosicaoMenu(0);});

  }
}

static void AppHomeNovaPosicaoMenu(uint8_t novaPos) {
  AppHomePosicaoMenu += novaPos;
  menuOptions(AppHomeMenuListName, AppHomePosicaoMenu, 3);
  if (novaPos == 0) {
    AppHomeMenuListVoid[AppHomePosicaoMenu]();
    vTaskDelete(NULL);
  }

  resetTimer();
}


void callHome() {
  xTaskCreate( AppHome, "AppHome", 3000, NULL, 1, &pHome );
}
