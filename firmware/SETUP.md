# Firmware Setup Guide

## Installation

### Option 1: PlatformIO (Recommended)

1. Install [PlatformIO](https://platformio.org/install)
2. Open this folder in PlatformIO or VS Code with PlatformIO extension
3. Build and upload:
   ```bash
   pio run --target upload
   ```

### Option 2: Arduino IDE

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Add ESP32 board support:
   - Go to File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to Tools → Board → Boards Manager
   - Search for "esp32" and install "esp32 by Espressif Systems"

3. Select board:
   - Tools → Board → ESP32 Arduino → ESP32C3 Dev Module

4. Upload the sketch

**Note:** No external libraries needed - uses native ESP32 BLE libraries!

## Hardware Configuration

### ESP32-C3 Pin Assignments

- **GPIO2 (T0)**: Capacitive touch pad
- **GPIO9**: Boot button (built-in)
- **GPIO8**: Status LED (optional)

### Touch Sensor Calibration

1. Upload the firmware
2. Open Serial Monitor (115200 baud)
3. Touch the capacitive pad and observe the touch values
4. Adjust `TOUCH_THRESHOLD` in the code if needed (default: 40)

## Usage

### First Time Setup

1. Power on the device
2. Device automatically enters pairing mode
3. Connect from your MIDI software/DAW:
   - iOS: Settings → Bluetooth → Connect to "Mini MIDI Ctrl"
   - macOS: Audio MIDI Setup → MIDI Studio → Connect
   - Windows: Use a BLE MIDI bridge app

### Pairing Mode

- Hold boot button for 5+ seconds to re-enter pairing mode
- LED will blink rapidly during pairing

### MIDI Operation

- **Touch**: Sends Note On (C4, velocity 100)
- **Release**: Sends Note Off (C4)
- **Idle timeout**: Device sleeps after 30 seconds of inactivity

### LED Status Indicators

- **Slow blink**: Not connected, waiting for connection
- **Solid on**: Connected to BLE MIDI host
- **Rapid blink**: Pairing mode
- **Off**: Deep sleep

## Customization

Edit `mini_midi_ctrl.ino` to customize:

- `MIDI_NOTE`: Change the MIDI note number (default: 60 = C4)
- `MIDI_CHANNEL`: Change MIDI channel (default: 1)
- `MIDI_VELOCITY`: Change note velocity (default: 100)
- `TOUCH_THRESHOLD`: Adjust touch sensitivity (default: 40)
- `SLEEP_TIMEOUT`: Change sleep timeout in ms (default: 30000 = 30s)

## Troubleshooting

### Device not connecting
- Hold boot button for 5+ seconds to re-enter pairing mode
- Check that Bluetooth is enabled on your device
- Try power cycling the ESP32-C3

### Touch not working
- Check serial monitor for touch values
- Adjust `TOUCH_THRESHOLD` based on observed values
- Ensure capacitive pad is properly connected

### Serial Monitor shows nothing
- Check baud rate is set to 115200
- Ensure USB cable supports data transfer
- Try pressing the reset button
