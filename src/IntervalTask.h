#pragma once
#include <Arduino.h>

#define BLINK_MILLIS 555

class IntervalTask
{

public:
    static int *leds;
    static int ledCount;
    static bool state;

    static void init()
    {

        TCCR0A = (1 << WGM01); // Set the CTC mode
        OCR0A = 0xF9;          // Value for ORC0A for 1ms

        TIMSK0 |= (1 << OCIE0A); // Set   the interrupt request
        sei();                   // Enable interrupt

        TCCR0B |= (1 << CS01); // Set the prescale 1/64 clock
        TCCR0B |= (1 << CS00);
    }

    static void blink(int pin)
    {
        int index = getIndex(pin);
        if (index == -1)
        {
            if (leds == NULL)
                leds = (int *)malloc(sizeof(int));
            else
                leds = (int *)realloc(leds, ledCount * sizeof(int));

            leds[ledCount] = pin;
            ledCount++;
        }
    }

    static void noBlink(int pin)
    {
        int index = getIndex(pin);

        if (index != -1)
        {
            int *temp = (int *)malloc(sizeof(leds) - sizeof(int));

            if (index != 0)
                memcpy(temp, leds, index * sizeof(int)); // copy everything BEFORE the index

            if (index != (ledCount - 1))
                memcpy(temp + index, leds + index + 1, (ledCount - index - 1) * sizeof(int)); // copy everything AFTER the index

            free(leds);
            leds = temp;
            ledCount--;
        }
    }

    static void onBlink()
    {
        state = !state;
        int value = state ? HIGH : LOW;

        Serial.println(ledCount);
        for (int i = 0; i < ledCount; i++)
        {
            digitalWrite(leds[i], value);
        }
    }

private:
    static int getIndex(int pin)
    {
        for (int i = 0; i < ledCount; i++)
        {
            if (leds[i] == pin)
                return i;
        }

        return -1;
    }
};

int *IntervalTask::leds = NULL;
int IntervalTask::ledCount = 0;
bool IntervalTask::state = false;
int timer = 0;

ISR(TIMER0_COMPA_vect)
{
    if (timer >= BLINK_MILLIS)
    {
        IntervalTask::onBlink();
        timer = 0;
    }

    timer++;
}