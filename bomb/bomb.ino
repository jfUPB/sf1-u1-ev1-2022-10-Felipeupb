#include "SSD1306Wire.h"

#define BOMB_OUT 25
#define LED_COUNT 26
#define UP_BTN 13
#define DOWN_BTN 32
#define ARM_BTN 33

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

void btnsTask();
void bombTask();

void setup() {
  btnsTask();
  bombTask();

}

// Eventos para comunicar
bool evBtns = false;
uint8_t evBtnsData = 0;

void loop() {
  btnsTask();
  bombTask();
}

void btnsTask() {

}

void bombTask() {
  enum class BombStates {INIT, DISARMED, ARMED};
  static BombStates bombState =  BombStates::INIT;
  static uint8_t counter;

  switch (bombState) {
    case BombStates::INIT: {
        counter = 20;
        display.init();
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);

        display.clear();
        display.drawString(10, 20, String(counter));  
        display.display();

        bombState = BombStates::DISARMED;
        break;
      }
    case BombStates::DISARMED: {

        break;
      }

    case BombStates::ARMED: {

        break;
      }

    default:
      break;


  }



}
