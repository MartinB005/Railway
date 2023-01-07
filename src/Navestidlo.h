#pragma once

#include <Arduino.h>

#define NAVESTIDLO

class Navestidlo {

    public:
        
        char* currentTrack;
        int directionMode;
        int RED_VAL;

        Navestidlo(int pinC, ...) {
            va_list l_Arg;

            int pin = 0;
            va_start(l_Arg, pinC);

            pins = (int*) malloc(sizeof(int));

            for(int i = 0; i < pinC; i++) {
                pin = va_arg(l_Arg, int);  
                pinsCount++;
                if(i > 0) pins = (int*) realloc(pins, pinsCount * sizeof(int));
                pins[i] = pin;
                pinMode(pin, OUTPUT);
            }

            va_end(l_Arg);
        }

        void stoj() {
            changeState(RED_VAL);
        }

        virtual void volno() = 0;
        virtual void posun() = 0;

        void changeState(byte state) {
            for(int i = 0; i < pinsCount; i++) {
                digitalWrite(pins[i], (state >> (pinsCount - i - 1) & 1 ? HIGH : LOW));
            }
        }


    private:

        int* pins;
        int pinsCount;
};