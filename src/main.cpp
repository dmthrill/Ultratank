#include <Arduino.h>

/*
CHANNELS  1,  2, 3, 4,  5,  6, 7,  8,  9,  10
PINS      15, 2, 4, 16, 17, 5, 18, 19, 21, 22
*/

const int radioPin[] = {15, 2, 4, 16, 17, 5, 18, 19, 21, 22}; // 1. Газ 2. Поворот. 3. Тормоз. Реверс.
const int motorPin[] = {32, 33, 25, 26};
const int directionPin[] = {27, 14, 12, 13};

bool directionOut[] = {1, 0}; // налаштування направлення двигунів за замовчуванням 0 — вперед, 1 — назад
bool brakeOut[] = {0, 0, 0, 0};
int motorOut[] = {0, 0, 0, 0};



// Переменные для хранения значений PWM
volatile unsigned long pwmRX[10];

// Обработчики прерываний
void IRAM_ATTR handleInterrupt(void* arg) {
  int channelIndex = (int)arg; // Индекс канала
  static uint32_t prevRise[10] = {0}; // Переменные для хранения времени предыдущего нарастающего фронта
  static uint32_t prevFall[10] = {0}; // Переменные для хранения времени предыдущего спадающего фронта
  uint32_t currTime = micros(); // Получаем текущее время в микросекундах
  // Проверяем состояние пина
  if (digitalRead(radioPin[channelIndex]) == HIGH) {
    prevRise[channelIndex] = currTime; // Если высокий уровень, сохраняем время нарастающего фронта
  } else {
    prevFall[channelIndex] = currTime; // Если низкий уровень, сохраняем время спадающего фронта
    pwmRX[channelIndex] = prevFall[channelIndex] - prevRise[channelIndex]; // Записываем длительность импульса в соответствующий элемент массива
  }
}

void setup() {
  Serial.begin(115200); // Инициализируем Serial порт

  // Прерывание
  for (int i = 0; i < 10; i++) {
    attachInterruptArg(digitalPinToInterrupt(radioPin[i]), handleInterrupt, (void*)i, CHANGE);
  }
  interrupts();


  for (int i = 0; i < 2; i++)
  {
    pinMode(directionPin[i], OUTPUT);
    pinMode(motorOut[i], OUTPUT);
    pinMode(brakeOut[i], OUTPUT);

  };
  
  

}

void loop() {




for (int i = 0; i < 4; i++) {
  motorOut[i] = map(pwmRX[2], 986, 1972, 0, 255);
  motorOut[i] = constrain(motorOut[i], 0, 255);
  analogWrite(motorPin[i], motorOut[i]);

  digitalWrite(directionPin[i], directionOut[i]);
}









  // Используем значения pwmRX
  for (int i = 0; i < 10; i++) {
    Serial.printf("pwmRX%d: %lu, ", i + 1, pwmRX[i]);
  }
  Serial.println();

  
}