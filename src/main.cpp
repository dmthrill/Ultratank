#include <Arduino.h>

/*
CHANNELS  1,  2, 3, 4,  5,  6, 7,  8,  9,  10
PINS      15, 2, 4, 16, 17, 5, 18, 19, 21, 22
*/

const int radioPin[10] = {15, 2, 4, 16, 17, 5, 18, 19, 21, 22}; // 1. Газ 2. Поворот. 3. Тормоз. Реверс.
const int motorPin[4] = {32, 33, 25, 26};
const int directionPin[4] = {27, 14, 12, 13};

bool directionOut[4] = {1, 0, 0, 0}; // налаштування направлення двигунів за замовчуванням 0 — вперед, 1 — назад
bool brakeOut[4] = {0, 0, 0, 0};
int motorOut[4] = {0, 0, 0, 0};



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

int yawValue = map(pwmRX[3], 986, 1972, -255, 255);
int throttleValue = map(pwmRX[2], 986, 1972, 0, 255);



if (yawValue > 0)
{
  // Плавно скидаємо швидкість моторів, доки motorOut не буде 0
  for (int i = 0; motorOut[0] <= 0; i++)
  {
    motorOut[0] = throttleValue - 10;
    motorOut[1] = throttleValue - 10;
    delay(200);
  }
	directionOut[0] = !directionOut[0];
	directionOut[1] = !directionOut[1];

  //Плавно набираемо оберти
  for (int i = 0; motorOut[0] = abs(yawValue); i++)
  {
    motorOut[0] = i;
    delay(200);
  }
}

if (yawValue > 0)
{
	directionOut[0] = !directionOut[0];
	directionOut[1] = !directionOut[1];
	motorOut[0] = abs(yawValue);
	motorOut[1] = abs(yawValue);
}

/*
Якщо yawValue > 0 робимо
	directionOut[0] = !directionOut[0]
	directionOut[1] = !directionOut[1]
	motorOut[0] = abs(yawValue)
	motorOut[1] = abs(yawValue)
Якщо yawValue < 0 робимо
	directionOut[2] = !directionOut[2]
	directionOut[3] = !directionOut[3]
	motorOut[2] = abs(yawValue)
	motorOut[3] = abs(yawValue)
*/






for (int i = 0; i < 4; i++) {
  motorOut[i] = map(pwmRX[2], 986, 1972, 0, 255);
  motorOut[i] = constrain(motorOut[i], 0, 255);
  analogWrite(motorPin[i], motorOut[i]);
  digitalWrite(directionPin[i], directionOut[i]);
}









  // Вывод значений pwmRX
  for (int i = 0; i < 10; i++) {
    Serial.printf("pwmRX%d: %lu, ", i + 1, pwmRX[i]);
  }
  Serial.println();


/*
Троттл ін
Яв ін

Правий передній троттл
Правий задній троттл
Лівий передній троттл
Лівий задній троттл
*/

  
}