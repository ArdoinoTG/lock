#ifndef Settings_h
#define Settings_h

#include "Arduino.h"
#include <EEPROM.h>

class Settings
{
  
  public:

    struct NFC_t 
    {
      // access device name
      char name[20];
      // access uuid
      char uuid[36];
      
    } NFC;
    
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
      NFC_t devices[10];
      
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

};

#endif
