#include <Arduino.h>
#include <Zhlavie.h>
#include <OdchodoveNavestidlo.h>
#include <VchodoveNavestidlo.h>
#include <Panel.h>
#include <IntervalTask.h>

OdchodoveNavestidlo L1_3(28, 29, TYPE_FOUR_LED);
OdchodoveNavestidlo L2(36, 37, TYPE_THREE_LED);
OdchodoveNavestidlo L4(38, 39, TYPE_FOUR_LED);

VchodoveNavestidlo S(24, 25, 26);
Predzvest prS(22, 23);

Panel panel;

Zhlavie zhlavie;

void setup() {
  Serial.begin(9600);

  panel.connectSwitch("L4", A1, &L4);
  panel.connectSwitch("L1_3", A4, &L1_3);
  panel.connectSwitch("L2", A7, &L2);

  panel.connectSwitch("S", A3, &S);


  zhlavie.kolaje(6, "K4", "K2", "K1", "K3", "K5", "K7");

  zhlavie.odchodovaKolaj("K2");
  zhlavie.odchodovaKolaj("K1");


  zhlavie.vyhybka("V1", 0, "K2", NORMAL, RIGHT, 34);
  zhlavie.vyhybka("V2", 1, "K2", NORMAL, LEFT, 35);
  zhlavie.vyhybka("V3", 1, "K1", REVERSE, RIGHT, 33);
  zhlavie.vyhybka("V4", 2, "K1", NORMAL, RIGHT, 32);

  zhlavie.build();

  Serial.print(" test: ");
  Serial.println(zhlavie.cielovaKolaj("K2", ODCHOD)->tag);


  L1_3.place(&zhlavie, 2, "K1", "K3");
  L2.place(&zhlavie, "K2");
  L4.place(&zhlavie, "K4");

  S.place(&zhlavie, "K1");

  S.predzvest(&prS);

  IntervalTask::init();

}

void loop() {
  panel.check();
  zhlavie.checkSwitches();
  delay(100);

}

