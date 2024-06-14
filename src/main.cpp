#include <Arduino.h>

const int ch1 = 15;
// ... остальные пины

volatile unsigned long risingEdge[10];
volatile unsigned long fallingEdge[10];

void IRAM_ATTR handleInterrupt0(void* arg) {
  static uint32_t prevRise = 0;
  static uint32_t prevFall = 0;
  uint32_t currTime = micros();
  
  if (digitalRead(ch1) == HIGH) {
    prevRise = currTime;
  } else {
    prevFall = currTime;
    risingEdge[0] = prevRise;
    fallingEdge[0] = prevFall;
  }
}

// Похожие функции handleInterrupt1, handleInterrupt2, ... для остальных каналов

void setup() {
  Serial.begin(115200);
  
  // Attach interrupts
  attachInterruptArg(ch1, handleInterrupt0, NULL, CHANGE);
  // ... остальные пины

  // Enable interrupts
  interrupts();
}

void loop() {
  for (int i = 0; i < 10; i++) {
    if (fallingEdge[i] > risingEdge[i]) {
      unsigned long pulseDuration = fallingEdge[i] - risingEdge[i];
      Serial.printf("ch%d: %lu\n", i + 1, pulseDuration);
      risingEdge[i] = 0;
      fallingEdge[i] = 0;
    }
  }
}