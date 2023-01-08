#include <Arduino.h>
#include <Navestidlo.h>
#include <Zhlavie.h>

#ifndef ODCHODOVE
#define ODCHODOVE

#define GREEN 0b11
#define GREEN_40 0b10
#define RED 0b00
#define WHITE 0b01


class OdchodoveNavestidlo : public Navestidlo {
    
    public:

        OdchodoveNavestidlo(int pinA, int pinB, bool type) : Navestidlo(2, pinA, pinB) {
            this->type = type;
            directionMode = ODCHOD;
            RED_VAL = 0b00;
        }

        void volno() {
            Kolaj* kolaj = safeDeparture();
            if(kolaj != NULL) {
                int newState = GREEN_40;
                if(type == TYPE_FOUR_LED) newState = zhlavie->limitedSpeed(currentTrack, ODCHOD) ? GREEN_40 : GREEN;
                changeState(newState);
                zhlavie->reserveWay(currentTrack, ODCHOD, this);
                go = true;
            }
        }

        void posun() {
            if(safeShunt()) {
                changeState(WHITE);
                zhlavie->reserveWay(currentTrack, POSUN, this);
                go = true;
            }
        }

        void stoj() {
            if(go) {
                Navestidlo::stoj();
                zhlavie->releaseWay(currentTrack, directionMode);
                go = false;
            }
        }

        void place(Zhlavie* zhlavie, char* kolaj) {
            this->zhlavie = zhlavie;
            kolaje = (char**) malloc(sizeof(char*));
            this->kolaje[0] = kolaj;
            pocetKolaji = 1;
        }

        void place(Zhlavie* zhlavie, size_t pocetKolaji, char* kolaj, ...) {
            this->zhlavie = zhlavie;

            va_list l_Arg;
            va_start(l_Arg, kolaj);
            this->pocetKolaji = pocetKolaji;

            kolaje = (char**) malloc(pocetKolaji * sizeof(char*));


            for (size_t i = 0; i < pocetKolaji; i++)
            {
                kolaje[i] = kolaj;
                kolaj = va_arg(l_Arg, char*);
                //Serial.println(kolaj.length());
            }

            va_end(l_Arg);
        }

    private:

        bool type;
        char** kolaje;
        size_t pocetKolaji;
        Zhlavie* zhlavie;
        bool go;

        Kolaj* safeDeparture() {
            for (size_t i = 0; i < pocetKolaji; i++)
            {
                Kolaj* kolaj = zhlavie->cielovaKolaj(kolaje[i], ODCHOD);
                if(kolaj != NULL) {
                    currentTrack = kolaje[i];
                    return kolaj;
                } 
            }

            return NULL;
        }

        bool safeShunt() {
            for (size_t i = 0; i < pocetKolaji; i++)
            {
                if(zhlavie->cielovaKolaj(kolaje[i], POSUN) != NULL) return true;
            }

            return false;
        }
};

#endif