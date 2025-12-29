# BQ25186 Arduino Library

**Datasheet-compliant Arduino library for the Texas Instruments BQ25186 ultra-low-power battery charger**

---

## 📌 Overview

This Arduino library provides full configuration, monitoring, and diagnostics for the **Texas Instruments BQ25186** linear battery charger IC via I²C.

The library is designed with a strong focus on:

- **Strict datasheet compliance**
- **Complete register and bitfield coverage**
- **Clear separation between low-level register access and high-level APIs**
- **Robust status and fault handling**

All registers and bitfields are implemented **exclusively according to the official TI datasheet**, using register tables **6-10 through 6-22**.  
No code or definitions from the **BQ25180** or other TI charger devices are used.

---

## ✅ Features

- Complete mapping of **all I²C registers** (Table 6-8)
- **All bitfields** from Tables **6-10 through 6-22** defined  
  (including unused or optional fields)
- Configurable:
  - Charge current (mA)
  - Battery regulation voltage (mV)
  - Safety timer and wake timers
  - TS / thermistor behavior
  - SYS regulation settings
  - IC control options
  - Ship / reset modes
- Charge state detection:
  - Charging active
  - Charge complete
- Status and fault monitoring:
  - STAT0 / STAT1
  - FLAG0 (latched faults)
- Human-readable fault decoding via `Serial`
- Full register dump for debugging
- Low-level raw register access (`readRegisterRaw`, `writeRegisterRaw`)
- Uses the standard Arduino `Wire` I²C interface

---

## 🧩 Library Architecture

The library is intentionally split into two layers:

### 1️⃣ Low-Level Layer
- Direct register read/write access
- Complete bitmask and field definitions
- No logic or interpretation

### 2️⃣ High-Level API
- Easy-to-use configuration functions such as:
  - `setBatteryVoltage_mV()`
  - `setChargeCurrent_mA()`
  - `configureTimer()`
  - `setTSControl()`
  - `enableCharging()`
- Status and fault interpretation with readable output

This makes the library suitable for both:
- **Rapid prototyping**
- **Production-grade firmware development**

---

## 🔌 Supported Hardware

- Texas Instruments **BQ25186**
- Arduino-compatible boards with I²C (`Wire`)
- Tested with both 3.3 V and 5 V MCUs  
  *(observe I²C level compatibility)*

---

## 📂 Repository Structure

```
BQ25186/
├── src/
│   ├── BQ25186.h
│   └── BQ25186.cpp
├── examples/
│   └── BQ25186_demo/
│       └── BQ25186_demo.ino
├── library.properties
└── README.md
```

---

## 🚀 Example Usage

```cpp
#include "BQ25186.h"

BQ25186 charger;

void setup() {
  charger.begin();
  charger.setBatteryVoltage_mV(4200);
  charger.setChargeCurrent_mA(500);
  charger.enableCharging(true);
}
```

---

## ⚠️ Notes

- Always verify electrical limits (VIN, battery voltage, charge current) against the **TI BQ25186 datasheet**
- This library does **not** enforce safety limits automatically
- The user is responsible for proper configuration according to the battery and system design

---

## 📜 License

This project is released under the **MIT License**.

---

## 🙌 Contributions

Issues, bug reports, and pull requests are welcome.  
Please ensure that all changes remain **datasheet-accurate** and clearly documented.
