#include <time.h>
#include <sys/time.h>
#include <Fonts/FreeSansBold24pt7b.h>

struct tm data;
timeval tv;

String hora;

void setHora (String h) {

  struct tm ltm = {0};
    ltm.tm_year = (h.substring(0,4)).toInt() - 1900;
    ltm.tm_mon  = (h.substring(4,6)).toInt() - 1;
    ltm.tm_mday = (h.substring(6,8)).toInt();
    ltm.tm_hour = (h.substring(9, 11)).toInt();
    ltm.tm_min = (h.substring(11, 13)).toInt();
    ltm.tm_sec = (h.substring(14, 16)).toInt();

    int32_t epoch_seconds = difftime(mktime(&ltm), (time_t)0);
    
    tv.tv_sec = epoch_seconds;
    settimeofday(&tv, NULL);
  
  hora = h;
}

void getHora () {
  time_t tt = time(NULL);
  data = *gmtime(&tt);
  char data_formatada[64];
  //strftime(data_formatada, 64, "%d/%m/%Y %H:%M:%S", &data);
  strftime(data_formatada, 64, "%H:%M", &data);

  Serial.println(data_formatada);

  screenOn();
  canvas.setTextColor(0xa514 , 0x2945);
  canvas.setTextWrap(true);
  canvas.setFont(&FreeSansBold24pt7b);
  canvas.fillScreen(ST77XX_BLACK);
  //tft.fillRoundRect(0, 0, 160, 80, 8, 0x2945);
  //tft.fillRect(0, 0, 156, 76, 0x2945);

  int16_t x1, y1;
  uint16_t w, h;
  canvas.getTextBounds(data_formatada, 0, 0, &x1, &y1, &w, &h); // Calculate w/h of text
  uint8_t x = (140 - w) / 2;
  uint8_t y = (74 - h) / 2+30;
  canvas.setCursor(x, y);

  
  canvas.println(data_formatada);
  tft.drawRGBBitmap(6, 3, canvas.getBuffer(), canvas.width(), canvas.height());
  
  //return data_formatada;
}
