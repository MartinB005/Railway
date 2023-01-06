#include <Arduino.h>
#include <Zhlavie.h>

#ifndef ODCHODOVE
#define ODCHODOVE

#define GREEN 0b11
#define GREEN_40 0b10
#define RED 0b00
#define WHITE 0b01


class OdchodoveNavestidlo {
    
    public:

        OdchodoveNavestidlo(int pinA, int pinB) {
            pinMode(pinA, OUTPUT);
            pinMode(pinB, OUTPUT);
           
            this->pinA = pinA;
            this->pinB = pinB;
        }

        void volno() {
            Kolaj* kolaj = safeDeparture();
            if(kolaj != NULL) {
                changeState(zhlavie->limitedSpeed(kolaj->tag, ODCHOD) ? GREEN_40 : GREEN);
            }
        }

        void stoj() {
            changeState(RED);
        }

        void posun() {
            if(safeShunt()) changeState(WHITE);
        }

        void place(Zhlavie* zhlavie, String kolaj) {
            this->zhlavie = zhlavie;
            this->kolaje[0] = kolaj;
            pocetKolaji = 1;
        }

        void place(Zhlavie* zhlavie, size_t pocetKolaji, String kolaj, ...) {
            this->zhlavie = zhlavie;

            va_list l_Arg;
            va_start(l_Arg, kolaj);
            this->pocetKolaji = pocetKolaji;

            for (size_t i = 1; i < pocetKolaji; i++)
            {
                kolaje[i] = kolaj;
                kolaj = va_arg(l_Arg, char*);
                Serial.println(kolaj.length());
                Serial.println(kolaj != NULL);
            }

            va_end(l_Arg);
        }

    private:

        int pinA, pinB;
        Zhlavie* zhlavie;
        String kolaje[5];
        size_t pocetKolaji;

        void changeState(byte state) {
            digitalWrite(pinA, (state >> 1) & 1 ? HIGH : LOW);
            digitalWrite(pinB, (state >> 0) & 1 ? HIGH : LOW);
        }

        Kolaj* safeDeparture() {
            for (size_t i = 0; i < pocetKolaji; i++)
            {
                Serial.println(kolaje[i]);
                Kolaj* kolaj = zhlavie->cielovaKolaj(kolaje[i], ODCHOD);
                Serial.println(kolaj->tag);
                if(kolaj != NULL) return kolaj;
            }

            return NULL;
        }

        bool safeShunt() {
            for (size_t i = 1; i < pocetKolaji; i++)
            {
                if(zhlavie->cielovaKolaj(kolaje[i], POSUN) != NULL) return true;
            }

            return false;
        }
};

#endif