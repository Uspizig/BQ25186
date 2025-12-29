#include "BQ25186.h"
#include <Wire.h>

BQ25186::BQ25186(uint8_t i2cAddress) : _addr(i2cAddress) {}

bool BQ25186::begin() {
    Wire.begin();
    uint8_t val;
    return readRegister(REG_MASK_ID, val); // Prüft ob I2C antwortet
}

/* ================= Low-Level Registerzugriff ================= */
bool BQ25186::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

bool BQ25186::readRegister(uint8_t reg, uint8_t &value) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false; // Restart
    Wire.requestFrom(_addr, (uint8_t)1);
    if (Wire.available()) {
        value = Wire.read();
        return true;
    }
    return false;
}

bool BQ25186::writeRegisterRaw(uint8_t reg, uint8_t value) {
    return writeRegister(reg, value);
}

bool BQ25186::readRegisterRaw(uint8_t reg, uint8_t &value) {
    return readRegister(reg, value);
}

void BQ25186::dumpRegisters(Stream &out) {
    out.println(F("BQ25186 Register Dump"));
    for (uint8_t reg = REG_STAT0; reg <= REG_MASK_ID; reg++) {
        uint8_t val = 0;
        if (readRegister(reg, val)) {
            out.print(F("Reg 0x")); out.print(reg, HEX);
            out.print(F(": 0x")); out.println(val, HEX);
        }
    }
}

/* ================= High-Level API ================= */
bool BQ25186::disableCharging(bool enable) {
    uint8_t val;
    if (!readRegister(REG_ICHG_CTRL, val)) return false;
    if (enable) val |= ICHG_CTRL_CHG_DIS;
    else val &= ~ICHG_CTRL_CHG_DIS;
    return writeRegister(REG_ICHG_CTRL, val);
}

bool BQ25186::setChargeCurrent_mA(uint16_t mA) {
    uint8_t val = (((mA -40) /10) +31) & 0x7F; // 5 mA pro Schritt (Table 6‑14)
    Serial.print("Charge Current Wert eingestellt"); out.println(val);
    return writeRegister(REG_ICHG_CTRL, val);
}

bool BQ25186::setInputCurrentLimit_mA(uint16_t mA) { //Noch Falsch
    uint8_t bits;

    switch (mA)//Table 6-18
    {
        case 50:   bits = 0b000; break;
        case 100:  bits = 0b001; break;
        case 200:  bits = 0b010; break;
        case 300:  bits = 0b011; break;
        case 400:  bits = 0b100; break;
        case 500:  bits = 0b101; break;
        case 665:  bits = 0b110; break;
        case 1050: bits = 0b111; break;
        default:   bits = 0b110;  break; // Sicherer Wert
    }
    uint8_t val = (bits & 0x7); // 50 mA pro Schritt (Table 6‑21)
    Serial.print("Input current Limiter Wert eingestellt"); out.println(val);
    return writeRegister(REG_TS_CONTROL, val);
}

bool BQ25186::setBatteryVoltage_mV(uint16_t mV) {
    uint8_t val = ((mV - 3300) / 20) & 0x7F; // 3.3V..4.9V, Schritt 20 mV
    return writeRegister(REG_VBAT_CTRL, val);
}

bool BQ25186::isChargeDone() {
    uint8_t stat;
    uint8_t stat2;
    stat2 = 0;
    if (!readRegister(REG_STAT0, stat)) return false;
    //readRegister(REG_STAT0, stat);
    stat2 = stat & 0x60;
    switch (stat2)//Table 6-10
    {
        case 0: out.println("Not chargeing while chargeing function is enabled"); break;
        case 32: out.println("Constant Current Chargeing"); break;
        case 64: out.println("Constant Voltage Chargeing"); break;//BIN 10
        case 96: out.println("Chargeing Done or Chareging is disabled by host"); break;//BIN 11
        default:  Serial.print("Default:"); break;
    }
    return stat2; // STAT0.CHARGE_STAT bits 6-5
}

/* ================= Timer / Ship / SYS / IC_CTRL / TS ================= */
bool BQ25186::configureTimer(uint8_t longPressMin, uint8_t wake1Min) {
    uint8_t val = 0;
    val |= ((longPressMin & 0x03) << 6); // Long Press
    val |= ((wake1Min & 0x03) << 3);     // Auto Wake
    return writeRegister(REG_TMR_ILIM, val);
}

bool BQ25186::setShipReset(uint8_t mode, bool regReset) {
    uint8_t val = 0;
    val |= ((mode & 0x03) << 5);
    if (regReset) val |= SHIP_RST_REG_RST;
    return writeRegister(REG_SHIP_RST, val);
}

bool BQ25186::setTSControl(uint8_t tsValue) {
    return writeRegister(REG_TS_CONTROL, tsValue);
}

bool BQ25186::setICControl(uint8_t icValue) {
    return writeRegister(REG_IC_CTRL, icValue);
}

bool BQ25186::setSYSReg(uint8_t sysValue) {
    return writeRegister(REG_SYS_REG, sysValue);
}

/* ================= Status / Fault ================= */
uint8_t BQ25186::readSTAT0() {
    uint8_t val = 0; readRegister(REG_STAT0, val); return val;
}

uint8_t BQ25186::readSTAT1() {
    uint8_t val = 0; readRegister(REG_STAT1, val); return val;
}

uint8_t BQ25186::readFLAG0() {
    uint8_t val = 0; readRegister(REG_FLAG0, val); return val;
}

void BQ25186::clearFLAG0() {
    writeRegister(REG_FLAG0, 0x00); // Alle latched Flags löschen
}

void BQ25186::printSTAT0(Stream &out) {
    uint8_t val = readSTAT0();
    out.print(F("STAT0: 0x")); out.println(val, HEX);
}

void BQ25186::printSTAT1(Stream &out) {
    uint8_t val = readSTAT1();
    out.print(F("STAT1: 0x")); out.println(val, HEX);
}

void BQ25186::printFLAG0(Stream &out) {
    uint8_t val = readFLAG0();
    out.print(F("FLAG0: 0x")); out.println(val, HEX);
}

/* ================= MASK / DEVICE ID ================= */
uint8_t BQ25186::getDeviceID() {
    uint8_t val = 0;
    readRegister(REG_MASK_ID, val);
    return val & MASK_ID_DEVICE_ID_MASK;
}

uint8_t BQ25186::getMaskFault() {
    uint8_t val = 0;
    readRegister(REG_MASK_ID, val);
    return val & 0xF0;
}

void BQ25186::check_BQ25186_faults(){
  uint8_t stat0 = readSTAT0();
  uint8_t stat1 = readSTAT1();
  uint8_t flag0 = readFLAG0();

  Serial.print("STAT0: 0x"); out.println(stat0, HEX);
  Serial.print("STAT1: 0x"); out.println(stat1, HEX);
  Serial.print("FLAG0: 0x"); out.println(flag0, HEX);
  
  // Faults interpretieren FLAG0
  if (flag0 & BQ25186::FLAG0_TS_FAULT) out.println("Fault: TS open / Temperaturproblem");
  if (flag0 & BQ25186::FLAG0_ILIM_ACTIVE_FLAG) out.println("FLAG0: Fault: ILIM Fault detected");
  if (flag0 & BQ25186::FLAG0_VDPPM_ACTIVE_FLAG) out.println("Fault: VDPPM active");
  if (flag0 & BQ25186::FLAG0_VINDPM_ACTIVE_FLAG) out.println("Fault: VINDPM active");
  if (flag0 & BQ25186::FLAG0_THERMREG_ACTIVE_FLAG) out.println("Fault: Thermal regulation active");
  if (flag0 & BQ25186::FLAG0_VIN_OVP_FAULT_FLAG) out.println("Fault: VIN overvoltage");
  if (flag0 & BQ25186::FLAG0_BUVLO_FAULT_FLAG) out.println("Fault: Battery undervoltage");
  if (flag0 & BQ25186::FLAG0_BAT_OCP_FAULT) out.println("Fault: Battery overcurrent");

// Faults interpretieren STAT0
  if (stat0 & BQ25186::STAT0_TS_OPEN_STAT) out.println("Fault: TS is OPEN Active");
  //if (stat0 & BQ25186::STAT0_CHG_STAT_MASK ) out.println("Info Battery am laden");
  if (stat0 & BQ25186::STAT0_ILIM_ACTIVE_STAT) out.println("STAT 0: Info: Input current Limiter is Activated for high currents");
  if (stat0 & BQ25186::STAT0_VDPPM_ACTIVE_STAT) out.println("Info: VDPPM is Active");
  if (stat0 & BQ25186::STAT0_VINDPM_ACTIVE_STAT ) out.println("Info: VINDPM is Active");
  if (stat0 & BQ25186::STAT0_THERMREG_ACTIVE_STAT ) out.println("Info Thermal Regulation is Active");
  if (stat0 & BQ25186::STAT0_VIN_PGOOD_STAT ) out.println("Info: Power Good is OK");

  // Faults interpretieren STAT1
  if (stat1 & BQ25186::STAT1_VIN_OVP_STAT) out.println("Fault: Over Voltage Protection Active");
  if (stat1 & BQ25186::STAT1_BUVLO_STAT ) out.println("Fault: Battery Undervolt Lockout Status");
  if (stat1 & BQ25186::STAT1_TS_STAT_MASK) out.println("Fault: zu Heiss oder zu kalt");
  if (stat1 & BQ25186::STAT1_SAFETY_TMR_FLAG) out.println("Fault: Safety Timer");
  if (stat1 & BQ25186::STAT1_WAKE1_FLAG ) out.println("Fault: WAKE1 Timer Flag");
  if (stat1 & BQ25186::STAT1_WAKE2_FLAG ) out.println("Fault: WAKE2 Timer Flag");

  // Ladeende prüfen
  if (isChargeDone()) ;//out.println("Ladung abgeschlossen");
  else out.println("Ladung aktiv");
  clearFLAG0();
}
