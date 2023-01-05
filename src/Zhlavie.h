#include <Arduino.h>

#ifndef ZHLAVIE
#define ZHLAVIE

#define ROVNO true
#define ODBOCKA false

#define NORMAL 1
#define REVERSE -1
#define SLOPED 2
#define SLOPED_REVERSE -2

#define RIGHT true
#define LEFT false

#define ODCHOD 1
#define VCHOD 0
#define POSUN 2

struct Vyhybka
{
    String tag;
    String kolaj;
    int pin;
    int distanceIndex;
    int rotation;
    void *start;
    void *straight;
    void *digress;
    bool direction;
    bool state;
    bool endings[3];
};

struct Kolaj
{
    char *tag;
    bool odchodova;
    Vyhybka *vyhybkaVchod;
    Vyhybka *vyhybkaOdchod;
};

class Zhlavie
{

public:
    void kolaje(size_t pocet, char *kolaj, ...)
    {
        va_list l_Arg;
        va_start(l_Arg, kolaj);

        stanica = (Kolaj *)malloc(sizeof(Kolaj));
        pocetKolaji = pocet;

        for (size_t i = 1; i < pocet; i++)
        {
            stanica = (Kolaj*) realloc(stanica, i * sizeof(Kolaj));
            kolaj = va_arg(l_Arg, char*);
            stanica[i - 1] = {kolaj, false, NULL, NULL};
        }

        va_end(l_Arg);

        pocetVyhybiek = 0;
       // vyhybky = (Vyhybka*) malloc(50 * sizeof(Vyhybka));
    }

    void odchodovaKolaj(char* kolaj) {
        stanica[getTrackIndex(kolaj)].odchodova = true;
    }

    void vyhybka(String tag, int distanceIndex, String kolaj, int rotation, bool direction, int pin)
    {
        pinMode(pin, INPUT_PULLUP);
       
        vyhybky[pocetVyhybiek] = {tag, kolaj, pin, distanceIndex, rotation, NULL, NULL, NULL, direction, !digitalRead(pin)};


        attachTerminals(&vyhybky[pocetVyhybiek]);

        pocetVyhybiek++;
    }

    Vyhybka* find(String tag)
    {
        for (int i = 0; i < pocetVyhybiek; i++)
        {
            if (vyhybky[i].tag.equals(tag))
            {
                return &vyhybky[i];
            }
        }
        return NULL;
    }

    Kolaj* cielovaKolaj(String zKolaje, int rezim) {
        update();
        Kolaj from = stanica[getTrackIndex(zKolaje)];
        Vyhybka* vyhybka = rezim == VCHOD ? from.vyhybkaVchod : from.vyhybkaOdchod;
        void* prevAddress = &stanica[getTrackIndex(zKolaje)];

        Kolaj* kolaj = NULL;
        
        while(true) {
            if(vyhybka->start == prevAddress) {
                prevAddress = vyhybka;
                if(vyhybka->state == ROVNO ? vyhybka->endings[1] : vyhybka->endings[2]) 
                    kolaj = (Kolaj*) (vyhybka->state == ROVNO ? vyhybka->straight : vyhybka->digress);

                else vyhybka = vyhybka->state == ROVNO ? ((Vyhybka*) vyhybka->straight) : ((Vyhybka*) vyhybka->digress);
           
            } else if(vyhybka->straight == prevAddress) {
                prevAddress = vyhybka;
                if(vyhybka->state == ODBOCKA) return NULL;
                else if(vyhybka->endings[0]) kolaj = (Kolaj*) vyhybka->start;
                else vyhybka = (Vyhybka*) vyhybka->start;
            
            } else if(vyhybka->digress == prevAddress) {
                prevAddress = vyhybka;
                if(vyhybka->state == ROVNO) return NULL;
                else if(vyhybka->endings[0]) kolaj = (Kolaj*) vyhybka->start;
                else vyhybka = (Vyhybka*) vyhybka->start;
            
            } else return NULL;

            if(kolaj != NULL && (rezim == VCHOD || (rezim == ODCHOD && kolaj->odchodova) || rezim == POSUN))
                return kolaj;
        }

        return NULL;
    }

    bool limitedSpeed(String zKolaje, bool odchod) {
        update();
        Kolaj from = stanica[getTrackIndex(zKolaje)];
        Vyhybka* vyhybka = odchod ? from.vyhybkaOdchod : from.vyhybkaVchod;
        void* prevAddress = &stanica[getTrackIndex(zKolaje)];

        bool limited;
        
        while(true) {
            if(vyhybka->state == ODBOCKA) limited = true;
            if(vyhybka->start == prevAddress) {
                prevAddress = vyhybka;
                if(vyhybka->state == ROVNO ? vyhybka->endings[1] : vyhybka->endings[2]) 
                    return limited;

                else vyhybka = vyhybka->state == ROVNO ? ((Vyhybka*) vyhybka->straight) : ((Vyhybka*) vyhybka->digress);
           
            } else if(vyhybka->straight == prevAddress) {
                prevAddress = vyhybka;
                if(vyhybka->endings[0]) return limited;
                else vyhybka = (Vyhybka*) vyhybka->start;
            
            } else if(vyhybka->digress == prevAddress) {
                prevAddress = vyhybka;
                if(vyhybka->endings[0]) return limited;
                else vyhybka = (Vyhybka*) vyhybka->start;
            
            } else return false;
        }

        return false;
    }

    void printStation() {
        for (int i = 0; i < pocetVyhybiek; i++)
        {
            Vyhybka vyhybka = vyhybky[i];
            Serial.print("Vyhybka ");
            Serial.println(vyhybka.tag);
            char buffer[100];
            sprintf(buffer, "address: %p start: %p straight: %p digress: %p end: %d %d %d", 
                &vyhybky[i], vyhybka.start, vyhybka.straight, vyhybka.digress, vyhybka.endings[0], vyhybka.endings[1], vyhybka.endings[2]);
            Serial.println(buffer);
        }

        for (int i = 0; i < pocetKolaji; i++)
        {
            Kolaj kolaj = stanica[i];
            char buffer[100];
            sprintf(buffer, "name: %s vchod: %p odchod: %p", kolaj.tag, kolaj.vyhybkaVchod, kolaj.vyhybkaOdchod);
            Serial.println(buffer);
        }
    }

    void update() {
        for (int i = 0; i < pocetVyhybiek; i++)
        {
            vyhybky[i].state = !digitalRead(vyhybky[i].pin);
        }
    }

    bool getState(String tag) {
        Vyhybka* vyhybka = find(tag);
        vyhybka->state = digitalRead(vyhybka->pin);
        return vyhybka->state;
    }

    void printStationState() {
        update();
        for(int y = 3; y >= 0; y--) {
            for(int x = 3; x >= 0; x--) {
                for (int i = 0; i < pocetVyhybiek; i++)
                {   
                    if(vyhybky[i].distanceIndex == x && getTrackIndex(vyhybky[i].kolaj) == y) {
                        Serial.print(vyhybky[i].state ? "_" : "\\");
                    } else Serial.print(" ");

                }
            }
            Serial.println();
        }
    }

    void build()
    {
        for (int i = 0; i < pocetVyhybiek; i++)
        {
            Vyhybka* vyhybka = &vyhybky[i];
            int trackIndex = getTrackIndex(vyhybka->kolaj);

            if (vyhybka->start == NULL)
            {
                vyhybka->start = &stanica[trackIndex];
                if(vyhybka->rotation == REVERSE || vyhybka->rotation == SLOPED_REVERSE)
                    stanica[trackIndex].vyhybkaOdchod = vyhybka;
                else stanica[trackIndex].vyhybkaVchod = vyhybka;
                vyhybka->endings[0] = true;
            }

            if (vyhybka->straight == NULL)
            {
                if (vyhybka->rotation == SLOPED || vyhybka->rotation == SLOPED_REVERSE)
                    vyhybka->straight = digressDestinationUp(*vyhybka) ? &stanica[trackIndex + 1] : &stanica[trackIndex - 1];
                else vyhybka->straight = &stanica[trackIndex];

                if(vyhybka->rotation == NORMAL || vyhybka->rotation == SLOPED)
                    stanica[trackIndex].vyhybkaOdchod = vyhybka;
                else stanica[trackIndex].vyhybkaVchod = vyhybka;

                vyhybka->endings[1] = true;
            }

            if(vyhybka->digress == NULL) {
                if (vyhybka->rotation == SLOPED || vyhybka->rotation == SLOPED_REVERSE) {
                    vyhybka->digress = &stanica[trackIndex];
                    stanica[trackIndex].vyhybkaOdchod = vyhybka;
                }
                else
                    vyhybka->digress = vyhybka->direction == RIGHT ? &stanica[trackIndex + 1] : &stanica[trackIndex - 1];

                int movedIndex = vyhybka->direction == RIGHT ? trackIndex + 1 : trackIndex - 1;
                int newTrackIndex = (vyhybka->rotation == SLOPED || vyhybka->rotation == SLOPED_REVERSE) ? trackIndex : movedIndex;

                if(vyhybka->rotation == NORMAL || vyhybka->rotation == SLOPED)
                    stanica[newTrackIndex].vyhybkaOdchod = vyhybka;
                else stanica[newTrackIndex].vyhybkaVchod = vyhybka;

                vyhybka->endings[2] = true;
            }
        }
    }


private:

    Vyhybka vyhybky[20];
    int pocetVyhybiek;
    Kolaj *stanica;
    int pocetKolaji;

    void attachTerminals(Vyhybka* vyhybka)
    {
        for (int i = 0; i < pocetVyhybiek; i++)
        {
            Vyhybka* current = &vyhybky[i];
            
   /*         Serial.print(current->tag + " " + vyhybka->tag + " ");
            Serial.print(current->distanceIndex);
            Serial.print(" ");
            Serial.print(vyhybka->distanceIndex);
            Serial.print(" ");
            Serial.println(abs(current->distanceIndex - vyhybka->distanceIndex) == 1);*/

            if (abs(current->distanceIndex - vyhybka->distanceIndex) == 1 && vyhybka->kolaj.equals(current->kolaj))
            {
                Serial.println(vyhybka->rotation);
                if (vyhybka->rotation == NORMAL)
                    vyhybka->start = current;
                else
                    vyhybka->straight = current;

                if (current->rotation == NORMAL)
                    current->straight = vyhybka;
                else
                    current->start = vyhybka;
            }


            checkConnection(vyhybka, current);
        }
    }

    void checkConnection(Vyhybka* vyhybkaA, Vyhybka* vyhybkaB)
    {
        int i = getTrackIndex(vyhybkaA->kolaj);
        bool aUp = digressDestinationUp(*vyhybkaA);
        bool bUp = digressDestinationUp(*vyhybkaB);

        Serial.print(vyhybkaA->tag + ": ");
        Serial.println(aUp);
        Serial.print(vyhybkaB->tag + ": ");
        Serial.println(bUp);

        bool rotationOk = aUp != bUp || ((vyhybkaB->rotation == SLOPED || vyhybkaB->rotation == SLOPED_REVERSE) && aUp == bUp);
        Serial.println(rotationOk);
        String destinationTrack = aUp ? stanica[i + 1].tag : stanica[i - 1].tag;
        Serial.println(destinationTrack);

        if (rotationOk && abs(vyhybkaA->distanceIndex - vyhybkaB->distanceIndex) == 1 && vyhybkaB->kolaj.equals(destinationTrack))
        {
            if (aUp != bUp)
            {
                if (vyhybkaA->rotation == SLOPED || vyhybkaA->rotation == SLOPED_REVERSE)
                    vyhybkaA->straight = vyhybkaB;
                else
                    vyhybkaA->digress = vyhybkaB;
                
                vyhybkaB->digress = vyhybkaA;
            }

            if (aUp == bUp)
            {
                if (vyhybkaA->rotation == SLOPED || vyhybkaA->rotation == SLOPED_REVERSE)
                    vyhybkaA->straight = vyhybkaB;
                else
                    vyhybkaB->digress = vyhybkaB;
                vyhybkaB->start = vyhybkaA;
            }
        }
    }

    bool digressDestinationUp(Vyhybka vyhybka)
    {
        return (vyhybka.direction == RIGHT && vyhybka.rotation == NORMAL) ||
               (vyhybka.direction == LEFT && vyhybka.rotation == REVERSE) ||
               (vyhybka.direction == LEFT && vyhybka.rotation == SLOPED) ||
               (vyhybka.direction == RIGHT && vyhybka.rotation == SLOPED_REVERSE);
    }

    int getTrackIndex(String trackTag)
    {
        for (int i = 0; i < pocetKolaji; i++)
        {
            if (trackTag.equals(stanica[i].tag))
            {
                return i;
            }
        }

        return -1;
    }
};

#endif