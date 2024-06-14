#include <Arduino.h>

// Объявляем номера пинов для каналов
const int channels[] = {15, 2, 4, 16, 17, 5, 18, 19, 21, 22};

// Переменные для хранения значений PWM
volatile unsigned long pwmRX[10];

// Обработчики прерываний для каналов
void IRAM_ATTR handleInterrupt(void* arg) {
  int channelIndex = (int)arg; // Индекс канала
  static uint32_t prevRise[10] = {0}; // Переменные для хранения времени предыдущего нарастающего фронта
  static uint32_t prevFall[10] = {0}; // Переменные для хранения времени предыдущего спадающего фронта
  uint32_t currTime = micros(); // Получаем текущее время в микросекундах

  // Проверяем состояние пина
  if (digitalRead(channels[channelIndex]) == HIGH) {
    prevRise[channelIndex] = currTime; // Если высокий уровень, сохраняем время нарастающего фронта
  } else {
    prevFall[channelIndex] = currTime; // Если низкий уровень, сохраняем время спадающего фронта
    pwmRX[channelIndex] = prevFall[channelIndex] - prevRise[channelIndex]; // Записываем длительность импульса в соответствующий элемент массива
  }
}

void setup() {
  Serial.begin(115200); // Инициализируем Serial порт

  // Прикрепляем обработчики прерываний к соответствующим пинам
  for (int i = 0; i < 10; i++) {
    attachInterruptArg(digitalPinToInterrupt(channels[i]), handleInterrupt, (void*)i, CHANGE);
  }

  // Разрешаем прерывания
  interrupts();
}

void loop() {
  // Используем значения pwmRX
  for (int i = 0; i < 10; i++) {
    Serial.printf("pwmRX%d: %lu, ", i + 1, pwmRX[i]);
  }
  Serial.println();
}