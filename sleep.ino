//sudo chown gabriel /dev/ttyUSB0


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60*60*24        /* Time ESP32 will go to sleep (in seconds) */


void goToSleep() {
  digitalWrite(5, LOW);
  touchSleepWakeUpEnable(T0, THRESHOLD);
  touchSleepWakeUpEnable(T5, THRESHOLD);
  touchSleepWakeUpEnable(T9, THRESHOLD);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  esp_deep_sleep_start();
}
void wakeUpReason() {

  Serial.begin(115200);

  /*#if defined(BOD_ENABLE)
    init_bod();
    #endif*/
  //  beginOTA();

  setButtons();
  SampleSecureServer();
  mountSpiffs();
  startScreen();


  touch_pad_t touchPin;
  touchPin = esp_sleep_get_touchpad_wakeup_status();
  //goToSleep();

  callTimerScreen();

  if (touchPin == 0) {
    callGetHora();
  } else {
    callHome();
  }

}
