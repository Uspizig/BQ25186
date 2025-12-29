#ifndef BQ25186_H
#define BQ25186_H

#include <Arduino.h>
#include <Wire.h>

/*
 * BQ25186 Arduino Library
 * Register and bitfield definitions from Tables 6-10 … 6-22
 * All bits are defined — even if not currently used by high-level API.
 * Source: TI BQ25186 Datasheet Rev. A (Jan 2025) :contentReference[oaicite:1]{index=1}
 */

class BQ25186 {
public:
    explicit BQ25186(uint8_t i2cAddress = 0x6A);

    bool begin();

    /* ================= High-Level API ================= */
    
    bool disableCharging(bool enable);
    bool setChargeCurrent_mA(uint16_t mA);
    bool setBatteryVoltage_mV(uint16_t mV);
    bool setInputCurrentLimit_mA(uint16_t mA);
    
    bool isChargeDone();

    /* Low‑level register access */
    bool writeRegisterRaw(uint8_t reg, uint8_t value);
    bool readRegisterRaw(uint8_t reg, uint8_t &value);
    void dumpRegisters(Stream &out);

    /* === Timer / Ship / SYS / IC_CTRL / TS === */
    bool configureTimer(uint8_t longPressMin, uint8_t wake1Min);
    bool setShipReset(uint8_t mode, bool regReset);
    bool setTSControl(uint8_t tsValue);
    bool setICControl(uint8_t icValue);
    bool setSYSReg(uint8_t sysValue);
    

    /* === Status / Fault === */
    uint8_t readSTAT0();
    uint8_t readSTAT1();
    uint8_t readFLAG0();
    void clearFLAG0();
    void printSTAT0(Stream &out);
    void printSTAT1(Stream &out);
    void printFLAG0(Stream &out);

    /* === MASK / DEVICE ID === */
    uint8_t getDeviceID();
    uint8_t getMaskFault();

    /* ===== I2C Register Map (Table 6‑8) ===== */
    enum Register : uint8_t {
        REG_STAT0      = 0x00, // STAT0 (Table 6‑10) :contentReference[oaicite:2]{index=2}
        REG_STAT1      = 0x01, // STAT1 (Table 6‑11) :contentReference[oaicite:3]{index=3}
        REG_FLAG0      = 0x02, // FLAG0 (Table 6‑12) :contentReference[oaicite:4]{index=4}
        REG_VBAT_CTRL  = 0x03, // VBAT_CTRL (Table 6‑13) :contentReference[oaicite:5]{index=5}
        REG_ICHG_CTRL  = 0x04, // ICHG_CTRL (Table 6‑14) :contentReference[oaicite:6]{index=6}
        REG_CHG_CTRL0  = 0x05, // CHARGECTRL0 (Table 6‑15) :contentReference[oaicite:7]{index=7}
        REG_CHG_CTRL1  = 0x06, // CHARGECTRL1 (Table 6‑16) :contentReference[oaicite:8]{index=8}
        REG_IC_CTRL    = 0x07, // IC_CTRL (Table 6‑17) :contentReference[oaicite:9]{index=9}
        REG_TMR_ILIM   = 0x08, // TMR_ILIM (Table 6‑18) :contentReference[oaicite:10]{index=10}
        REG_SHIP_RST   = 0x09, // SHIP_RST (Table 6‑19) :contentReference[oaicite:11]{index=11}
        REG_SYS_REG    = 0x0A, // SYS_REG (Table 6‑20) :contentReference[oaicite:12]{index=12}
        REG_TS_CONTROL = 0x0B, // TS_CONTROL (Table 6‑21) :contentReference[oaicite:13]{index=13}
        REG_MASK_ID    = 0x0C  // MASK_ID (Table 6‑22) :contentReference[oaicite:14]{index=14}
    };

    /* ===== STAT0 Register (Table 6‑10) ===== */
    static constexpr uint8_t STAT0_TS_OPEN_STAT       = (1 << 7);
    static constexpr uint8_t STAT0_CHG_STAT_MASK      = (3 << 5);
    static constexpr uint8_t STAT0_ILIM_ACTIVE_STAT   = (1 << 4);
    static constexpr uint8_t STAT0_VDPPM_ACTIVE_STAT  = (1 << 3);
    static constexpr uint8_t STAT0_VINDPM_ACTIVE_STAT = (1 << 2);
    static constexpr uint8_t STAT0_THERMREG_ACTIVE_STAT = (1 << 1);
    static constexpr uint8_t STAT0_VIN_PGOOD_STAT     = (1 << 0);

    /* ===== STAT1 Register (Table 6‑11) ===== */
    static constexpr uint8_t STAT1_VIN_OVP_STAT       = (1 << 7);
    static constexpr uint8_t STAT1_BUVLO_STAT         = (1 << 6);
    static constexpr uint8_t STAT1_TS_STAT_MASK       = (3 << 4); // bits 5:4
    static constexpr uint8_t STAT1_SAFETY_TMR_FLAG    = (1 << 3);
    static constexpr uint8_t STAT1_WAKE1_FLAG         = (1 << 2);
    static constexpr uint8_t STAT1_WAKE2_FLAG         = (1 << 1);

    /* ===== FLAG0 Register (Table 6‑12) ===== */
    static constexpr uint8_t FLAG0_TS_FAULT           = (1 << 7);
    static constexpr uint8_t FLAG0_ILIM_ACTIVE_FLAG   = (1 << 6);
    static constexpr uint8_t FLAG0_VDPPM_ACTIVE_FLAG  = (1 << 5);
    static constexpr uint8_t FLAG0_VINDPM_ACTIVE_FLAG = (1 << 4);
    static constexpr uint8_t FLAG0_THERMREG_ACTIVE_FLAG = (1 << 3);
    static constexpr uint8_t FLAG0_VIN_OVP_FAULT_FLAG = (1 << 2);
    static constexpr uint8_t FLAG0_BUVLO_FAULT_FLAG   = (1 << 1);
    static constexpr uint8_t FLAG0_BAT_OCP_FAULT      = (1 << 0);

    /* ===== VBAT_CTRL Register (Table 6‑13) ===== */
    static constexpr uint8_t VBAT_CTRL_PG_MODE        = (1 << 7);
    static constexpr uint8_t VBAT_CTRL_VBATREG_MASK   = 0x7F;

    /* ===== ICHG_CTRL Register (Table 6‑14) ===== */
    static constexpr uint8_t ICHG_CTRL_CHG_DIS        = (1 << 7);
    static constexpr uint8_t ICHG_CTRL_ICHG_MASK      = 0x7F;

    /* ===== CHARGECTRL0 Register (Table 6‑15) ===== */
    static constexpr uint8_t CHG0_EN_FC_MODE          = (1 << 7);
    static constexpr uint8_t CHG0_IPRECHG             = (1 << 6);
    static constexpr uint8_t CHG0_ITERM_MASK          = (3 << 4);
    static constexpr uint8_t CHG0_VINDPM_SEL_MASK     = (3 << 2);
    static constexpr uint8_t CHG0_THERM_REG_MASK      = (3 << 0);

    /* ===== CHARGECTRL1 Register (Table 6‑16) ===== */
    static constexpr uint8_t CHG1_IBAT_OCP_MASK       = (3 << 6);
    static constexpr uint8_t CHG1_BUVLO_MASK          = (7 << 3);
    static constexpr uint8_t CHG1_CHG_STATUS_INT_MASK = (1 << 2);
    static constexpr uint8_t CHG1_ILIM_INT_MASK       = (1 << 1);
    static constexpr uint8_t CHG1_VINDPM_INT_MASK     = (1 << 0);

    /* ===== IC_CTRL Register (Table 6‑17) ===== */
    static constexpr uint8_t IC_CTRL_TS_EN            = (1 << 7);
    static constexpr uint8_t IC_CTRL_VLOWV_SEL        = (1 << 6);
    static constexpr uint8_t IC_CTRL_VRCH0            = (1 << 5);
    static constexpr uint8_t IC_CTRL_2XTMR_EN         = (1 << 4);
    static constexpr uint8_t IC_CTRL_SAFETY_TIMER_MASK= (3 << 2);
    static constexpr uint8_t IC_CTRL_WATCHDOG_SEL_MASK= (3 << 0);

    /* ===== TMR_ILIM Register (Table 6‑18) ===== */
    static constexpr uint8_t TMR_ILIM_MR_LPRESS_MASK  = (3 << 6);
    static constexpr uint8_t TMR_ILIM_RESET_VIN       = (1 << 5);
    static constexpr uint8_t TMR_ILIM_AUTOWAKE_MASK   = (3 << 3);
    static constexpr uint8_t TMR_ILIM_ILIM_MASK       = (7 << 0);

    /* ===== SHIP_RST Register (Table 6‑19) ===== */
    static constexpr uint8_t SHIP_RST_REG_RST         = (1 << 7);
    static constexpr uint8_t SHIP_RST_EN_RST_SHIP_MASK= (3 << 5);
    static constexpr uint8_t SHIP_RST_PB_LPRESS_MASK  = (3 << 3);
    static constexpr uint8_t SHIP_RST_WAKE1_TMR       = (1 << 2);
    static constexpr uint8_t SHIP_RST_WAKE2_TMR       = (1 << 1);
    static constexpr uint8_t SHIP_RST_EN_PUSH         = (1 << 0);

    /* ===== SYS_REG Register (Table 6‑20) ===== */
    static constexpr uint8_t SYS_REG_SYSV_MASK        = (7 << 4);
    static constexpr uint8_t SYS_REG_MODE_MASK        = (3 << 2);

    /* ===== TS_CONTROL Register (Table 6‑21) ===== */
    static constexpr uint8_t TS_CTRL_TS_HOT_MASK      = (3 << 6);
    static constexpr uint8_t TS_CTRL_TS_COLD_MASK     = (3 << 4);
    static constexpr uint8_t TS_CTRL_TS_WARM          = (1 << 3);
    static constexpr uint8_t TS_CTRL_TS_COOL          = (1 << 2);
    static constexpr uint8_t TS_CTRL_TS_ICHG          = (1 << 1);
    static constexpr uint8_t TS_CTRL_TS_VRCG          = (1 << 0);

    /* ===== MASK_ID Register (Table 6‑22) ===== */
    static constexpr uint8_t MASK_ID_TS_INT_MASK      = (1 << 7);
    static constexpr uint8_t MASK_ID_TREG_INT_MASK    = (1 << 6);
    static constexpr uint8_t MASK_ID_BAT_INT_MASK     = (1 << 5);
    static constexpr uint8_t MASK_ID_PGOOD_INT_MASK   = (1 << 4);
    static constexpr uint8_t MASK_ID_DEVICE_ID_MASK   = 0x0F;

private:
    uint8_t _addr;
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t &value);
};

#endif // BQ25186_H
