#pragma once

#include <Arduino.h>
#include <Navestidlo.h>
#include <Zhlavie.h>
#include <Predzvest.h>

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
                bool limitedSpeed = zhlavie->limitedSpeed(kolaj, VCHOD);
                changeState(limitedSpeed ? GREEN_40 : GREEN);
                
                if(limitedSpeed) presage->volno40();
                else presage->signalVolno();

                zhlavie->reserveWay(kolaj, VCHOD, this);
                go = true;
            }
        }

        void posun() {
            go = true;
            stoj();
        };

        void stoj() {
            if(go) {
                Navestidlo::stoj();
                presage->vystraha();
                zhlavie->releaseWay(kolaj, VCHOD);
                go = false;
            }
        }

        void predzvest(Predzvest* predzvest) {
            this->presage = predzvest;
        }

    private:

        Predzvest* presage;
        Zhlavie* zhlavie;
        char* kolaj;
        bool go;
    
};