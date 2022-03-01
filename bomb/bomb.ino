#include "SSD1306Wire.h"

#define BOMB_OUT 25
#define LED_COUNT 25
#define LED_COUNT1 26
#define LED_COUNT2 27
#define LED_COUNT3 14
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
uint8_t btnpr = 0;
const uint32_t interval = 1000;


void loop() {
  serialTask();
  btnsTask();
  bombTask();
}

void btnsTask() {
  enum class BtnsStates {INIT, PRESS, STABLE, RELEASE};
  static BtnsStates btnsState =  BtnsStates::INIT;
  static uint32_t referenceTime;
  static uint8_t ledState = LOW;


  const uint32_t STABLETIMEOUT = 100;

  switch (btnsState) {
    case BtnsStates::INIT: {

        pinMode(UP_BTN, INPUT_PULLUP);
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(ARM_BTN, INPUT_PULLUP);
        pinMode(LED_COUNT, OUTPUT);
        pinMode(LED_COUNT1, OUTPUT);
        pinMode(LED_COUNT2, OUTPUT);
        pinMode(LED_COUNT3, OUTPUT);


        btnsState = BtnsStates::PRESS;


        break;
      }
    case BtnsStates::PRESS: {

        if (digitalRead(UP_BTN) == LOW) {
          btnpr = 1;
          referenceTime = millis();
          btnsState = BtnsStates::STABLE;
        } else if (digitalRead(DOWN_BTN) == LOW) {
          btnpr = 2;
          referenceTime = millis();
          btnsState = BtnsStates::STABLE;
        } else if (digitalRead(ARM_BTN) == LOW) {
          btnpr = 3;
          referenceTime = millis();
          btnsState = BtnsStates::STABLE;
        }

        break;
      }

    case BtnsStates::STABLE: {

        if (digitalRead(UP_BTN) == HIGH && btnpr == 1) {
          btnsState = BtnsStates::PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          btnsState = BtnsStates::RELEASE;
        } if (digitalRead(DOWN_BTN) == HIGH && btnpr == 2) {
          btnsState = BtnsStates::PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          btnsState = BtnsStates::RELEASE;
        } else if (digitalRead(ARM_BTN) == HIGH && btnpr == 3) {
          btnsState = BtnsStates::PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          btnsState = BtnsStates::RELEASE;
        }

        break;
      }

    case BtnsStates::RELEASE: {

        if (digitalRead(UP_BTN) == HIGH && btnpr == 1) {
          evBtns = true;
          evBtnsData = UP_BTN;
          Serial.println("UP_BTN");
          btnsState = BtnsStates::PRESS;
          digitalWrite(LED_COUNT, HIGH);
          delay(300); // Wait for 1000 millisecond(s)
          digitalWrite(LED_COUNT, LOW);
          delay(300);

        } else if (digitalRead(DOWN_BTN) == HIGH && btnpr == 2) {
          evBtns = true;
          evBtnsData = DOWN_BTN;
          Serial.println("DOWN_BTN");
          btnsState = BtnsStates::PRESS;
          digitalWrite(LED_COUNT1, HIGH);
          delay(300); // Wait for 1000 millisecond(s)
          digitalWrite(LED_COUNT1, LOW);
          delay(300);
        } else if (digitalRead(ARM_BTN) == HIGH && btnpr == 3) {
          evBtns = true;
          evBtnsData = ARM_BTN;
          Serial.println("ARM_BTN");
          btnsState = BtnsStates::PRESS;
          digitalWrite(LED_COUNT2, HIGH);
          delay(300); // Wait for 1000 millisecond(s)
          digitalWrite(LED_COUNT2, LOW);
          delay(300);
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
  static uint8_t ledState = LOW;
  static uint32_t previousMillis = 0;

  switch (bombState) {
    case BombStates::INIT: {

        pinMode(LED_COUNT3, OUTPUT);
        counter = 20;
        display.init();
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);

        display.clear();
        display.drawString(20, 15, String(counter));
        display.display();
        digitalWrite(LED_COUNT, LOW);
        digitalWrite(LED_COUNT1, LOW);
        digitalWrite(LED_COUNT2, LOW);
        digitalWrite(LED_COUNT3, LOW);

        bombState = BombStates::DISARMED;
        break;
      }
    case BombStates::DISARMED: {

        digitalWrite(LED_COUNT3, HIGH);

        if (evBtns == true) {
          evBtns = false;
          if (evBtnsData == UP_BTN && btnpr == 1 ) {
            if (counter < 60) {
              counter++;
            }
            display.clear();
            display.drawString(20, 15, String(counter));
            display.display();
          } else if (evBtnsData == DOWN_BTN && btnpr == 2 ) {
            if (counter > 10) {
              counter--;
            }
            display.clear();
            display.drawString(20, 15, String(counter));
            display.display();
          } else if (evBtnsData == ARM_BTN && btnpr == 3) {
            bombState = BombStates::ARMED;
          }

        }

        break;
      }

    case BombStates::ARMED: {
        const uint8_t zise = 7;
        static uint8_t password[zise] = {UP_BTN, UP_BTN, DOWN_BTN, DOWN_BTN, UP_BTN, DOWN_BTN, ARM_BTN};
        static uint8_t clave[zise];
        static uint8_t cuntas = 0;

        static bool pstrue = false;



        uint32_t currentMillis = millis();


        if ( (currentMillis - previousMillis) >= interval) {
          previousMillis = currentMillis;
          counter --;
          if (counter == 0) {
            display.clear();
            display.drawString(10, 20, String("BOOM!"));
            display.display();
            digitalWrite(LED_COUNT, HIGH);
            digitalWrite(LED_COUNT1, HIGH);
            digitalWrite(LED_COUNT2, HIGH);
            digitalWrite(LED_COUNT3, HIGH);
            delay(1000);
            display.clear();
            display.drawString(10, 10, String("GAME"));
            display.drawString(9, 30, String("OVER!"));
            display.display();
            Serial.println("Game Over");
            delay(1500);
            cuntas = 0;
            bombState = BombStates::INIT;
          }
          display.clear();
          display.drawString(20, 15, String(counter));
          display.display();
          if (ledState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          digitalWrite(LED_COUNT3, ledState);
        }

        if (evBtns == true) {
          evBtns = false;
          if (cuntas < zise) {

            if (evBtnsData == UP_BTN && btnpr == 1 ) {
              clave[cuntas] = evBtnsData;
            }
            else if (evBtnsData == DOWN_BTN && btnpr == 2 ) {
              clave[cuntas] = evBtnsData;
            }
            else if (evBtnsData == ARM_BTN && btnpr == 3) {
              clave[cuntas] = evBtnsData;
            }
            cuntas++;
          }
        }

        else if (cuntas == zise) {
          Serial.println("full password");
          disarmBoom(clave, password, zise, &pstrue);
          // verificas so clave es igual a password
          if (pstrue == true) {
            Serial.println("correct password");
            display.display();
            digitalWrite(LED_COUNT, LOW);
            digitalWrite(LED_COUNT1, LOW);
            digitalWrite(LED_COUNT2, LOW);
            digitalWrite(LED_COUNT3, LOW);
            display.clear();
            display.drawString(10, 10, String("NICE"));
            display.drawString(12, 30, String("JOB!"));
            display.display();
            delay(1500);
            bombState = BombStates::INIT;
            for ( uint8_t i = 0; i < zise; i++) {
              clave[i] = 0;

            }
          }
          else {
            Serial.println("incorrect password");
            cuntas = 0;
            for (uint8_t j = 0; j < zise; j++) {
              clave[j] = 0;
            }
          }
        }

        break;
      }

    default:
      break;
  }
}
void disarmBoom(uint8_t *verClave, uint8_t *verPasswork, uint8_t vecZice, bool *compro) {
  for (uint8_t i = 0; i < vecZice; i++) {
    if (verPasswork[i] == verClave[i]) {
      *compro = true;
    }
    else {
      *compro = false;
      break;
    }
  }
}


void serialTask() {
  if (Serial.available() > 0) {
    int dataIn = Serial.read();
    if (dataIn == 'u') {
      evBtns = true;
      evBtnsData = UP_BTN;
    } else if (dataIn == 'd') {
      evBtns = true;
      evBtnsData = DOWN_BTN;
    }
  }
}
