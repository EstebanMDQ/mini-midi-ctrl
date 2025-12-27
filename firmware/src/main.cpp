/**
 * Mini MIDI Controller - ESP32-C3 Bluetooth MIDI
 *
 * Features:
 * - BLE MIDI communication (native ESP32 BLE, no external libraries)
 * - Capacitive touch sensing
 * - Deep sleep power management
 * - Boot button pairing mode (hold 5+ seconds)
 */

#include <Arduino.h>
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include "esp_sleep.h"

// BLE MIDI Service UUIDs (standardized)
#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

// Configuration
#define DEVICE_NAME "Mini MIDI Ctrl"
#define BUTTON_PIN 2            // GPIO2 for trigger button
#define BOOT_BUTTON 9           // GPIO9 is the boot button on ESP32-C3
#define LED_PIN 8               // GPIO8 for status LED (if available)
#define DEBOUNCE_DELAY 50       // Debounce time in milliseconds
#define BOOT_HOLD_TIME 5000     // 5 seconds to enter pairing mode
#define SLEEP_TIMEOUT 30000     // 30 seconds of inactivity before sleep

// MIDI Configuration
#define MIDI_CHANNEL 1          // MIDI channel (1-16)
#define CC_NUMBER 102           // CC number for Loopy Pro clip 1 overdub (adjust as needed)
#define CC_VALUE_ON 127         // CC value when button pressed
#define CC_VALUE_OFF 0          // CC value when button released

// BLE Objects
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// State variables
bool buttonActive = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long lastActivityTime = 0;
unsigned long bootPressStartTime = 0;
bool bootButtonPressed = false;

// BLE MIDI packet header
uint8_t midiPacket[5];

// Forward declarations
void initBLE();
void checkBootButton();
void checkButton();
void sendNoteOn();
void sendNoteOff();
void sendMIDI(uint8_t type, uint8_t channel, uint8_t note, uint8_t velocity);
void enterPairingMode();
void enterDeepSleep();
void updateStatusLED();

// BLE Server Callbacks
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("BLE MIDI connected!");
      lastActivityTime = millis();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("BLE MIDI disconnected");
      lastActivityTime = millis();

      // Restart advertising
      BLEDevice::startAdvertising();
      Serial.println("Restarting advertising...");
    }
};

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for serial to initialize
  Serial.println("Mini MIDI Controller Starting...");

  // Configure pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Button with internal pull-up
  pinMode(BOOT_BUTTON, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize BLE
  initBLE();

  // Start in pairing mode
  enterPairingMode();

  lastActivityTime = millis();
}

void loop() {
  // Check boot button for pairing mode
  checkBootButton();

  // Check trigger button
  checkButton();

  // Handle BLE connection changes
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // Give the bluetooth stack time to get ready
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }

  // Check for sleep timeout (only if not connected)
  if (!deviceConnected && (millis() - lastActivityTime > SLEEP_TIMEOUT)) {
    enterDeepSleep();
  }

  // Update LED status
  updateStatusLED();

  delay(10);
}

void initBLE() {
  // Initialize BLE Device
  BLEDevice::init(DEVICE_NAME);

  // Create BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));

  // Create BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
    BLEUUID(MIDI_CHARACTERISTIC_UUID),
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_WRITE_NR
  );

  // Add descriptor for notifications
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(MIDI_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Help with iPhone connections
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE MIDI initialized. Waiting for connection...");
}

void checkBootButton() {
  bool currentState = (digitalRead(BOOT_BUTTON) == LOW); // Active low

  if (currentState && !bootButtonPressed) {
    // Button just pressed
    bootPressStartTime = millis();
    bootButtonPressed = true;
  } else if (!currentState && bootButtonPressed) {
    // Button released
    unsigned long pressDuration = millis() - bootPressStartTime;

    if (pressDuration >= BOOT_HOLD_TIME) {
      // Long press - enter pairing mode
      enterPairingMode();
    }

    bootButtonPressed = false;
  }
}

void checkButton() {
  // Read button state (active LOW with pull-up)
  int reading = digitalRead(BUTTON_PIN);

  // Check if button state changed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Only update after debounce delay
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    bool buttonPressed = (reading == LOW);

    if (buttonPressed && !buttonActive) {
      // Button just pressed - send Note On
      buttonActive = true;
      sendNoteOn();
      lastActivityTime = millis();
    } else if (!buttonPressed && buttonActive) {
      // Button just released - send Note Off
      buttonActive = false;
      sendNoteOff();
      lastActivityTime = millis();
    }
  }

  lastButtonState = reading;
}

void sendMIDI(uint8_t type, uint8_t channel, uint8_t note, uint8_t velocity) {
  if (!deviceConnected) {
    Serial.println("Not connected - MIDI not sent");
    return;
  }

  // BLE MIDI packet structure:
  // Byte 0: Header (timestamp high bit + timestamp low bits)
  // Byte 1: Timestamp low (not used in simple implementation)
  // Byte 2: MIDI status byte
  // Byte 3: MIDI data byte 1
  // Byte 4: MIDI data byte 2 (if needed)

  uint8_t status = type | ((channel - 1) & 0x0F);

  midiPacket[0] = 0x80;  // Header byte (timestamp high bit set)
  midiPacket[1] = 0x80;  // Timestamp low (set high bit)
  midiPacket[2] = status;
  midiPacket[3] = note & 0x7F;
  midiPacket[4] = velocity & 0x7F;

  pCharacteristic->setValue(midiPacket, 5);
  pCharacteristic->notify();
}

void sendNoteOn() {
  Serial.printf("MIDI CC: Ch%d CC#%d Value=%d\n", MIDI_CHANNEL, CC_NUMBER, CC_VALUE_ON);
  sendMIDI(0xB0, MIDI_CHANNEL, CC_NUMBER, CC_VALUE_ON); // 0xB0 = Control Change
}

void sendNoteOff() {
  Serial.printf("MIDI CC: Ch%d CC#%d Value=%d\n", MIDI_CHANNEL, CC_NUMBER, CC_VALUE_OFF);
  sendMIDI(0xB0, MIDI_CHANNEL, CC_NUMBER, CC_VALUE_OFF); // 0xB0 = Control Change
}

void enterPairingMode() {
  Serial.println("Entering pairing mode...");

  // Restart advertising
  BLEDevice::startAdvertising();

  // Blink LED rapidly to indicate pairing mode
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }

  lastActivityTime = millis();
}

void enterDeepSleep() {
  Serial.println("Entering deep sleep...");

  // Send any pending MIDI messages
  if (buttonActive) {
    sendNoteOff();
  }

  // Turn off LED
  digitalWrite(LED_PIN, LOW);

  // ESP32-C3 doesn't support ext0 wakeup, use GPIO wakeup instead
  esp_sleep_enable_gpio_wakeup();
  gpio_wakeup_enable((gpio_num_t)BOOT_BUTTON, GPIO_INTR_LOW_LEVEL);
  gpio_wakeup_enable((gpio_num_t)BUTTON_PIN, GPIO_INTR_LOW_LEVEL);

  // Enter deep sleep
  Serial.println("Going to sleep now...");
  delay(100);
  esp_deep_sleep_start();
}

void updateStatusLED() {
  if (deviceConnected) {
    // Connected - LED solid on
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Not connected - LED slow blink
    digitalWrite(LED_PIN, (millis() / 500) % 2);
  }
}
