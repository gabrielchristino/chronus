#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include <SPI.h>

#define TFT_CS          21
#define TFT_DC          19
#define TFT_RST         18
#define TFT_MOSI        17
#define TFT_SCLK        16

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

int lastTimeDisplay = 0;
int nowTimeDisplay = 0;

RTC_DATA_ATTR int screenTime = 30000;

GFXcanvas16 canvas(154, 74);

Task *task;

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String getHoraNotificacao(String date) {
  //String ano = date.substring(0,4);
  //String mes = date.substring(4,6);
  //String dia = date.substring(6,8);
  String hora = date.substring(9, 11);
  String minu = date.substring(11, 13);

  return hora + ":" + minu;
}

void getApp(String title) {
  String valor = getValue(title, '.', 2).substring(0, 20);
  canvas.print(valor);
  for (uint16_t i; i < 20 - valor.length(); i++) {
    canvas.print(" ");
  }
}

void resetTimer() {
  lastTimeDisplay = nowTimeDisplay;
}

void TimerScreen (void *data) {
      while (1) {
        nowTimeDisplay = millis();
        if (nowTimeDisplay - lastTimeDisplay > screenTime) {
          lastTimeDisplay = nowTimeDisplay;
          clearScreen();
          Serial.println(F("sleep"));
          goToSleep();
        }
      }
    }

void callTimerScreen() {
  xTaskCreate( TimerScreen, "TimerScreen", 1000, NULL, 1, NULL );
}

void startScreen() {
  //pinMode(17, OUTPUT);
  //digitalWrite(17, HIGH);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);

  tft.initR(INITR_MINI160x80_PLUGIN);  // Init ST7735S mini display
  tft.setSPISpeed(40000000);
  tft.setRotation(1);
  //tft.setFont(&FreeSans9pt7b);
  tft.cp437(true);
  clearScreen();
}

void screenOn() {
  //Serial.println(F("screen on"));
  digitalWrite(5, HIGH);
  tft.enableDisplay(true);
  resetTimer();
}

void clearScreen() {
  tft.enableDisplay(false);
  tft.fillScreen(ST77XX_BLACK);
}

void DisplayNotification (void *data) {
      //void displayNotification(MESSAGE* message) {
      MESSAGE* message = (MESSAGE*)data;
      //setCpuFrequencyMhz(240);
      clearScreen();
      canvas.setFont();
      //tft.fillScreen(ST77XX_BLACK);
      tft.fillRoundRect(0, 0, 160, 80, 8, 0x2945);
      canvas.fillRect(0, 0, 156, 76, 0x2945);
      canvas.setCursor(0, 2);
      canvas.setTextColor(0xa514 , 0x2945);
      canvas.setTextWrap(true);
      getApp(message->type.c_str());
      canvas.print(getHoraNotificacao(String(message->dateTime)));
      canvas.print("\n\n");
      canvas.drawLine(5, 12, 146, 12, 0x57f9);
      canvas.setTextColor(0x57f9, 0x2945);
      canvas.print(String(message->title.c_str()));
      canvas.print("\n");
      canvas.setTextColor(0xef9d, 0x2945);
      canvas.print(String(message->text.c_str()));
      tft.drawRGBBitmap(6, 3, canvas.getBuffer(), canvas.width(), canvas.height());
      screenOn();
      vTaskDelete(NULL);
    }

static void menuOptions(String menuName[], uint8_t item, uint16_t length) {
  clearScreen();
  if (menuName[item] && menuName[item] != "") {
    //tft.fillScreen(ST77XX_BLACK);
    tft.fillRoundRect(0, 0, 160, 80, 8, 0x2945);
    canvas.fillRect(0, 0, 156, 76, 0x2945);
    canvas.setTextWrap(true);

    int16_t x1, y1 = 0;
    uint16_t w, h = 0;
    canvas.getTextBounds(menuName[item], 0, 0, &x1, &y1, &w, &h); // Calculate w/h of text
    uint8_t x = (160 - w) / 2;
    uint8_t y = (80 - h) / 2;
    canvas.setCursor(x, y);

    canvas.setTextColor(0x57f9, 0x2945);
    canvas.setFont(&FreeSans9pt7b);
    if (item < length) canvas.println(menuName[item]);

    canvas.setFont();
    canvas.setTextColor(0xa514 , 0x2945);
    
    canvas.setCursor(x, y - h*4);
    if ((item - 2) > 0) canvas.println(menuName[item - 2]);
    
    canvas.setCursor(x, y - h*2.5);
    if ((item - 1) > 0) canvas.println(menuName[item - 1]);
    
    canvas.setCursor(x, y + h*1.5);
    if ((item + 1) < length) canvas.println(menuName[item + 1]);
    
    canvas.setCursor(x, y + h*3);
    if ((item + 2) < length) canvas.println(menuName[item + 2]);

    tft.drawRGBBitmap(6, 3, canvas.getBuffer(), canvas.width(), canvas.height());
    screenOn();
  Serial.println("menu "+menuName[item]);
  }
}
