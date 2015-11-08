#ifndef Helper_h
#define Helper_h

#include "Arduino.h"
#include "Entities.h"

#include <EEPROM.h>
#include <vector>

class Helper
{
  
  public:
    /**
     * Initializes a new instance of the Helper class.
     */
    Helper();

    /**
     * Find stored wifi parameters in EEPROM
     */
    Entities::WiFiParameters GetWiFiParameters();

    /**
     * Build welcome response
     */
    String BuildWelcomeResponse();

    /**
     * Build welcome response with register to wifi network button
     */
    String BuildWelcomeRegisterResponse();

    /**
     * Build available WiFi netwroks response
     * 
     * @param1 - list of available wifi networks
     */
    String BuildAvailableWiFiReponse(std::vector<Entities::WiFiNetwork>&);
    
  private:

    const char *_start = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n";
};

#endif
