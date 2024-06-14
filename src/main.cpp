#include <Arduino.h>

// Объявляем номера пинов для каналов
const int ch1 = 15;
const int ch2 = 2;
const int ch3 = 4;
const int ch4 = 16;
const int ch5 = 17;
const int ch6 = 5;
const int ch7 = 18;
const int ch8 = 19;
const int ch9 = 21;
const int ch10 = 22;

// Переменные для хранения значений PWM
volatile unsigned long pwmRX1;
volatile unsigned long pwmRX2;
volatile unsigned long pwmRX3;
volatile unsigned long pwmRX4;
volatile unsigned long pwmRX5;
volatile unsigned long pwmRX6;
volatile unsigned long pwmRX7;
volatile unsigned long pwmRX8;
volatile unsigned long pwmRX9;
volatile unsigned long pwmRX10;

// Обработчик прерывания для канала ch1
void IRAM_ATTR handleInterrupt0(void* arg) {
  static uint32_t prevRise0 = 0; // Переменная для хранения времени предыдущего нарастающего фронта
  static uint32_t prevFall0 = 0; // Переменная для хранения времени предыдущего спадающего фронта
  uint32_t currTime = micros(); // Получаем текущее время в микросекундах

  // Проверяем состояние пина ch1
  if (digitalRead(ch1) == HIGH) {
    prevRise0 = currTime; // Если высокий уровень, сохраняем время нарастающего фронта
  } else {
    prevFall0 = currTime; // Если низкий уровень, сохраняем время спадающего фронта
    pwmRX1 = prevFall0 - prevRise0; // Записываем длительность импульса в pwmRX1
  }
}

// Обработчик прерывания для канала ch2 (аналогичен handleInterrupt0)
void IRAM_ATTR handleInterrupt1(void* arg) {
  static uint32_t prevRise1 = 0;
  static uint32_t prevFall1 = 0;
  uint32_t currTime = micros();

  if (digitalRead(ch2) == HIGH) {
    prevRise1 = currTime;
  } else {
    prevFall1 = currTime;
    pwmRX2 = prevFall1 - prevRise1; // Записываем длительность импульса в pwmRX2
  }
}

void setup() {
  Serial.begin(115200); // Инициализируем Serial порт

  // Прикрепляем обработчики прерываний к соответствующим пинам
  attachInterruptArg(ch1, handleInterrupt0, NULL, CHANGE);
  attachInterruptArg(ch2, handleInterrupt1, NULL, CHANGE);

  // Разрешаем прерывания
  interrupts();
}

void loop() {
  // Используем значения pwmRX1 и pwmRX2
  Serial.printf("pwmRX1: %lu, pwmRX2: %lu\n", pwmRX1, pwmRX2);

  // Другой код...
}