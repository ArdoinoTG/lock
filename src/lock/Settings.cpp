#include "Arduino.h"
#include "Settings.h"

#define CONFIG_VERSION "c01"
#define DEFAULT_PIN "1234"

/**
 * Initializes a new instance of the Helper class.
 */
Settings::Settings() {
  
}

/**
 * Public methods
 */

void Settings::Init() {
  
  Serial.println("Read from EEPROM");

  Serial.print("0: ");
  Serial.println(EEPROM.read(0));
  Serial.print("1: ");
  Serial.println(EEPROM.read(1));
  Serial.print("2: ");
  Serial.println(EEPROM.read(2));
  
  if (EEPROM.read(0) == CONFIG_VERSION[0] &&
      EEPROM.read(1) == CONFIG_VERSION[1] &&
      EEPROM.read(2) == CONFIG_VERSION[2]) {

    for (unsigned int t=0; t<sizeof(Configuration); t++) {
      *((char*)&Configuration + t) = EEPROM.read(t);
    }
  }
  else {
    // save init settings
    strcpy(Configuration.pin, DEFAULT_PIN);
    strcpy(Configuration.version, CONFIG_VERSION);
    Serial.println("Save init configuration ... ");
    
    Settings:SaveSettings();
  }
}
 
// Save settings to EEPROM
// ---------------------

void Settings::SaveSettings() {
  Serial.println("Save configuration to EEPROM");
  Serial.print("Version: ");
  Serial.println(Configuration.version);
  Serial.print("PIN: ");
  Serial.println(Configuration.pin);
  Serial.print("SSID: ");
  Serial.println(Configuration.ssid);
  Serial.print("Password: ");
  Serial.println(Configuration.password);
  
  for (unsigned int t=0; t<sizeof(Configuration); t++) {
    EEPROM.write(t, *((char*)&Configuration + t));
  }

  EEPROM.commit();
}

/**
 * Private methods
 */



