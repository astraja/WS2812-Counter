#include <FastLED.h>
#include <MillisTimer.h>
#include "screenMap.h"

#define LED_PIN 5
#define POTENTIOMETER_PIN A0
#define NUM_LEDS 64
#define BRIGHTNESS 12
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
uint8_t timeSet = 8;
uint8_t brightness = 12;  // Domyślna jasność
int counter = timeSet; 
int inMenu = 1;

MillisTimer tim1(1000);  // Timer odmierzający 1 sekundę
MillisTimer tim2(1000);

volatile unsigned long lastInterruptTimeUp = 0;
volatile unsigned long lastInterruptTimeDown = 0;
const unsigned long debounceDelay = 100;  // 100 ms debounce

void resetCounter() {
  counter = timeSet;  // Reset licznika do timeSet
  tim1.reset();       // Resetowanie timera
  Serial.println("Licznik zresetowany!");
}

void OnBtnClick() {
  inMenu = -1;
  Serial.println(timeSet);
  tim1.updateDelay(1000);
  resetCounter();  // Resetujemy licznik przy każdej zmianie wartości
}

void myISRup() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimeUp > debounceDelay) {
    lastInterruptTimeUp = currentTime;
    if (timeSet < 20) {
      timeSet++;
      OnBtnClick();
    }
  }
}

void myISRdown() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimeDown > debounceDelay) {
    lastInterruptTimeDown = currentTime;
    if (timeSet > 3) {
      timeSet--;
      OnBtnClick();
    }
  }
}

void clearDisplay() {
  FastLED.clear(); 
  FastLED.show();  
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(POTENTIOMETER_PIN, INPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  attachInterrupt(digitalPinToInterrupt(2), myISRup, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), myISRdown, FALLING);
  tim1.updateDelay(1000);
  tim1.reset();
  tim2.reset();
}

void displayNumber(int num) {
  for (int i = 0; i < NUM_LEDS; i++) {
    long val = nums[num][i];
    if(num == 0){
      val *= 0xFF0000;
    }
    else if(num == 2){
      val *= 0x00ff2a;
    }
    else if(num == 1){
      val *= 0xffa200;
    }
    else{
      val *= 0xff;
    }
    leds[i] = val;
  }
  FastLED.show();
}

void readBrightness(){
  int potValue = analogRead(POTENTIOMETER_PIN);
  brightness = map(potValue, 0, 1023, 5, 255);
  FastLED.setBrightness(brightness);
}

void loop() {
  if (inMenu == -1) {
    clearDisplay();
    fill_solid(leds, timeSet, CRGB::Red);
    FastLED.show();
    if (tim2.canExecute()) {
      inMenu = 1;
      resetCounter();  // Reset po powrocie do głównej pętli
    }
  } else {
    if (tim1.canExecute()) {
      displayNumber(counter);
      if (counter > 0) {
        counter--;
      } else {
        counter = timeSet;
      }
      readBrightness();
    }
  }
}






