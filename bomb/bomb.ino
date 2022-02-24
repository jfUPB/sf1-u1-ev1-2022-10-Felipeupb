#include "SSD1306Wire.h"

#define BOMB_OUT 25
#define LED_COUNT 26
#define UP_BTN 13
#define DOWN_BTN 32
#define ARM_BTN 33

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

void btnsTask();
void bombTask();
void serialTask();

void setup() {
  Serial.begin(115200);

  btnsTask();
  bombTask();

}

// Eventos para comunicar
bool evBtns = false;
uint8_t evBtnsData = 0;

void loop() {
  serialTask();
  btnsTask();
  bombTask();
}

void btnsTask() {
  enum class BtnsStates {INIT, PRESS, STABLE, RELEASE};
  static BtnsStates btnsState =  BtnsStates::INIT;
  static uint32_t referenceTime;

  const uint32_t STABLETIMEOUT = 100;

  switch (btnsState) {
    case BtnsStates::INIT: {

        pinMode(UP_BTN, INPUT_PULLUP);

        btnsState = BtnsStates::PRESS;


        break;
      }
    case BtnsStates::PRESS: {

        if (digitalRead(UP_BTN) == LOW) {
          referenceTime = millis();
          btnsState = BtnsStates::STABLE;
        }

        break;
      }

    case BtnsStates::STABLE: {

        if (digitalRead(UP_BTN) == HIGH) {
          btnsState = BtnsStates::PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          btnsState = BtnsStates::RELEASE;
        }

        break;
      }

    case BtnsStates::RELEASE: {

        if (digitalRead(UP_BTN) == HIGH) {
          evBtns = true;
          evBtnsData = UP_BTN;
          Serial.println("UP_BTN");
          btnsState = BtnsStates::PRESS;
        }

        break;
      }

    default:
      break;


  }
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

        if (evBtns == true) {
          evBtns = false;
          if (evBtnsData == UP_BTN ) {
            if (counter < 60) {
              counter++;
            }
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
          }

        }

        break;
      }

    case BombStates::ARMED: {

        break;
      }

    default:
      break;


  }



}

void serialTask() {
  if (Serial.available() > 0) {
    int dataIn = Serial.read();
    if (dataIn == 'u') {
      evBtns = true;
      evBtnsData = UP_BTN;
    }
  }
}
