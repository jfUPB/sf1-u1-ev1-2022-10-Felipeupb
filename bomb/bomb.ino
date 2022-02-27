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
uint8_t btnpr =0;

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
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(ARM_BTN, INPUT_PULLUP);

        btnsState = BtnsStates::PRESS;


        break;
      }
    case BtnsStates::PRESS: {

        if (digitalRead(UP_BTN) == LOW) {
          btnpr = 1;
          referenceTime = millis();
          btnsState = BtnsStates::STABLE;
        } else if(digitalRead(DOWN_BTN) == LOW) {
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

        if (digitalRead(UP_BTN) == HIGH&& btnpr ==1) {
          btnsState = BtnsStates::PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          btnsState = BtnsStates::RELEASE;
        }if (digitalRead(DOWN_BTN) == HIGH&& btnpr ==2) {
          btnsState = BtnsStates::PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          btnsState = BtnsStates::RELEASE;
        }else if (digitalRead(ARM_BTN) == HIGH&& btnpr ==3) {
          btnsState = BtnsStates::PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          btnsState = BtnsStates::RELEASE;
        }

        break;
      }

    case BtnsStates::RELEASE: {

        if (digitalRead(UP_BTN) == HIGH && btnpr ==1) {
          evBtns = true;
          evBtnsData = UP_BTN;
          Serial.println("UP_BTN");
          btnsState = BtnsStates::PRESS;
        }else if (digitalRead(DOWN_BTN) == HIGH && btnpr ==2) {
          evBtns = true;
          evBtnsData = DOWN_BTN;
          Serial.println("DOWN_BTN");
          btnsState = BtnsStates::PRESS;
        }else if (digitalRead(ARM_BTN) == HIGH && btnpr ==3) {
          evBtns = true;
          evBtnsData = ARM_BTN;
          Serial.println("ARM_BTN");
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
          if (evBtnsData == UP_BTN && btnpr ==1 ) {
            if (counter < 60) {
              counter++;
            }
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
          }else if (evBtnsData == DOWN_BTN && btnpr ==2 ) {
            if (counter < 60) {
              counter--;
            }else if (counter > 1){
              evBtnsData = 0;
            }
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
          }else if (evBtnsData == ARM_BTN && btnpr ==3) {
          bombState = BombStates::ARMED;
          } 

        }

        break;
      }

    case BombStates::ARMED: {
      int password[]= {1,1,0,0,1,0}; //UP,UP,DOWN, DOWN, UP, DOWN, ARM = 110010
      int clave[5];
      int i;

      if (evBtns == true) {
          evBtns = false;
          if (evBtnsData == UP_BTN && btnpr ==1 ) {
            i = 1;
          }else if (evBtnsData == DOWN_BTN && btnpr ==2 ) {
            i = 0;
          }else if (evBtnsData == ARM_BTN && btnpr ==3) {
            
          if ( counter !=0 )
          {
            for( int i = 0; i < 5; i++){
              clave[i];
            }
            if( clave == password){
              bombState = BombStates::DISARMED;
            }
          
          } else if ( counter == 0){
            display.clear();
            display.drawString(10, 20, String("GAME OVER"));
            display.display();
            Serial.println("Game Over");
            if (evBtnsData == ARM_BTN && btnpr ==3) {
          bombState = BombStates::DISARMED;
          } 
          }
          
          }
      }
      /*static uint32_t previousMillis = 0;
      uint32_t currentMillis = millis();

      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        counter = previousMillis;
      
      }*/
      

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
