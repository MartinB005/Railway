#pragma once

#include <Arduino.h>
#include <Navestidlo.h>
#include <Zhlavie.h>

#define GREEN 0b101
#define GREEN_40 0b100
#define YELLOW 0b010
#define YELLOW_40 0b110
#define RED 0b001

class VchodoveNavestidlo : public Navestidlo {

    public:

        VchodoveNavestidlo(int pinA, int pinB, int pinC) : Navestidlo(3, pinA, pinB, pinC) {
            directionMode = VCHOD;
            RED_VAL = 0b001;
            changeState(RED_VAL);
        }

        void place(Zhlavie* zhlavie, char* kolaj) {
            this->zhlavie = zhlavie;
            this->kolaj = kolaj;
        }

        void volno() {
            Kolaj* result = zhlavie->cielovaKolaj(kolaj, VCHOD);

            if(result != NULL) {
                changeState(zhlavie->limitedSpeed(kolaj, VCHOD) ? GREEN_40 : GREEN);
                zhlavie->reserveWay(kolaj, VCHOD, this);
            }
        }

        void posun() {
            stoj();
        };

        void stoj() {
            Navestidlo::stoj();
            zhlavie->releaseWay(kolaj, VCHOD);
        }

    private:

        Zhlavie* zhlavie;
        char* kolaj;
    
};