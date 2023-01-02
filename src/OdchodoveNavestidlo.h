#include <Arduino.h>

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
            changeState(isSpeedLimited() ? GREEN_40 : GREEN);
        }

        void stoj() {
            changeState(RED);
        }

        void posun() {
            changeState(WHITE);
        }

    private:

        int pinA, pinB;

        bool isSpeedLimited() {
            return true;
        }

        void changeState(byte state) {
            digitalWrite(pinA, (state >> 1) & 1 ? HIGH : LOW);
            digitalWrite(pinB, (state >> 0) & 1 ? HIGH : LOW);
        }
};

#endif