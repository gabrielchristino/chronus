#include <time.h>
#include <sys/time.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

struct tm data;
timeval tv;

String hora;

void setHora (String h) {

  struct tm ltm = {0};
  ltm.tm_year = (h.substring(0, 4)).toInt() - 1900;
  ltm.tm_mon  = (h.substring(4, 6)).toInt() - 1;
  ltm.tm_mday = (h.substring(6, 8)).toInt();
  ltm.tm_hour = (h.substring(9, 11)).toInt();
  ltm.tm_min = (h.substring(11, 13)).toInt();
  ltm.tm_sec = (h.substring(14, 16)).toInt();

  int32_t epoch_seconds = difftime(mktime(&ltm), (time_t)0);

  tv.tv_sec = epoch_seconds;
  settimeofday(&tv, NULL);

  hora = h;
}

void GetHora(void * parameter) {
      //setCpuFrequencyMhz(240);
      //clearScreen();
      /*time_t tt = time(NULL);
      data = *gmtime(&tt);
      char data_formatada[64];

      strftime(data_formatada, 64, "%H:%M", &data);

      tft.setTextColor(0xef5d);
      tft.setTextWrap(true);
      tft.setFont(&FreeSans24pt7b);*/

      //drawJpeg("/fundo.jpg", 0, 0);

      /*int16_t x1, y1;
        uint16_t w, h;
        tft.getTextBounds(data_formatada, 0, 0, &x1, &y1, &w, &h); // Calculate w/h of text
        uint8_t x = (160 - w - 10);
        uint8_t y = h + 5;
        tft.setCursor(x, y);

        tft.println(data_formatada);

        tft.setFont(&FreeSans9pt7b);
        strftime(data_formatada, 64, "%d %b %a", &data);
        String data_formatada_lower = String(data_formatada);
        data_formatada_lower.toLowerCase();
        tft.getTextBounds(data_formatada_lower, 0, 0, &x1, &y1, &w, &h); // Calculate w/h of text
        x = (160 - w - 10);
        y = (80 - h + 5);
        tft.setCursor(x, y);
        tft.println(data_formatada_lower);
        //tft.drawRGBBitmap(6, 3, canvas.getBuffer(), canvas.width(), canvas.height());*/
      screenOn();
      //setCpuFrequencyMhz(40);
};

/*
  %a Abbreviated weekday name
  %A Full weekday name
  %b Abbreviated month name
  %B Full month name
  %c Date and time representation for your locale
  %d Day of month as a decimal number (01-31)
  %H Hour in 24-hour format (00-23)
  %I Hour in 12-hour format (01-12)
  %j Day of year as decimal number (001-366)
  %m Month as decimal number (01-12)
  %M Minute as decimal number (00-59)
  %p Current locale’s A.M./P.M. indicator for 12-hour clock
  %S Second as decimal number (00-59)
  %U Week of year as decimal number,  Sunday as first day of week (00-51)
  %w Weekday as decimal number (0-6; Sunday is 0)
  %W Week of year as decimal number, Monday as first day of week (00-51)
  %x Date representation for current locale
  %X Time representation for current locale
  %y Year without century, as decimal number (00-99)
  %Y Year with century, as decimal number
  %z %Z Time-zone name or abbreviation, (no characters if time zone is unknown)
  %% Percent sign
  You can include text literals (such as spaces and colons) to make a neater display or for padding between adjoining columns.
  You can suppress the display of leading zeroes  by using the "#" character  (%#d, %#H, %#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y)
*/
