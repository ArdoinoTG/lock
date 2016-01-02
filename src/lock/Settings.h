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
      char ssid[33];
      // ssid password
      char password[33];
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
    boolean IsCardValid(const char *);

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
    boolean RegisterNFCDevice(const char *, const char *);

    /**
     * Delete NFC devices
     * 
     * @param1: device uuids
     */
    boolean DeleteNFCDevices(String);

    /**
     * Check whether device name already exits or not
     * 
     * @param1: device name
     */
    boolean DeviceNameExists(const char *);

  private:

    /**
     * Show EEPROM data
     */
    void ShowEEPROMData();

    /**
     * Sort devices by name
     */
    void Sort();
};

#endif
