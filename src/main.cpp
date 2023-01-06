#include <Arduino.h>
#include <OdchodoveNavestidlo.h>
#include <Panel.h>
#include <Zhlavie.h>

OdchodoveNavestidlo L1_3(28, 29);
OdchodoveNavestidlo L2(36, 37);
OdchodoveNavestidlo L4(38, 39);

Zhlavie zhlavie;

Panel panel;

void setup() {
 Serial.begin(9600);

     Serial.println("----");

  panel.connectSwitch("L4", A1, &L4);
  panel.connectSwitch("L1_3", A4, &L1_3);
  panel.connectSwitch("L2", A7, &L2);

  Serial.println("----");

  zhlavie.kolaje(6, "K4", "K2", "K1", "K3", "K5", "K7");

  zhlavie.odchodovaKolaj("K2");
  zhlavie.odchodovaKolaj("K1");


  zhlavie.vyhybka("V1", 0, "K2", NORMAL, RIGHT, 34);
  zhlavie.vyhybka("V2", 1, "K2", NORMAL, LEFT, 35);
  zhlavie.vyhybka("V3", 1, "K1", REVERSE, RIGHT, 33);
  zhlavie.vyhybka("V4", 2, "K1", NORMAL, RIGHT, 32);

  zhlavie.build();


  L1_3.place(&zhlavie, 2, "K1", "K3");
  L2.place(&zhlavie, "K2");
  L4.place(&zhlavie, "K4");

  Serial.println(zhlavie.cielovaKolaj("K2", ODCHOD)->tag);



}

void loop() {
  panel.check();
  delay(100);
}