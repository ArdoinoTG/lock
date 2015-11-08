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
        /**
         * WiFi ssid
         */
        String ssid;
  
        /**
         * WiFi password
         */
        String password;
    };

    struct WiFiNetwork {
      public:
        String ssid;
        int rssi;
        String encType;
    };

    static int const ConnectedToWiFi = 1;
    static int const AccessPointCreated = 2;
    static int const Failure = 100;

    // requests
    static int const Welcome = 1;
    static int const WelcomeWithRegisterLink = 2;
    static int const Register = 3;
    static int const RegisterWiFi = 4;
    static int const Administration = 10;
    static int const Unlock = 50;
    static int const UnknownRequest = 100;
};

#endif

