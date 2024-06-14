#include <Arduino.h>
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

void setup() {
  pinMode(ch1, INPUT); 
  pinMode(ch2, INPUT);
  pinMode(ch3, INPUT);
  pinMode(ch4, INPUT);
  pinMode(ch5, INPUT);
  pinMode(ch6, INPUT);
  pinMode(ch7, INPUT);
  pinMode(ch8, INPUT);
  pinMode(ch9, INPUT);
  pinMode(ch10, INPUT);

  Serial.begin(115200);
}

void loop() {
  unsigned long pwmRX1 = pulseIn(ch1, HIGH);
  unsigned long pwmRX2 = pulseIn(ch2, HIGH);
  unsigned long pwmRX3 = pulseIn(ch3, HIGH);
  unsigned long pwmRX4 = pulseIn(ch4, HIGH);
  unsigned long pwmRX5 = pulseIn(ch5, HIGH);
  unsigned long pwmRX6 = pulseIn(ch6, HIGH);
  unsigned long pwmRX7 = pulseIn(ch7, HIGH);
  unsigned long pwmRX8 = pulseIn(ch8, HIGH);
  unsigned long pwmRX9 = pulseIn(ch9, HIGH);
  unsigned long pwmRX10 = pulseIn(ch10, HIGH);
  
  // Вывод значения pwmValue в Serial порт
  Serial.print("ch1: ");
  Serial.print(pwmRX1);
  Serial.print(", ch2: ");
  Serial.print(pwmRX2);
  Serial.print(", ch3: ");
  Serial.print(pwmRX3);
  Serial.print(", ch4: ");
  Serial.print(pwmRX4);
  Serial.print(", ch5: ");
  Serial.print(pwmRX5);
  Serial.print(", ch6: ");
  Serial.print(pwmRX6);
  Serial.print(", ch7: ");
  Serial.print(pwmRX7);
  Serial.print(", ch8: ");
  Serial.print(pwmRX8);
  Serial.print(", ch9: ");
  Serial.print(pwmRX9);
  Serial.print(", ch10: ");
  Serial.print(pwmRX10);
  Serial.print("\n");
 
  // Здесь можно добавить другую обработку pwmValue

  
}