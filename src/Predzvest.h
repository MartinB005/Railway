#pragma once
#include <Arduino.h>
#include <IntervalTask.h>

class Predzvest {

    public:

        Predzvest(int pinY, int pinG) {
            this->pinY = pinY;
            this->pinG = pinG;

            Serial.begin(9600);



            pinMode(pinG, OUTPUT);
            pinMode(pinY, OUTPUT);

            digitalWrite(pinY, HIGH);
        }

        void signalVolno() {
            Serial.println("volno");
            Serial.println(pinG);
            Serial.println("thisY " + String(this->pinY));
            digitalWrite(pinY, LOW);
            digitalWrite(pinG, HIGH);
            IntervalTask::noBlink(pinY);
        }

        void volno40() {
            digitalWrite(pinG, LOW);
            
            IntervalTask::blink(pinY);
        }

        void vystraha() {
            IntervalTask::noBlink(pinY);
            digitalWrite(pinY, HIGH);
            digitalWrite(pinG, LOW);
        }


    private:

        int pinG = 0;
        int pinY = 0;
        int millisBlink;
        bool blink;

};