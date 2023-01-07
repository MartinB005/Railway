#include <Arduino.h>
#include <OdchodoveNavestidlo.h>

#define POSITION_DOWN 0
#define POSITION_MIDDLE 1
#define POSITION_UP 2

struct Switch {
    char* tag;
    int pin;
    Navestidlo* navestidlo;
    short state;
};

class Panel {

    
    public:
        
        void connectSwitch(char* tag, int pin, Navestidlo* navestidlo) {
            Switch newSwitch = {tag, pin, navestidlo, -1};
            switchList[count] = newSwitch;
            Serial.println(pin);
            
            pinMode(pin, INPUT);

            count++;
            switchList = (Switch*) realloc(switchList, count * sizeof(Switch) * 2);
        }

        void check() {
            for(int i = 0; i < count; i++) {
                Switch* currentSw = &switchList[i];
                int value = analogRead(currentSw->pin);
                int state = value >= 1010 ? POSITION_UP : POSITION_DOWN;
                if(value < 800) state = POSITION_MIDDLE;

               // Serial.println(String(i) + ": " + String(currentSw->pin));
               
                
                if(state != currentSw->state) {
                    Serial.print("change detected: ");
                    Serial.println(currentSw->tag);
                    currentSw->state = state;
                    if(state == POSITION_UP) currentSw->navestidlo->volno();
                    else if(state == POSITION_DOWN) currentSw->navestidlo->posun();
                    else currentSw->navestidlo->stoj();
                }
            }
        }


    private:
        Switch* switchList = (Switch*) malloc(sizeof(Switch));
        int count = 0;
};