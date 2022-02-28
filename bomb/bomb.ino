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
  static uint8_t ledState = LOW;
  static uint32_t previousMillis = 0;

  switch (bombState) {
    case BombStates::INIT: {

      pinMode(LED_COUNT, OUTPUT);
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

      digitalWrite(LED_COUNT, HIGH);

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
            if (counter > 10) {
              counter--;
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
      static int password[]= {1,1,2,2,1,2}; //UP,UP,DOWN, DOWN, UP, DOWN, ARM = 112212
      static int j=6;
      static uint8_t st = 0;
      int clave[j];
      
      static bool pstrue;
      
      

      uint32_t currentMillis = millis();
      
        if ( counter !=255 ){
          if ( (currentMillis - previousMillis) >= interval) {
            previousMillis = currentMillis;
            counter --;
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
              if (ledState == LOW) {
                ledState = HIGH;
              } else {
                ledState = LOW;
              }
             digitalWrite(LED_COUNT, ledState);
             if (evBtns == true) {
              evBtns = false;
              if (evBtnsData == UP_BTN && btnpr ==1 ) {
                st = 1;
                Serial.println(st);
              }else if (evBtnsData == DOWN_BTN && btnpr ==2 ){
                st = 2;
                Serial.println(st);
              } 
               for( int i = 0; i < j; i++){
                clave[i];
                if(Serial.available() < 0){
                  st = Serial.parseInt();
              } if(i== 6){
              Serial.println(clave[i]);
              
               if (evBtns == true) {
                  evBtns = false;
                 if (evBtnsData == UP_BTN && btnpr ==1 ) {
                   evBtns = false;
                }else if (evBtnsData == DOWN_BTN && btnpr ==2 ){
                  evBtns = false;
                } 
                if (evBtnsData == ARM_BTN && btnpr ==3) {
                 if(clave == password){
                    pstrue = true;
                  }else{
                    pstrue = false;
                  }  if( pstrue == true){
                Serial.println("correct password");
                bombState = BombStates::INIT; 
             }else if( pstrue == false){
               memset(clave, 0, j);
              Serial.println("incorrect password");
             }
             Serial.println(clave[st,i]);
            }
             
                }
              }
               }
            
              }
               
               
              }
              
          
          }else if ( counter == 255){
              display.clear();
              display.drawString(10, 10, String("GAME"));
              display.drawString(9, 30, String("OVER!"));
              display.display();
             // Serial.println("Game Over");
              digitalWrite(LED_COUNT, HIGH);
              if (evBtns == true) {
              evBtns = false;
              if (evBtnsData == ARM_BTN && btnpr ==3) {
              bombState = BombStates::INIT;
              }
             }
          }  
        
    

      
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
    }else if (dataIn == 'd') {
      evBtns = true;
      evBtnsData = DOWN_BTN;
    }
  }
}
