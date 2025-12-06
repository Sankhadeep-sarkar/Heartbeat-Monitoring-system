#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
#define SCREEN_RESET -1

#define REPORTING_PERIOD_MS 999

PulseOximeter sensor;

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);

#define START 17
#define PREV 18
#define RESET 19

uint32_t tsLastReport = 0;

int count = 0;

unsigned long startTime;
unsigned long endTime;
unsigned long duration;

void onBeatDetected() {}

void displayMenu() {
  while(true) {
  screen.clearDisplay();
   screen.drawRoundRect(0, 0, screen.width(), screen.height()/2,
      screen.height()/4, SSD1306_WHITE);
      screen.setTextSize(1);
      screen.setTextColor(SSD1306_WHITE);
      screen.setCursor(screen.height()/4, screen.height()/8);
      screen.println("START");
    screen.drawRoundRect(0, screen.height()/2, screen.width(), screen.height()/2,
      screen.height()/4, SSD1306_WHITE);
      screen.setTextSize(1);
      screen.setTextColor(SSD1306_WHITE);
      screen.setCursor(screen.height()/4, screen.height()/8+screen.height()/2);
      screen.println("PREVIOUS");
    screen.display();

    if(digitalRead(START) == LOW) {
      screen.fillRoundRect(0, 0, screen.width(), screen.height()/2,
      screen.height()/4, SSD1306_INVERSE);
      screen.display();
      delay(200);
      return;
    } else if (digitalRead(PREV) == LOW) {
      screen.fillRoundRect(0, screen.height()/2, screen.width(), screen.height()/2,
        screen.height()/4, SSD1306_INVERSE);
        screen.display();
        delay(200);
        displayResult();
    }
    
  }
}

void setup() {
  Serial.begin(115200);

  Serial.print("Initializing Pulse Oximeter..");

  pinMode(START, INPUT_PULLUP);
  pinMode(PREV, INPUT_PULLUP);
  pinMode(RESET, INPUT_PULLUP);

  if(!sensor.begin()) {
    Serial.println("FAILED");
    for(;;);
  } else {
    Serial.println("SUCCESS");
  }

  if(!screen.begin()) {
    Serial.println(F("SSD_1306 Allocation Failed"));
    for(;;);
  }

  
  sensor.setOnBeatDetectedCallback(onBeatDetected);

}

float lastReadHR = 0;
float lastReadBO = 0;

float prevReadHR = 0.0;
float PrevReadBO = 0.0;

void displayResult() {
  while(true) {
    screen.clearDisplay();
    screen.setTextSize(1);
        screen.setTextColor(SSD1306_WHITE);
        screen.setCursor(0,0);
        screen.print("HEARTRATE: ");
        screen.print(lastReadHR/30);
        screen.println(" bpm");
        screen.print("BLOOD OXYGEN: ");
        screen.print(lastReadBO/30);
        screen.println(" %");
    screen.drawRoundRect(0, screen.height()/2, screen.width(), screen.height()/2,
      screen.height()/4, SSD1306_WHITE);
      screen.setTextSize(1);
      screen.setTextColor(SSD1306_WHITE);
      screen.setCursor(screen.height()/4, screen.height()/8+screen.height()/2);
      screen.println("CONTINUE");
      screen.display();

      if(digitalRead(START) == LOW) {
        screen.fillRoundRect(0, screen.height()/2, screen.width(), screen.height()/2,
        screen.height()/4, SSD1306_INVERSE);
        screen.display();
        delay(200);
        return;
    } 
  }
}

void loop() {
  sensor.update();

  lastReadBO = 0.0;
  lastReadHR = 0.0;

  screen.clearDisplay();
  screen.setTextSize(1);
      screen.setTextColor(SSD1306_WHITE);
      screen.setCursor(0,screen.height()/2);
      screen.println("Taking reading please wait..");
      screen.display();
  screen.display();

 if(millis() - tsLastReport  > REPORTING_PERIOD_MS) {
    float HR = sensor.getHeartRate();
    float BO = sensor.getSpO2();
    screen.clearDisplay();
    screen.display();
    Serial.print("HR: ");
    Serial.print(HR);
    Serial.print("bpm\nSpO2: ");
    Serial.print(BO);
    Serial.println("%");


    if (HR > 60 && HR < 100 && BO > 95){
      count++;
      lastReadHR += HR;
      lastReadBO += BO;
    }
    // Serial.println(millis());
    // Serial.println(tsLastReport);

    tsLastReport = millis();
  }

  if(digitalRead(RESET) == LOW) {
    displayMenu();
    sensor.begin();
  }

  if(count >= 30) {
    displayResult();
    displayMenu();
    sensor.begin();
    count = 0;
  }

}