//import
#include "LedControl.h"
#include <ezButton.h>

//hardware
LedControl lc = LedControl(5, 7, 6, 1);
ezButton button_L(9), button_U(10), button_D(11), button_R(12), button(13);

//timing
unsigned long lastTick = 0;
const unsigned long TICK_INTERVAL = 1000; // 1 second heartbeats
int secondsCounter = 0; 
int hoursCounter = 0;
unsigned long lastMeowBlink = 0;
bool meowMouthOpen = false;
int mouthX = 4; 
int mouthY = 1;
int HealthDecayHours = 0;
int sleepHoursCounter = 0;
int healthHours = 0; 
int happinessHours = 0;  
int energyDecayClock = 0;

enum AppState { CAT_MENU, MAIN_MENU, SELECTING_GAME, SELECTING_FOOD, SLEEPING, ANIMATION };
enum StatType { NONE, HAPPINESS, HUNGER, ENERGY, HEALTH };
AppState currentState = CAT_MENU;
StatType currentStat = NONE;
int selectionIndex = 0; 

//CAT
struct Cat { int h=8, hu=8, e=8, he=8; int activeFrame=0; bool isSleeping=false; bool meowing = false;};
Cat mycat;

void setup() {
    lc.shutdown(0, false); 
    lc.setIntensity(0, 0); 
    lc.clearDisplay(0);

    button.setDebounceTime(20); 
    button_L.setDebounceTime(20);
    button_U.setDebounceTime(20); 
    button_D.setDebounceTime(20); 
    button_R.setDebounceTime(20);

    kitty();
}

void loop() {
    handleInputs();
    updateTiming();
}

void updateTiming() {
    if (millis() - lastTick >= TICK_INTERVAL) {
        lastTick = millis();
        secondsCounter++;
        
        if (secondsCounter >= 3600) { 
            secondsCounter = 0;
            
            //health
            healthHours++;
            if (healthHours >= HealthDecayHours) {
                mycat.he = max(mycat.he - 4, 0); 
                healthHours = 0; 
                HealthDecayHours = random(48, 121); 
            }

            //happyness
            happinessHours++;
            if (happinessHours >= 3) {
                if (mycat.h > 0) mycat.h--; 
                happinessHours = 0;
            }

            // hunger
            if (mycat.hu > 0) mycat.hu--; 

            //energy
            if (currentState == SLEEPING) {
                sleepHoursCounter++;
                
                if (mycat.e < 8) {
                    mycat.e = min(mycat.e + 2, 8);
                }
                
                if (sleepHoursCounter >= 8) {
                    currentState = CAT_MENU;
                    currentStat = NONE;
                    mycat.isSleeping = false;
                    sleepHoursCounter = 0;
                    energyDecayClock = 0; 
                    kitty(); 
                } else {
                    sleep(); 
                }
            } 
            else {
                energyDecayClock += 2; 
                if (energyDecayClock >= 3) {
                    if (mycat.e > 0) mycat.e--;
                    energyDecayClock -= 3; 
                }
            }

            //refresh
            if (currentState == MAIN_MENU) {
                if (currentStat == HAPPINESS) happy_pic();
                else if (currentStat == HUNGER) feed_pic();
                else if (currentStat == ENERGY) energy_pic();
                else if (currentStat == HEALTH) health_pic();
            }
        }
    }
}

void handleInputs() {
    button.loop(); button_L.loop(); button_U.loop(); button_D.loop(); button_R.loop();

    switch (currentState) {
        case CAT_MENU:
            if (button.isPressed()) { currentState = MAIN_MENU; face(); }
            
            if (button_L.isPressed()) { 
                kitty_ld(); 
                mouthX = 3; mouthY = 0; 
            } 
            if (button_R.isPressed()) { 
                kitty(); 
                mouthX = 4; mouthY = 1; 
            } 
            if (button_U.isPressed()) { 
                kitty_lu(); 
                mouthX = 3; mouthY = 1;
            } 
            if (button_D.isPressed()) { 
                kitty_rd(); 
                mouthX = 4; mouthY = 0;
            } 

            //meow
            if (mycat.h == 0 || mycat.hu == 0 || mycat.e == 0 || mycat.he == 0) {
                if (millis() - lastMeowBlink >= 450) { 
                    lastMeowBlink = millis();
                    meowMouthOpen = !meowMouthOpen;
                    
                    lc.setLed(0, mouthX, mouthY, meowMouthOpen);
                }
            }
            break;

        case MAIN_MENU:
            if (button_L.isPressed()) { 
                currentStat = (currentStat == HAPPINESS) ? NONE : HAPPINESS; 
                if (currentStat == HAPPINESS) happy_pic(); else face();
            }
            if (button_U.isPressed()) { 
                currentStat = (currentStat == HUNGER) ? NONE : HUNGER; 
                if (currentStat == HUNGER) feed_pic(); else face();
            }
            if (button_D.isPressed()) { 
                currentStat = (currentStat == ENERGY) ? NONE : ENERGY; 
                if (currentStat == ENERGY) energy_pic(); else face();
            }
            if (button_R.isPressed()) { 
                currentStat = (currentStat == HEALTH) ? NONE : HEALTH; 
                if (currentStat == HEALTH) health_pic(); else face();
            }

            if (button.isPressed()) {
                if (currentStat == NONE)           { currentState = CAT_MENU; kitty();}
                else if (currentStat == HAPPINESS) { currentState = SELECTING_GAME; selectionIndex = 0; purr();}
                else if (currentStat == HUNGER)    { currentState = SELECTING_FOOD; selectionIndex = 0; fish();} 
                else if (currentStat == ENERGY)    { currentState = SLEEPING; mycat.isSleeping = true; sleep(); sleepHoursCounter = 0;}
                else if (currentStat == HEALTH)    { medicine(); }
            }
            break;

        case SELECTING_GAME:
            if (button_L.isPressed()) {selectionIndex = 0; purr();}
            if (button_U.isPressed()) {selectionIndex = 1; laser();}
            if (button.isPressed()) {
                if (selectionIndex == 0) purr_game();
                else laser_game(); 
            }
            break;
            
        case SELECTING_FOOD:
            if (button_L.isPressed()) { selectionIndex = 0; fish(); }
            if (button_U.isPressed()) { selectionIndex = 1; chicken(); }
            if (button_D.isPressed()) { selectionIndex = 2; bug(); }
            
            if (button.isPressed()) {
                if(selectionIndex == 0) mycat.hu = min(mycat.hu + 2, 8); 
                if(selectionIndex == 1) mycat.hu = min(mycat.hu + 4, 8); 
                if(selectionIndex == 2) mycat.hu = min(mycat.hu + 1, 8); mycat.h = min(mycat.h + 1, 8);
                
                currentState = MAIN_MENU;
                currentStat = NONE;
                face(); 
            }
            break;
        case SLEEPING:
            if (button.isPressed() || button_L.isPressed() || button_R.isPressed() || button_U.isPressed() || button_D.isPressed()) {
                currentState = CAT_MENU;
                currentStat = NONE;
                mycat.isSleeping = false;
                sleepHoursCounter = 0;
                kitty();
            }
            break;
    }
}

//games
void purr_game() {
    
    int mashes = 0;
    unsigned long startTime = millis();
    kitty(); 

    while (millis() - startTime < 5000) { 
        button.loop();
        if (button.isPressed()) {
            mashes++;
            
            int randomCat = random(0, 4);
            if (randomCat == 0) kitty();
            else if (randomCat == 1) kitty_lu();
            else if (randomCat == 2) kitty_ld();
            else if (randomCat == 3) kitty_rd();

            int bars = min(mashes / 2, 8);
            for(int i = 0; i < bars; i++) {
                lc.setLed(0, 7, i, true);
            }
        }
    }

    if ((mashes/2) >= 8) {
        mycat.h = min(mycat.h + 1, 8); 
        
        //WIN
        for (int i = 0; i < 3; i++) {
            lc.clearDisplay(0);
            delay(100);
            for (int r = 0; r < 8; r++) {
                for (int c = 0; c < 8; c++) lc.setLed(0, r, c, true);
            }
            delay(100);
        }
    }
    
    currentState = MAIN_MENU;
    currentStat = NONE;
    face();
}

void laser_game() {
    
    int points = 0;
    int targetX, targetY, correctButton;
    unsigned long startTime;

    bool spawnNewDot = true; 

    while (points < 10 || millis() - startTime > 15000) {
        if (spawnNewDot) {
            targetX = random(0, 8);
            targetY = random(0, 8);
            
            if (targetX < 4 && targetY < 4)       correctButton = 0; // LEFT - bottom left
            else if (targetX < 4 && targetY >= 4)  correctButton = 1; // UP - upper left
            else if (targetX >= 4 && targetY < 4)  correctButton = 2; // DOWN - lower right
            else                                   correctButton = 3; // RIGHT - upper right

            lc.clearDisplay(0);
            lc.setLed(0, targetX, targetY, true);
            
            startTime = millis();
            spawnNewDot = false;
        }

        button.loop(); button_L.loop(); button_U.loop(); button_D.loop(); button_R.loop();

        if (button.isPressed()) {
            break;
        }

        if (millis() - startTime >= 1000) {
            spawnNewDot = true; 
            continue;
        }

        bool buttonPressedThisFrame = false;
        
        if (button_L.isPressed()) {
            if (correctButton == 0) points++;
            buttonPressedThisFrame = true;
        }
        else if (button_U.isPressed()) {
            if (correctButton == 1) points++;
            buttonPressedThisFrame = true;
        }
        else if (button_D.isPressed()) {
            if (correctButton == 2) points++;
            buttonPressedThisFrame = true;
        }
        else if (button_L.isPressed()) {
            if (correctButton == 3) points++;
            buttonPressedThisFrame = true;
        }

        if (buttonPressedThisFrame) {
            spawnNewDot = true;
            delay(100); 
        }
    }

    if (points == 10) {
        mycat.h = min(mycat.h + 2, 8);
        
        //WIN
        for (int i = 0; i < 3; i++) {
            lc.clearDisplay(0);
            delay(100);
            for (int r = 0; r < 8; r++) {
                for (int c = 0; c < 8; c++) lc.setLed(0, r, c, true);
            }
            delay(100);
        }
    }

    currentState = MAIN_MENU;
    currentStat = NONE;
    face();
}

//drawn functions
void kitty() {
  lc.clearDisplay(0); 
  lc.setLed(0,0,1,true);
  lc.setLed(0,1,1,true);
  lc.setLed(0,7,1,true);
  lc.setLed(0,4,2,true);
  lc.setLed(0,0,3,true);
  lc.setLed(0,1,3,true);
  lc.setLed(0,3,3,true);
  lc.setLed(0,5,3,true);
  lc.setLed(0,7,3,true);
  lc.setLed(0,2,5,true);
  lc.setLed(0,3,5,true);
  lc.setLed(0,5,5,true);
  lc.setLed(0,6,5,true);
  lc.setLed(0,2,6,true);
  lc.setLed(0,6,6,true);
}

void kitty_rd(){
  kitty();
  lc.setLed(0,3,3,false);
  lc.setLed(0,3,2,true);
  lc.setLed(0,4,2,false);
  lc.setLed(0,4,1,true);
  lc.setLed(0,5,3,false);
  lc.setLed(0,5,2,true);
}

void kitty_lu() {
  lc.clearDisplay(0); 
  lc.setLed(0,0,1,true); 
  lc.setLed(0,6,1,true); 
  lc.setLed(0,7,1,true); 
  lc.setLed(0,3,2,true); 
  lc.setLed(0,0,3,true); 
  lc.setLed(0,6,3,true); 
  lc.setLed(0,2,3,true); 
  lc.setLed(0,4,3,true); 
  lc.setLed(0,7,3,true); 
  lc.setLed(0,1,5,true);
  lc.setLed(0,2,5,true);
  lc.setLed(0,4,5,true);
  lc.setLed(0,5,5,true);
  lc.setLed(0,1,6,true);
  lc.setLed(0,5,6,true);
}

void kitty_ld(){

  kitty_lu();
  lc.setLed(0,2,3,false);
  lc.setLed(0,2,2,true);
  lc.setLed(0,3,2,false);
  lc.setLed(0,3,1,true);
  lc.setLed(0,4,3,false);
  lc.setLed(0,4,2,true);
}

void sleep(){
  kitty();
  lc.setLed(0,3,3,false);
  lc.setLed(0,3,2,true);
  lc.setLed(0,2,2,true);
  lc.setLed(0,4,2,false);
  lc.setLed(0,4,1,true);
  lc.setLed(0,5,3,false);
  lc.setLed(0,5,2,true);
  lc.setLed(0,6,2,true);
}

void face(){
  lc.clearDisplay(0);
  for(int i = 1; i < 8; i++){
    lc.setLed(0,i,4,true);
    lc.setLed(0,i,5,true);
  }
  lc.setLed(0,4,6,false);
  lc.setLed(0,4,5,false);
  lc.setLed(0,4,4,false);
  lc.setLed(0,2,2,true);
  lc.setLed(0,3,1,true);
  lc.setLed(0,4,2,true);
  lc.setLed(0,5,1,true);
  lc.setLed(0,6,2,true);

}

void fish(){
  lc.clearDisplay(0);
  for(int i = 1; i < 4; i++){
    for(int j = 4; j < 7; j++){
      lc.setLed(0,i,j,true);
    }
  }
  for(int i = 2; i < 7; i++){
    lc.setLed(0,i,3,true);
  }
  for(int i = 1; i < 6; i++){
    lc.setLed(0,4,i,true);
  }
  lc.setLed(0,5,2,true);
}

void chicken(){
  lc.clearDisplay(0);
  for(int i = 3; i < 7; i++){
    for(int j = 2; j < 5; j++){
      lc.setLed(0,i,j,true);
    }
  }
  lc.setLed(0,1,5,true);
  lc.setLed(0,2,5,true);
  lc.setLed(0,3,5,true);
  lc.setLed(0,2,6,true);
  lc.setLed(0,3,6,true);
  lc.setLed(0,6,4,true);
  lc.setLed(0,6,3,true);
  lc.setLed(0,4,0,true);
  lc.setLed(0,4,1,true);
}

void bug(){
  lc.clearDisplay(0);
  for(int i = 1; i < 4; i++){
    lc.setLed(0,2,i,true);
    lc.setLed(0,5,i,true);
  }
  for(int i = 0; i < 6; i++){
    lc.setLed(0,3,i,true);
    lc.setLed(0,4,i,true);
  }
  lc.setLed(0,1,0,true);
  lc.setLed(0,1,2,true);
  lc.setLed(0,1,4,true);
  lc.setLed(0,6,0,true);
  lc.setLed(0,6,2,true);
  lc.setLed(0,6,4,true);
  lc.setLed(0,2,6,true);
  lc.setLed(0,5,6,true);

}

void happy_pic(){
  lc.clearDisplay(0);
  for(int i = 3; i < 6; i++){
    lc.setLed(0,i,3,true);
  }
  lc.setLed(0,2,4,true);
  lc.setLed(0,3,6,true);
  lc.setLed(0,5,6,true);
  lc.setLed(0,6,4,true);

  for(int i = 0; i < mycat.h; i++){
    lc.setLed(0,i,1,true);
  }
}

void feed_pic(){
  lc.clearDisplay(0);
  for(int i = 2; i < 5; i++){
    for(int j = 4; j < 7; j++){
      lc.setLed(0,i,j,true);
    }
  }
  for(int i = 4; i < 7; i++){
    lc.setLed(0,6,i,true);
  }
  lc.setLed(0,1,5,true);
  lc.setLed(0,5,5,true);

  for(int i = 0; i < mycat.hu; i++){
    lc.setLed(0,i,1,true);
  }
}

void energy_pic(){
  lc.clearDisplay(0);
  lc.setLed(0,3,3,true);
  lc.setLed(0,3,5,true);
  lc.setLed(0,4,4,true);
  lc.setLed(0,4,5,true);
  lc.setLed(0,4,6,true);
  lc.setLed(0,5,5,true);
  lc.setLed(0,5,7,true);

  for(int i = 0; i < mycat.e; i++){
    lc.setLed(0,i,1,true);
  }
}


void health_pic(){
  lc.clearDisplay(0);
  for(int i = 2; i < 6; i++){
    lc.setLed(0,i,4,true);
    lc.setLed(0,i,5,true);    
  }
  lc.setLed(0,3,6,true);
  lc.setLed(0,4,6,true);
  lc.setLed(0,3,3,true);
  lc.setLed(0,4,3,true);

  for(int i = 0; i < mycat.he; i++){
    lc.setLed(0,i,1,true);
  }
}

void medicine() {
    for(int i = 0; i < 3; i++) {
        med1();
        delay(500);
        med2();
        delay(500);
    } 

    mycat.he = min(mycat.he+4,8); 

    currentState = MAIN_MENU;
    currentStat = NONE;

    kitty(); 
}

void med1(){
  lc.clearDisplay(0);
  lc.setLed(0,1,1,true);
  lc.setLed(0,2,2,true);
  lc.setLed(0,2,3,true);
  lc.setLed(0,3,2,true);
  lc.setLed(0,3,3,true);
  lc.setLed(0,3,4,true);
  lc.setLed(0,4,3,true);
  lc.setLed(0,4,4,true);
  lc.setLed(0,4,5,true);
  lc.setLed(0,5,4,true);
  lc.setLed(0,5,5,true);
  lc.setLed(0,5,7,true);
  lc.setLed(0,6,6,true);
  lc.setLed(0,7,5,true);
}

void med2(){
  lc.clearDisplay(0);
  lc.setLed(0,1,1,true);
  lc.setLed(0,2,2,true);
  lc.setLed(0,2,3,true);
  lc.setLed(0,3,2,true);
  lc.setLed(0,3,3,false);
  lc.setLed(0,3,4,true);
  lc.setLed(0,4,3,true);
  lc.setLed(0,4,4,false);
  lc.setLed(0,4,5,true);
  lc.setLed(0,5,4,true);
  lc.setLed(0,5,5,true);
  lc.setLed(0,4,6,true);
  lc.setLed(0,6,4,true);
  lc.setLed(0,5,7,false);
  lc.setLed(0,6,6,false);
  lc.setLed(0,7,5,false);
}

void purr(){
  lc.clearDisplay(0);
  lc.setLed(0,1,5,true);
  lc.setLed(0,2,6,true);
  lc.setLed(0,3,5,true);
  lc.setLed(0,5,5,true);
  lc.setLed(0,6,6,true);
  lc.setLed(0,7,5,true);
  lc.setLed(0,2,3,true);
  lc.setLed(0,3,2,true);
  lc.setLed(0,4,3,true);
  lc.setLed(0,5,2,true);
  lc.setLed(0,6,3,true);
}

void laser(){
 lc.clearDisplay(0);
 for(int i = 0; i < 6; i++){
    lc.setLed(0,3,i,true);
    lc.setLed(0,4,i,true);    
  }
  lc.setLed(0,2,4,true);
  lc.setLed(0,5,4,true); 
}
