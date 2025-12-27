# Firmware

ESP32-C3 firmware for the mini MIDI controller.

## Structure

- `src/mini_midi_ctrl.ino` - Main Arduino sketch
- `include/` - Header files (as needed)
- `platformio.ini` - PlatformIO configuration
- `SETUP.md` - Detailed setup and usage instructions

## Features

✅ BLE MIDI communication
✅ Capacitive touch sensing on GPIO2 (T0)
✅ Deep sleep power management
✅ Note On/Off message handling
✅ Boot button pairing mode (5 second hold)
✅ Auto-pairing on first boot
✅ LED status indicators

## Quick Start

See `SETUP.md` for detailed installation and usage instructions.

**PlatformIO:**
```bash
pio run --target upload
```

**Arduino IDE:**
1. Install ESP32 board support
2. Select ESP32C3 Dev Module
3. Upload sketch

**No external libraries required** - uses native ESP32 BLE!
