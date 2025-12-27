#include <Wire.h>
#include "BQ25186.h"

BQ25186 charger;  // Standard I2C-Adresse 0x6A
#define SDA2 17
#define SCL2 8
void setup() {
  Wire.begin(SDA2, SCL2);
  delay(3000);
  Serial.begin(115200);
  
  while (!Serial);
  delay(2000);

  Serial.println("BQ25186 Demo Start");

  if (!charger.begin()) {
    Serial.println("Fehler: BQ25186 nicht erreichbar!");
    while (1);
  }
  else{
    Serial.print("Device ID:"); Serial.println(charger.getDeviceID());
  }

  Serial.println("BQ25186 initialisiert");

  // Batterie-Spannung auf 4.2V setzen
  charger.setBatteryVoltage_mV(4000);

  // Ladestrom auf 500 mA
  charger.setChargeCurrent_mA(250);

  // Eingangsstrom auf 655 mA
  charger.setInputCurrentLimit_mA(1050);

  // Laden aktivieren
  charger.disableCharging(false);//True verhindert laden

  // Timer konfigurieren: longPress=2, wake1=1
  //charger.configureTimer(2, 1);

  // Ship/Reset konfigurieren
  charger.setShipReset(0x01, false);

  // TS_CONTROL Beispielwert
  charger.setTSControl(BQ25186::TS_CTRL_TS_HOT_MASK);

  // IC_CTRL Beispiel
  charger.setICControl(0xAB); //AA TS AUTO Enabled; 40s HW Reset, AB Disable Watchdof Reset

  // SYS_REG Beispiel
  charger.setSYSReg(0x50);
  //0x54 4.5V Sys Voltage, PG_GPO is low, Power from VBAT , I2C Watchdog disabled, Enable DPPM
  //0x50 4.5V Sys Voltage, PG_GPO is low, Power from VBAT and VSYS , I2C Watchdog disabled, Enable DPPM

  Serial.println("Initialisierung abgeschlossen");
}

void loop() {
  // Register-Dump
  Serial.println("\n=== Register Dump ===");
  charger.dumpRegisters(Serial);

  // Status lesen
  uint8_t stat0 = charger.readSTAT0();
  uint8_t stat1 = charger.readSTAT1();
  uint8_t flag0 = charger.readFLAG0();

  Serial.print("STAT0: 0x"); Serial.println(stat0, HEX);
  Serial.print("STAT1: 0x"); Serial.println(stat1, HEX);
  Serial.print("FLAG0: 0x"); Serial.println(flag0, HEX);

  // Faults interpretieren FLAG0
  if (flag0 & BQ25186::FLAG0_TS_FAULT) Serial.println("Fault: TS open / Temperaturproblem");
  if (flag0 & BQ25186::FLAG0_ILIM_ACTIVE_FLAG) Serial.println("FLAG0: Fault: ILIM Fault detected");
  if (flag0 & BQ25186::FLAG0_VDPPM_ACTIVE_FLAG) Serial.println("Fault: VDPPM active");
  if (flag0 & BQ25186::FLAG0_VINDPM_ACTIVE_FLAG) Serial.println("Fault: VINDPM active");
  if (flag0 & BQ25186::FLAG0_THERMREG_ACTIVE_FLAG) Serial.println("Fault: Thermal regulation active");
  if (flag0 & BQ25186::FLAG0_VIN_OVP_FAULT_FLAG) Serial.println("Fault: VIN overvoltage");
  if (flag0 & BQ25186::FLAG0_BUVLO_FAULT_FLAG) Serial.println("Fault: Battery undervoltage");
  if (flag0 & BQ25186::FLAG0_BAT_OCP_FAULT) Serial.println("Fault: Battery overcurrent");

// Faults interpretieren STAT0
  if (stat0 & BQ25186::STAT0_TS_OPEN_STAT) Serial.println("Fault: TS is OPEN Active");
  //if (stat0 & BQ25186::STAT0_CHG_STAT_MASK ) Serial.println("Info Battery am laden");
  if (stat0 & BQ25186::STAT0_ILIM_ACTIVE_STAT) Serial.println("STAT 0: Info: Input current Limiter is Activated for high currents");
  if (stat0 & BQ25186::STAT0_VDPPM_ACTIVE_STAT) Serial.println("Info: VDPPM is Active");
  if (stat0 & BQ25186::STAT0_VINDPM_ACTIVE_STAT ) Serial.println("Info: VINDPM is Active");
  if (stat0 & BQ25186::STAT0_THERMREG_ACTIVE_STAT ) Serial.println("Info Thermal Regulation is Active");
  if (stat0 & BQ25186::STAT0_VIN_PGOOD_STAT ) Serial.println("Info: Power Good is OK");

  // Faults interpretieren STAT1
  if (stat1 & BQ25186::STAT1_VIN_OVP_STAT) Serial.println("Fault: Over Voltage Protection Active");
  if (stat1 & BQ25186::STAT1_BUVLO_STAT ) Serial.println("Fault: Battery Undervolt Lockout Status");
  if (stat1 & BQ25186::STAT1_TS_STAT_MASK) Serial.println("Fault: zu Heiss oder zu kalt");
  if (stat1 & BQ25186::STAT1_SAFETY_TMR_FLAG) Serial.println("Fault: Safety Timer");
  if (stat1 & BQ25186::STAT1_WAKE1_FLAG ) Serial.println("Fault: WAKE1 Timer Flag");
  if (stat1 & BQ25186::STAT1_WAKE2_FLAG ) Serial.println("Fault: WAKE2 Timer Flag");

  // Ladeende prüfen
  if (charger.isChargeDone()) ;//Serial.println("Ladung abgeschlossen");
  else Serial.println("Ladung aktiv");
  
  // Ladeende prüfen
  

  // Latched Flags zurücksetzen
  charger.clearFLAG0();

  delay(5000); // alle 5 Sekunden prüfen
}
