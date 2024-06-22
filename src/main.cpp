#include <Arduino.h>

// RX CHANNELS          1,  2, 3, 4,  5,  6, 7,  8,  9,  10
// INDEX                0,  1, 2, 3,  4,  5, 6,  7,  8,  9
const int radioPin[] = {15, 2, 4, 16, 17, 5, 18, 19}; // 2 — Газ; 3 — Поворот; 7 — тормоз; 6 — реверс.
const int motorPin[] = {32, 33, 25, 26};
const int directionPin[] = {27, 14, 12, 13};
const int brakePin[4] = {21, 22};

bool directionOut[] = {0, 0, 1, 1}; // налаштування напрямку двигунів за замовчуванням 0 — вперед, 1 — назад
int motorOut[] = {0, 0, 0, 0};

// Флаги стану
bool isReversing = false;
bool isTurnRight = false;
bool isTurnLeft = false;

volatile unsigned long pwmRX[8]; // Змінні для збереження значень PWM

// Обробник переривань для коректного прийому PWM сигналу
void IRAM_ATTR handleInterrupt(void *arg)
{
    int channelIndex = (int)arg;
    static uint32_t prevRise[8] = {0};
    static uint32_t prevFall[8] = {0};
    uint32_t currTime = micros();
    if (digitalRead(radioPin[channelIndex]) == HIGH)
    {
        prevRise[channelIndex] = currTime;
    }
    else
    {
        prevFall[channelIndex] = currTime;
        pwmRX[channelIndex] = prevFall[channelIndex] - prevRise[channelIndex];
    }
}

void setup()
{
    Serial.begin(115200);

    // Обробник переривань для коректного прийому PWM сигналу
    for (int i = 0; i < 8; i++)
    {
        attachInterruptArg(digitalPinToInterrupt(radioPin[i]), handleInterrupt, (void *)i, CHANGE);
    }
    interrupts();

    // Оголошення портів
    for (int i = 0; i < 4; i++)
    {
        pinMode(directionPin[i], OUTPUT);
    };
    pinMode(brakePin[0], OUTPUT);
    pinMode(brakePin[1], OUTPUT);
}
void smoothReverse(int stepDelay = 8, int stepSize = 4)
{
    // const int stepDelay = 16; // Затримка між кроками зміни швидкості
    // const int stepSize = 4;   // Крок зміни швидкості

    // Плавне зменшення швидкості до нуля
    while (motorOut[0] > 0 || motorOut[1] > 0 || motorOut[2] > 0 || motorOut[3] > 0)
    {
        for (int i = 0; i < 4; i++)
        {
            motorOut[i] = max(motorOut[i] - stepSize, 0);
            analogWrite(motorPin[i], motorOut[i]);
        }
        delay(stepDelay);
    }

    // Зміна напрямку
    for (int i = 0; i < 4; i++)
    {
        directionOut[i] = !directionOut[i];
        digitalWrite(directionPin[i], directionOut[i]);
    }

    // Плавне збільшення швидкості до поточного значення PWM
    int targetSpeed = map(pwmRX[2], 986, 1972, 0, 255);
    targetSpeed = constrain(targetSpeed, 0, 255);
    while (motorOut[0] < targetSpeed || motorOut[1] < targetSpeed || motorOut[2] < targetSpeed || motorOut[3] < targetSpeed)
    {
        for (int i = 0; i < 4; i++)
        {
            motorOut[i] = min(motorOut[i] + stepSize, targetSpeed);
            analogWrite(motorPin[i], motorOut[i]);
        }
        delay(stepDelay);
    }

    isReversing = !isReversing; // Скидання прапорця після завершення плавного реверс
}

void smoothTurn(int motorIndex1, int motorIndex2, bool isTurnRightOrLeft)
{
    int stepDelay = 16; // Затримка між кроками зміни швидкості
    int stepSize = 4;   // Крок зміни швидкості

    // Плавне зменшення швидкості до нуля
    while (motorOut[motorIndex1] > 0 || motorOut[motorIndex2] > 0)
    {

        motorOut[motorIndex1] = max(motorOut[motorIndex1] - stepSize, 0);
        analogWrite(motorPin[motorIndex1], motorOut[motorIndex1]);
        motorOut[motorIndex2] = max(motorOut[motorIndex2] - stepSize, 0);
        analogWrite(motorPin[motorIndex2], motorOut[motorIndex2]);

        delay(stepDelay);
    }

    // Зміна напрямку
    directionOut[motorIndex1] = !directionOut[motorIndex1];
    digitalWrite(directionPin[motorIndex1], directionOut[motorIndex1]);
    directionOut[motorIndex2] = !directionOut[motorIndex2];
    digitalWrite(directionPin[motorIndex2], directionOut[motorIndex2]);

    // Плавне збільшення швидкості до поточного значення PWM
    int targetSpeed = map(pwmRX[2], 986, 1972, 0, 255);
    targetSpeed = constrain(targetSpeed, 0, 255);
    while (motorOut[motorIndex1] < targetSpeed || motorOut[motorIndex2] < targetSpeed)
    {

        motorOut[motorIndex1] = min(motorOut[motorIndex1] + stepSize, targetSpeed);
        analogWrite(motorPin[motorIndex1], motorOut[motorIndex1]);
        motorOut[motorIndex2] = min(motorOut[motorIndex2] + stepSize, targetSpeed);
        analogWrite(motorPin[motorIndex2], motorOut[motorIndex2]);

        delay(stepDelay);
    }

    isTurnRightOrLeft = !isTurnRightOrLeft; // Скидання прапорця після завершення плавного реверс
}

void loop()
{
    int yawValue = map(pwmRX[3], 986, 1972, -255, 255);
    int throttleValue = map(pwmRX[2], 986, 1990, 0, 255);

    bool onReversing = (pwmRX[7] >= 1450 && pwmRX[7] <= 1550) ? true : false;
    bool onBrake = (pwmRX[6] >= 1450 && pwmRX[6] <= 1500) ? true : false;

    for (int i = 0; i < 4; i++)
    {
        // Газ без поворотів
        motorOut[i] = throttleValue;
        motorOut[i] = constrain(motorOut[i], 0, 255);
        analogWrite(motorPin[i], motorOut[i]);
        // Міксування сигналу для поворотів
        if ((yawValue > 20 && yawValue > 10) != isTurnRight)
        {
            // directionOut[0] = !directionOut[0];
            // directionOut[1] = !directionOut[1];

            int stepDelay = 8; // Затримка між кроками зміни швидкості
            int stepSize = 4;  // Крок зміни швидкості

            // Плавне зменшення швидкості до нуля

            while (motorOut[0] > 0 || motorOut[1] > 0)
            {
                for (int i = 0; i < 2; i++)
                {
                    motorOut[i] = max(motorOut[i] - stepSize, 0);
                    analogWrite(motorPin[i], motorOut[i]);
                }
                delay(stepDelay);
            }

            // Зміна напрямку
            for (int i = 0; i < 2; i++)
            {
                directionOut[i] = !directionOut[i];
                digitalWrite(directionPin[i], directionOut[i]);
            }

            // Плавне збільшення швидкості до поточного значення PWM
            int targetSpeed = map(pwmRX[2], 986, 1972, 0, 255);
            targetSpeed = constrain(targetSpeed, 0, 255);
            while (motorOut[0] < targetSpeed || motorOut[1] < targetSpeed)
            {
                for (int i = 0; i < 2; i++)
                {
                    motorOut[i] = min(motorOut[i] + stepSize, targetSpeed);
                    analogWrite(motorPin[i], motorOut[i]);
                }
                delay(stepDelay);
            }

            isTurnRight = !isTurnRight; // Скидання прапорця після завершення плавного реверс

            Serial.print("yawValue > 20");
            Serial.print(yawValue);
            Serial.println();
        }
        if ((yawValue < -20 && yawValue < -10) != isTurnLeft)
        {
            int stepDelay = 8; // Затримка між кроками зміни швидкості
            int stepSize = 4;  // Крок зміни швидкості

            // Плавне зменшення швидкості до нуля

            while (motorOut[2] > 0 || motorOut[3] > 0)
            {
                for (int i = 2; i < 4; i++)
                {
                    motorOut[i] = max(motorOut[i] - stepSize, 0);
                    analogWrite(motorPin[i], motorOut[i]);
                }
                Serial.print("yawValue < 20 — ");
                Serial.print(yawValue);
                Serial.println();
                delay(stepDelay);
            }

            // Зміна напрямку
            for (int i = 2; i < 4; i++)
            {
                directionOut[i] = !directionOut[i];
                digitalWrite(directionPin[i], directionOut[i]);
            }

            // Плавне збільшення швидкості до поточного значення PWM
            int targetSpeed = map(pwmRX[2], 986, 1972, 0, 255);
            targetSpeed = constrain(targetSpeed, 0, 255);
            while (motorOut[2] < targetSpeed || motorOut[3] < targetSpeed)
            {
                for (int i = 2; i < 4; i++)
                {
                    motorOut[i] = min(motorOut[i] + stepSize, targetSpeed);
                    analogWrite(motorPin[i], motorOut[i]);
                }
                delay(stepDelay);
            }

            isTurnLeft = !isTurnLeft; // Скидання прапорця після завершення плавного реверс
        }
        // Логіка реверсу
        if (onReversing != isReversing)
        {
            smoothReverse();
            // digitalWrite(directionPin[i], !directionOut[i]);
        }
        else
        {
            digitalWrite(directionPin[i], directionOut[i]);
        }

        // Тормоз, тернарный оператор
        digitalWrite(brakePin[i], onBrake ? HIGH : LOW);
    }

    // Вивід у серійний порт
    for (int i = 0; i < 8; i++)
    {
        Serial.printf("pwmRX%d: %lu, ", i + 1, pwmRX[i]);
    }
    Serial.print(yawValue);
    Serial.println();
}