#ifndef NetworkManager_h
#define NetworkManager_h

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "Entities.h"
#include "Helper.h"
#include <vector>

class NetworkManager {

  public:
    /**
    * Initializes a new instance of the NetworkManager class.
    */
    NetworkManager();

    /**
     * Initializes AccessPoint in case when: WiFi parameters are not found or connection to the WiFi is not established. 
     * Status: 
     * 1 - connected to WiFi
     * 2 - Access point created
     * 100 - Failure
     */
    int Init();

    /**
     * Scan for WiFi networks.
     */
    std::vector<Entities::WiFiNetwork> ScanForWiFiNetworks();

    /**
    * Connect to selected wifi network
    * 
    * @param1 - wifi parameters (ssid and password)
    */
    boolean DisconnectAndConnectToWiFi(Entities::WiFiParameters);

  private:

    /**
     * Connect to selected wifi network
     */
    boolean ConnectToWiFi(Entities::WiFiParameters);

    /**
    * Setup access point
    */
    void SetupAccessPoint();

    /**
     * Try to connect to WiFi networks. Retry counter is set to 20.
     */
    boolean TestAndConnectToWiFi();

    /**
     * Helper instance.
     */
    Helper _helper;
    
    /** 
    * Credentials (ssid and password). 
    */
    const char *ssid = "LockAP";
    //const char *password = "admin123";
};


#endif
