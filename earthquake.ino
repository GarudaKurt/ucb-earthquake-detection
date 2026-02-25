#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>

#include "gyro.h"
GYRO gyro;

#define LED_PIN   4
#define LED_COUNT 7
#define BLINK_INTERVAL 500  // ms

unsigned long prevBlinkTime = 0;
bool ledState = false;

Adafruit_NeoPixel LEDstrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ================== PINS ================== */
const int vibrationSensor = 8;
const int buzzerPin = 7;

/* ================== OLED ================== */
#define I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ================== BUZZER TIMER ================== */
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long BUZZER_DURATION = 30000; // 30 seconds

int pulseFrame = 0;
int shakeOffset = 0;

void setup() {
  Serial.begin(9600);   // UART to slave

  pinMode(vibrationSensor, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  LEDstrip.begin();
  LEDstrip.clear();
  LEDstrip.show();
  gyro.initMPU();

  Wire.begin();
  display.begin(I2C_ADDRESS, true);
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
}

void loop() {
  int sensorValue = digitalRead(vibrationSensor);
  Serial.println(sensorValue);

  if (sensorValue == HIGH && gyro.Status() <= 2.0 && !buzzerActive) {
    buzzerActive = true;
    buzzerStartTime = millis();
    digitalWrite(buzzerPin, HIGH);

  }

  if (buzzerActive && millis() - buzzerStartTime >= BUZZER_DURATION) {
    buzzerActive = false;
    digitalWrite(buzzerPin, LOW);
    
    // Send signal to slave
    Serial.write('S');  
    Serial.println("Sent 'S' to slave"); 
    LEDstrip.clear();
    LEDstrip.show();
  }

  handleLEDStrip();

  if (buzzerActive) {
    earthquakeAnimation();
  } else {
    standbyScreen();
  }

  delay(50); 
}

void handleLEDStrip() {
  if (!buzzerActive) return;

  if (millis() - prevBlinkTime >= BLINK_INTERVAL) {
    prevBlinkTime = millis();
    ledState = !ledState;

    if (ledState) {
      for (int i = 0; i < LED_COUNT; i++) {
        LEDstrip.setPixelColor(i, LEDstrip.Color(255, 0, 0)); 
      }
    } else {
      LEDstrip.clear();
    }
    LEDstrip.show();
  }
}

void standbyScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(28, 25);
  display.println("System Stable");
  display.display();
}

void earthquakeAnimation() {
  display.clearDisplay();

  shakeOffset = (pulseFrame % 4) - 2;

  if ((pulseFrame / 5) % 2 == 0) {
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);
  }

  int radius = map(pulseFrame % 20, 0, 19, 5, 18);
  display.fillCircle(
    SCREEN_WIDTH / 2 + shakeOffset,
    SCREEN_HEIGHT / 2,
    radius,
    SH110X_WHITE
  );

  display.setTextSize(1);
  if ((pulseFrame / 10) % 2 == 0) {
    display.setCursor(18 + shakeOffset, 5);
    display.println("!!! EARTHQUAKE !!!");
  }

  display.display();
  pulseFrame++;
}
