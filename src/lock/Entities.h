#ifndef Entities_h
#define Entities_h

#include "Arduino.h"

class Entities {

  public:
    /**
     * WiFi connection parameters.
     */
    struct WiFiParameters
    {
      public: 
        String ssid;
        String password;
    };

    /**
     * WiFi name and encryption type - used when searching for available WiFi networks
     */
    struct WiFiNetwork {
      public:
        String ssid;
        String encType;
    };

    /**
     * Reset PIN
     */
    struct PINParameters {
      public:
        char oldpin[5];
        char newpin[5];
        char cnewpin[5];
    };

    /**
     * Chip state:
     * 1. connected to selected wifi
     * 2. access point is created
     */
    static int const ConnectedToWiFi = 1;
    static int const AccessPointCreated = 2;

    /**
     * Request types
     */
    static int const Welcome = 1;
    static int const Register = 2; // also Administration: set-wifi
    static int const RegisterWiFi = 3;
    static int const Administration = 10;
    static int const AdministrationPIN = 20;
    static int const AdministrationSetPIN = 21;
    static int const AdministrationNFC = 30;
    static int const AdministrationSetNFC = 31;
    static int const Unlock = 100;
    static int const UnknownRequest = -1;
};

#endif

