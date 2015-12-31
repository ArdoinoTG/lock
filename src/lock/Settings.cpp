#include "Arduino.h"
#include "Settings.h"

#define CONFIG_VERSION "c01"
#define DEFAULT_PIN "1234"

/**
 * Initializes a new instance of the Settings class.
 */
Settings::Settings() {
  
}

/**
 * Public methods
 */

void Settings::Init() {
  
  Serial.println("Read from EEPROM");

  if (EEPROM.read(0) == CONFIG_VERSION[0] &&
      EEPROM.read(1) == CONFIG_VERSION[1] &&
      EEPROM.read(2) == CONFIG_VERSION[2]) {

    for (unsigned int t=0; t<sizeof(Configuration); t++) {
      *((char*)&Configuration + t) = EEPROM.read(t);
    }

    Settings::ShowEEPROMData();
  }
  else {
    // save init settings
    strcpy(Configuration.pin, DEFAULT_PIN);
    strcpy(Configuration.version, CONFIG_VERSION);
    // set valid record to false
    for (int i = 0; i < 10; i++) {
      Configuration.devices[i].valid = false;
    }
    
    Serial.println("Save init configuration ... ");
    
    Settings::SaveSettings();
  }
}

// Check if card is valid (has valid unlock key)
// ---------------------

boolean Settings::IsCardValid(String uuid) {
  Serial.println("Checking if card is valid ... ");
  Serial.print("uuid: "); Serial.println(uuid);
  
  for (int i = 0; i < 10; i++) {
    String temp(Configuration.devices[i].uuid);
    if (temp == uuid) {
      Serial.println("Card is valid.");
      return true;
    }
  }

  Serial.println("Card is not valid.");
  return false;
}
 
// Save settings to EEPROM
// ---------------------

void Settings::SaveSettings() {
  Serial.println("Save configuration to EEPROM");
  Settings::ShowEEPROMData();
 
  for (unsigned int t=0; t<sizeof(Configuration); t++) {
    EEPROM.write(t, *((char*)&Configuration + t));
  }

  EEPROM.commit();
}

// Register NFC device
// ---------------------
boolean Settings::RegisterNFCDevice(char *dn, const char *uuid) {
  Serial.println("Settings::RegisterNFCDevice");
  Serial.print("Device name: ");
  Serial.println(dn);
  Serial.print("UUID: ");
  Serial.println(uuid);

  // save to first free slot
  for (int i = 0; i < 10; i++) {
    if (!Configuration.devices[i].valid) {
      Serial.println("write to EEPROM");
      // set device slot to valid
      Configuration.devices[i].valid = true;
      // set device slot name
      strcpy(Configuration.devices[i].name, dn);
      // set devic slot uuid
      strcpy(Configuration.devices[i].uuid, uuid);
      // save settings
      Settings::SaveSettings();
      // return true
      return true;
    }
  }

  Serial.println("write to EEPROM false");
  
  return false;
}

// Delete NFC devices
// ---------------------
boolean Settings::DeleteNFCDevices(String devices) {
  return true;
}

/**
 * Private methods
 */

// Show EEPROM data
// ---------------------
void Settings::ShowEEPROMData() {
  Serial.print("Version: ");
  Serial.println(Configuration.version);
  Serial.print("PIN: ");
  Serial.println(Configuration.pin);
  Serial.print("SSID: ");
  Serial.println(Configuration.ssid);
  Serial.print("Password: ");
  Serial.println(Configuration.password);

  Serial.println("Registered devices: ");
  for (int i = 0; i < 10; i++) {
    Serial.print("Device "); Serial.print(i); Serial.println(":");
    Serial.print("Valid: "); Serial.println(Configuration.devices[i].valid);
    Serial.print("Name: "); Serial.println(Configuration.devices[i].name);Serial.print("UUID: "); Serial.println(Configuration.devices[i].uuid);
  }
}


