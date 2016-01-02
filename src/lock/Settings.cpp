#include "Arduino.h"
#include "Settings.h"

#define CONFIG_VERSION "c01"
#define DEFAULT_PIN "1234"
#define DEVICES 10

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
    // REMOVE THIS
    /*strcpy(Configuration.ssid, "Les Couleurs");
    strcpy(Configuration.password, "1nt3rn3t");*/
    // init device slots
    for (int i = 0; i < DEVICES; i++) {
      Configuration.devices[i].valid = false;
      strcpy(Configuration.devices[i].name, "");
      strcpy(Configuration.devices[i].uuid, "");
    }
    
    Serial.println("Save init configuration ... ");
    
    Settings::SaveSettings();
  }
}

// Check if card is valid (has valid unlock key)
// ---------------------
boolean Settings::IsCardValid(const char *uuid) {
  Serial.println("Checking if card is valid ... ");
  Serial.print("uuid: "); Serial.println(uuid);
  
  for (int i = 0; i < DEVICES; i++) {
    if (strcmp(Configuration.devices[i].uuid, uuid) == 0 && Configuration.devices[i].valid == true) {
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

  Serial.println("EEPROM before save:");
  Settings::ShowEEPROMData();
  
  Settings::Sort();
  
  for (unsigned int t=0; t<sizeof(Configuration); t++) {
    EEPROM.write(t, *((char*)&Configuration + t));
  }

  EEPROM.commit();

  Serial.println("EEPROM after save:");
  Settings::ShowEEPROMData();
}

// Register NFC device
// ---------------------
boolean Settings::RegisterNFCDevice(const char *dn, const char *uuid) {
  Serial.println("Settings::RegisterNFCDevice");
  Serial.print("Device name: ");Serial.println(dn);
  Serial.print("UUID: ");Serial.println(uuid);

  bool save = false;
  // save to first free slot
  for (int i = 0; i < DEVICES; i++) {
    if (!Configuration.devices[i].valid && !save) {
      Serial.println("Found free slot");
      // set device slot to valid
      Configuration.devices[i].valid = true;
      // set device name
      strcpy(Configuration.devices[i].name, dn);
      // set device uuid
      strcpy(Configuration.devices[i].uuid, uuid);
      
      save = true;
    }
  }

  if (save == true) {
    // save settings
    Settings::SaveSettings();
  }
  else {
    Serial.println("write to EEPROM false");
  }
  
  return save;
}

// Delete NFC devices
// @param1: device uuids
// ---------------------
boolean Settings::DeleteNFCDevices(String uuids) {
  Serial.println("Settings::DeleteNFCDevices");
  Serial.println(uuids);

  int pos = 0;
  int len = uuids.length();
  
  for (int i = 0; i < len ; i++) {
    if (uuids[i] == ';' || i == (len - 1)) {
      int sLen = (i == (len - 1)) ? (i + 1) : i;
      
      String string = uuids.substring(pos, sLen);

      Serial.print("Removing key: ");
      Serial.println(string);

      for (int i = 0; i < DEVICES; i++) {
        if (strcmp(Configuration.devices[i].uuid, string.c_str()) == 0) {
          // remove
          Configuration.devices[i].valid = false;
          strcpy(Configuration.devices[i].name, "");
          strcpy(Configuration.devices[i].uuid, "");
          Serial.print(Configuration.devices[i].uuid);Serial.println(" key is removed.");
        }
      }
      
      pos = i + 1;
    }
  }

  Settings::SaveSettings();

  return true;
}

// Check whether device name already exits or not
// @param1: device name
// ---------------------
boolean Settings::DeviceNameExists(const char *dn) {
  Serial.println("Settings::DeviceNameExists");
  Serial.print("Device name: ");Serial.println(dn);

  for (int i = 0; i < DEVICES; i++) {
    if (Configuration.devices[i].valid && strcmp(Configuration.devices[i].name, dn) == 0) {
      Serial.print("Found device with same name on slot: ");Serial.println(i + 1);
      return true;
    }
  }

  return false;
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
  for (int i = 0; i < DEVICES; i++) {
    Serial.println("--------------");
    Serial.print("Device "); Serial.print(i+1); Serial.println(":");
    Serial.print("Valid: "); Serial.println(Configuration.devices[i].valid);
    Serial.print("Name: "); Serial.println(Configuration.devices[i].name);Serial.print("UUID: "); Serial.println(Configuration.devices[i].uuid);
    Serial.println("--------------");
  }
}

// Sort devices by name
// ---------------------
void Settings::Sort() {
  Serial.println("Sort devices by name ... ");
  Entities::NFC tnfc;
  for (int i = 0; i < DEVICES - 1; i++)
  {
    for (int j = i + 1; j < DEVICES; j++)
    {
      if ((strcmp(Configuration.devices[i].name, "") == 0 && strcmp(Configuration.devices[i].name, Configuration.devices[j].name) < 0) ||
          (strcmp(Configuration.devices[j].name, "") != 0 && strcmp(Configuration.devices[i].name, Configuration.devices[j].name) > 0))
      {
        tnfc = Configuration.devices[i];
        Configuration.devices[i] = Configuration.devices[j];
        Configuration.devices[j] = tnfc;
      }
    }
  }
}


