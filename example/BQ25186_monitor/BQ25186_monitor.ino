#include "Arduino.h"
#include <Wire.h>
#include <BQ25186.h>

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
    Serial.println("Fehler: BQ25186 not reachable!");
    while (1);
  }
  else{
    Serial.print("Device ID:"); Serial.println(charger.getDeviceID());
  }

  Serial.println("BQ25186 initialisiert");

  // Batterie-Spannung auf 4.2V setzen
  charger.setBatteryVoltage_mV(4200);

  // Ladestrom auf 500 mA
  charger.setChargeCurrent_mA(200);

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
  //Serial.println("\n=== Register Dump ===");
  //charger.dumpRegisters(Serial);
  charger.check_BQ25186_faults();  

  // Latched Flags zurücksetzen
  

  delay(5000); // alle 5 Sekunden prüfen
}
