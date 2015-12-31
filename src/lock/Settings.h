#ifndef Settings_h
#define Settings_h

#include "Arduino.h"
#include <EEPROM.h>
#include "Entities.h"

class Settings
{
  
  public:

    struct Config_t
    {
      // special configuration version
      char version[4];
      // ssid name
      char ssid[32];
      // ssid password
      char password[32];
      // unlock pin number
      char pin[5];
      
      // registered devices
      Entities::NFC devices[10];
      
    } Configuration;
    
    /**
     * Initializes a new instance of the Settings class.
     */
    Settings();

    /**
     * Init
     */
    void Init();

    /**
     * Check if card is valid (has valid unlock key). If card is valid unlock command will be send.
     * 
     * @param1: uuid
     */
    boolean IsCardValid(String);

    /**
     * Save settings
     */
    void SaveSettings();

    /**
     * Register NFC device
     * 
     * @param1: device name
     * @param2: device uuid
     */
    boolean RegisterNFCDevice(char *, const char *);

    /**
     * Delete NFC devices
     * 
     * @param1: device list
     */
    boolean DeleteNFCDevices(String);

  private:

    /**
     * Show EEPROM data
     */
    void ShowEEPROMData();
};

#endif
