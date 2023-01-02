#include <Arduino.h>
#include <OdchodoveNavestidlo.h>

#define POSITION_DOWN 0
#define POSITION_MIDDLE 1
#define POSITION_UP 2

struct Switch {
    char* tag;
    int pin;
    OdchodoveNavestidlo navestidlo;
    short state;
};

class Panel {

    
    public:
        
        void connectSwitch(char* tag, int pin, OdchodoveNavestidlo navestidlo) {
            switchList = (Switch*) realloc(switchList, count * sizeof(Switch));
            *(switchList + count) = {tag, pin, navestidlo, -1};
            count++;
            pinMode(pin, INPUT);
        }

        void check() {
            for(int i = 0; i < count; i++) {
                Switch currentSw = *(switchList + i);
                int value = analogRead(currentSw.pin);
                int state = value > 1010 ? POSITION_UP : POSITION_DOWN;
                if(value < 800) state = POSITION_MIDDLE;

                if(state != currentSw.state) {
                    if(state == POSITION_UP) currentSw.navestidlo.volno();
                    else if(state == POSITION_DOWN) currentSw.navestidlo.posun();
                    else currentSw.navestidlo.stoj();
                    currentSw.state = state;
                }
            }
        }


    private:
        Switch* switchList = (Switch*) malloc(sizeof(Switch));
        int count = 0;
};