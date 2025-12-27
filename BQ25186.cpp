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
    Serial.print("Charge Current Wert eingestellt"); Serial.println(val);
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
    Serial.print("Input current Limiter Wert eingestellt"); Serial.println(val);
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
        case 0: Serial.println("Not chargeing while chargeing function is enabled"); break;
        case 32: Serial.println("Constant Current Chargeing"); break;
        case 64: Serial.println("Constant Voltage Chargeing"); break;//BIN 10
        case 96: Serial.println("Chargeing Done or Chareging is disabled by host"); break;//BIN 11
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
