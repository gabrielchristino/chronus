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
class AppHome: public Task {
    void run(void *data) {
      clearScreen();
      Serial.println("ola");
      for (;;) {
        delay(10);

        btnUp.click([] {novaPosicaoMenu(1);});
        btnDown.click([] {novaPosicaoMenu(-1);});
        btnOk.click([] {novaPosicaoMenu(0);});

      }
    }

    static void novaPosicaoMenu(uint8_t novaPos) {
      AppHomePosicaoMenu += novaPos;
      menuOptions(AppHomeMenuListName, AppHomePosicaoMenu, 3);
      if (novaPos == 0) AppHomeMenuListVoid[AppHomePosicaoMenu]();
      
      resetTimer();
    }
};

void callHome() {
  pHome->start();
  killAll(0);
}
