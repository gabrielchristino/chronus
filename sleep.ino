//sudo chown gabriel /dev/ttyUSB0


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */

// Essa TaskHandle irá permitir 
TaskHandle_t task2Handle = NULL;
void task1(void * parameter){
   // A lógica da sua tarefa
}

void goToSleep() {
  touchSleepWakeUpEnable(T0, THRESHOLD);
  touchSleepWakeUpEnable(T5, THRESHOLD);
  touchSleepWakeUpEnable(T9, THRESHOLD);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  esp_deep_sleep_start();
}
void wakeUpReason() {
  //Serial.begin(115200);

  /*#if defined(BOD_ENABLE)
    init_bod();
    #endif*/
  //  beginOTA();

  setButtons();
  SampleSecureServer();
  mountSpiffs();
  startScreen();
  //setCpuFrequencyMhz(40);
  touch_pad_t touchPin;
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  
/*  xTaskCreate(
    timerScreen,    // Função a ser chamada
    "timerScreen",  // Nome da tarefa
    1000,            // Tamanho (bytes)
    NULL,            // Parametro a ser passado
    1,               // Prioridade da Tarefa
    NULL      // Task handle
  );*/

  //if (touchPin == 0) {
/*xTaskCreate(
    task1,
    "Task 1",
    1000,
    NULL,
    2,
    &task2Handle            // Task handle
);*/
  //}

}
