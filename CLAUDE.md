# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an **ultra-compact Bluetooth MIDI controller** hardware project. The device is a minimal battery-powered BLE MIDI controller with a single capacitive touch pad that sends MIDI Note On/Off messages when touched/released.

**Hardware Target:**
- MCU: ESP32-C3-MINI-1 (BLE capable)
- Size: ≤ 25 × 25 mm PCB, ≤ 8 mm total height
- Power: 150-300 mAh LiPo battery with USB-C charging (MCP73831)
- Input: Single capacitive touch pad (copper on PCB)
- No display, no mechanical buttons

**MIDI Specification:**
- Channel: 1
- Note: C4 (60)
- Velocity: 100 (fixed)
- Sends Note On when touched, Note Off when released

## Development Environment

**Platform:** ESP32-C3 development requires one of:
- Arduino IDE with ESP32 board support
- PlatformIO
- ESP-IDF (Espressif IoT Development Framework)

**Key Libraries Needed:**
- BLE MIDI library for ESP32
- Capacitive touch sensing (ESP32 has built-in touch sensing)
- Deep sleep/power management

## Firmware Architecture

The firmware (when implemented) should follow this behavior pattern:

1. **Deep Sleep by Default** - Device sleeps to conserve battery
2. **Wake on Touch** - Capacitive touch triggers wake from sleep
3. **BLE Connection** - Establish/maintain BLE MIDI connection
4. **Send MIDI** - Transmit Note On (touch) / Note Off (release)
5. **Return to Sleep** - Enter deep sleep after activity timeout

**Power Management is Critical:**
- Use ESP32-C3 deep sleep modes
- Wake on touch pad interrupt
- Minimize active time to maximize battery life (target: days/weeks)

## Hardware Files

- `schematic.svg` - Hardware schematic diagram
- README.md - Complete hardware specification, BOM, and project goals

## Development Commands

### PlatformIO (Recommended)

```bash
# Build firmware
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor

# Build + Upload + Monitor
pio run --target upload && pio device monitor

# Clean build
pio run --target clean
```

### Arduino IDE

- Select board: Tools → Board → ESP32 Arduino → ESP32C3 Dev Module
- Upload: Sketch → Upload
- Serial Monitor: Tools → Serial Monitor (115200 baud)

### Testing

```bash
# Monitor serial output for debugging
pio device monitor

# Watch for MIDI messages and touch values
# Connect via BLE MIDI from DAW/MIDI software
```

## Future Enhancements

Documented in README.md:
- Multiple capacitive pads
- Continuous CC (Control Change) messages
- Configuration via BLE
- 3D printed enclosure
