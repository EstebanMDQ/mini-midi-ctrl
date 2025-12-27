# Ultra-Compact Bluetooth MIDI Controller

## Objective
Build an **ultra-small battery-powered Bluetooth MIDI controller** with **1 capacitive touch pad**, capable of sending **a single MIDI message** when touched. Designed to be attached to another instrument.

---

## Functionality
- Bluetooth LE MIDI
- 1 capacitive touch zone
- Sends:
  - Note On when touched
  - Note Off when released
- Battery powered
- Low power consumption (deep sleep)
- No display
- No mechanical buttons

---

## Technical Specifications
- Target PCB size: ≤ 25 × 25 mm
- Total height: ≤ 8 mm
- Communication: BLE MIDI (standard)
- Expected battery life: days/weeks depending on usage

---

## BOM (Bill of Materials)

### Microcontroller
- **ESP32-C3-MINI-1**
  - Manufacturer: Espressif
  - BLE + low power consumption
  - Integrated antenna
  - Quantity: 1

---

### Power Supply
- **LiPo Battery 3.7V 150–300 mAh** (flat format)
  - Quantity: 1
- **LiPo Charger IC MCP73831**
  - Quantity: 1
- **USB-C Connector** (power only)
  - Quantity: 1
- **Current programming resistor** (10kΩ → ~100 mA)
  - Quantity: 1
- **Ceramic capacitor 4.7 µF** (charger input)
  - Quantity: 1
- **Ceramic capacitor 4.7 µF** (charger output)
  - Quantity: 1

---

### Regulation / Power
- **LDO 3.3V** (MCP1700 / XC6206 / equivalent)
  - Quantity: 1
- **Capacitor 1 µF** (LDO input)
  - Quantity: 1
- **Capacitor 1 µF** (LDO output)
  - Quantity: 1

---

### Capacitive Touch
- **Capacitive pad** (copper on PCB, ~10–15 mm diameter)
  - Quantity: 1
- **Resistor 1 MΩ** (optional)
  - Quantity: 1

---

### Indicators (Optional)
- **SMD LED 0603**
  - Quantity: 1
- **LED resistor 1 kΩ**
  - Quantity: 1

---

### Miscellaneous Passives
- **Decoupling capacitors 100 nF**
  - Quantity: 2–3

---

## Firmware Behavior
- Deep sleep by default
- Wake on touch
- Sends MIDI Note On / Off
- Returns to sleep

---

## Default MIDI Message
- Channel: 1
- Note: C4 (60)
- Velocity: 100 (fixed)

---

## Future Versions
- Multiple pads
- Continuous CC
- Configuration via BLE
- 3D printed enclosure

---

**Status: MVP ready for prototyping**

