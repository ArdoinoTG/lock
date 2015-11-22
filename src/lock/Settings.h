#ifndef Settings_h
#define Settings_h

#include "Arduino.h"
#include <EEPROM.h>

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
     * Save settings
     */
    void SaveSettings();

  private:


    
    
};

#endif
