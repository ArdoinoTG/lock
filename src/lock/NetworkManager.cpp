#include "Arduino.h"
#include "NetworkManager.h"
#include "Entities.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <vector>

/**
* Initializes a new instance of the NetworkManager class.
*/
NetworkManager::NetworkManager() {

}

/**
 * Public methods
 */
 
// Find WiFi parameters and connect to WiFi. If parameters are not specified. Create the Access Point.
// ---------------------

int NetworkManager::Init() {
  
  // get wifi parameters from EEPROM
  Entities::WiFiParameters params = _helper.GetWiFiParameters();

  if (NetworkManager::ConnectToWiFi(params) == true) {
    Serial.print("Connected to WiFi: ");
    Serial.println(params.ssid);

    // exit init function
    return Entities::ConnectedToWiFi;
  }

  Serial.println("Opening AP ... ");
  
  // create Access Point, if connection to wifi is failed
  NetworkManager::SetupAccessPoint();
  
  return Entities::AccessPointCreated;
}

// Scan for available networks
// ---------------------

std::vector<Entities::WiFiNetwork> NetworkManager::ScanForWiFiNetworks() {
  
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  //WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  delay(100);

  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  std::vector<Entities::WiFiNetwork> networks(n);
  
  if (n == 0) {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      networks[i] = (Entities::WiFiNetwork){ WiFi.SSID(i), WiFi.RSSI(i), ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : "*") };
      
      // Print SSID and RSSI for each network found 
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : "*");
      delay(10);
    }
  }
  
  Serial.println("");

  delay(100);
  //WiFi.softAP(ssid);
  //delay(400);
  
  return networks;
}

boolean NetworkManager::DisconnectAndConnectToWiFi(Entities::WiFiParameters params) {
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  return NetworkManager::ConnectToWiFi(params);
}

/**
 * Private methods
 */

// Connect to WiFi.
// ---------------------

boolean NetworkManager::ConnectToWiFi(Entities::WiFiParameters params) {
  Serial.print("Connecting to WiFi: ");
  Serial.println(params.ssid);

  if (params.ssid.length() == 0) {
    // No ssid and/or password specified. Cannot connect to WiFi
    return false;
  }

  WiFi.begin(params.ssid.c_str(), params.password.c_str());

  return NetworkManager::TestAndConnectToWiFi();
}

// Try to connect to WiFi.
// ---------------------

boolean NetworkManager::TestAndConnectToWiFi() {

  Serial.println("Waiting for WiFi to connect ... ");
  
  int retryCounter = 0;
  while (retryCounter < 100) {
    if (WiFi.status() == WL_CONNECTED) { 
      return true; 
    }

    delay(500);

    Serial.print("WiFi status: "); 
    Serial.println(WiFi.status());    
    retryCounter++;
  }

  Serial.println("Connect timed out");
  
  return false;
}

// Setup Access Point
// ---------------------

void NetworkManager::SetupAccessPoint() {

  Serial.println();
  Serial.println("Configuring access point...");
  
  /* You can remove the password parameter if you want the AP to be open. */
  //WiFi.softAP(ssid, password);
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println();
}


