/**
 * Access Point url http://192.168.4.1
 */
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <vector>
#include "NetworkManager.h"
#include "RequestHandler.h"

WiFiServer _server(80);
NetworkManager _nm;
RequestHandler _rh;
int _serverResult;
boolean _unlockInProgress = false;
unsigned long _previousMillis = 0;
const long _interval = 10000; // read from settings
int _ledState = LOW; 

void setup() {
  delay(500);  
  Serial.begin(115200); // init Serial port
  EEPROM.begin(4096); // init EEPROM
  delay(500);

  Serial.println("Initializing server ... ");

  /* Check for saved wifi connection. If there is no saved WiFi netoworks, Access point will be created. Access Point URL: http://192.168.4.1 */
  _serverResult = _nm.Init();
  _server.begin();

  Serial.println("HTTP server started ... ");
  
  // unlock PIN out
  pinMode(D1, OUTPUT);
}

void loop() {

  if (_unlockInProgress == true) {
    unlock();
  }
  
  // Check if a client has connected
  WiFiClient client = _server.available();
  if (!client) {
    return;
  }

  unsigned long pm = millis();

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    //Serial.println("new client is not available");

    if (millis() - pm > 10000) {
      // return after 10 seconds
      Serial.println("new client was not available for 100 seconds");
      return;
    }

    delay(10);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  String response = _rh.BuildUnknownRequestReponse();
  
  int action = _rh.ProcessRequest(req, _serverResult);
  Serial.print("action: ");
  Serial.print(action);
  
  switch (action) {
    case Entities::Welcome:
      {
        Serial.println(" - Welcome");
        response = _rh.BuildMainResponse(false, false);
      }
      break;
    case Entities::Register:
      {
        Serial.println(" - Register");
        std::vector<Entities::WiFiNetwork> networks = _nm.ScanForWiFiNetworks();
        response =_rh.BuildAvailableWiFiReponse(networks);
      } 
      break;
    case Entities::RegisterWiFi:
      {
        Serial.println(" - RegisterWiFi");

        response = _rh.BuildRegisteredToWiFiNetworkResponse();

        Entities::WiFiParameters params = _rh.GetWiFiParameters();
        
        Serial.println("------------");
        Serial.println(params.ssid);
        Serial.println(params.password);
        Serial.println("------------");

        delay(100);
        if (_nm.DisconnectAndConnectToWiFi(params)) {
          Serial.println("Connected to selected WiFi");
          _serverResult = Entities::ConnectedToWiFi;  
          _server.begin();
        }
        else {
          // set error
          Serial.println("Failed to connect to WiFi ...");
        }
      }
      break;
    case Entities::Administration:
      {
        Serial.println(" - Administration");
        response = _rh.BuildAdministrationResponse(1, false);
      }
      break;
    case Entities::Unlock:
      {
        Serial.println(" - Unlock");

        String PIN = _rh.GetUnlockPIN();

        Serial.print("PIN: ");
        Serial.println(PIN);

        boolean incorrectPin = true;
        if (PIN == _nm._settings.Configuration.pin) { // read PIN from settings
          _previousMillis = millis();
          _unlockInProgress = true;
          incorrectPin = false;
        }

        response = _rh.BuildMainResponse(!incorrectPin, incorrectPin);
      }
      break;
    case Entities::AdministrationPIN:
      {
        Serial.println(" - Administration - PIN");
        response = _rh.BuildAdministrationResponse(2, false);
      }
      break;
    case Entities::AdministrationSetPIN:
      {
        Serial.println(" - Administration - Set PIN");

        Entities::PINParameters params = _rh.GetPINParameters();

        Serial.println("------------");
        Serial.println(params.oldpin);
        Serial.println(params.newpin);
        Serial.println(params.cnewpin);
        Serial.println("");
        Serial.println(_nm._settings.Configuration.pin);
        Serial.println("------------");

        String temp1(params.oldpin);
        String temp2(_nm._settings.Configuration.pin);
        String temp3(params.newpin);
        String temp4(params.cnewpin);
        
        // check old pin
        if (temp1 != temp2) {
          // incorrect old pin
          Serial.println("Incorrect old pin");
          response = _rh.BuildAdministrationResponse(2, true);  
        }
        else if (temp3 != temp4) {
          // check new pin and confirm new pin match
          Serial.println("New pin and confirm new pin does not match");
          //TODO inform user
          response = _rh.BuildAdministrationResponse(2, false);
        }
        else {
          Serial.println("Save new pin");
          // save
          strcpy(_nm._settings.Configuration.pin, params.newpin);
          _nm._settings.SaveSettings();
          // return administration
          response = _rh.BuildAdministrationResponse(1, false);
        }
      }
      break;
    case Entities::AdministrationNFC:
      {
        Serial.println(" - Administration - NFC");
        response = _rh.BuildAdministrationResponse(3, false);
      }
      break;
    case Entities::AdministrationSetNFC:
      {
        Serial.println(" - Administration- Set NFC");
        response = _rh.BuildAdministrationResponse(3, false);
      }
      break;
    case Entities::UnknownRequest:
      {
        Serial.println(" - UnknownRequest");
      }
      break;
    default:
      {
        Serial.print(" - default");
      }
      break;
  }

  Serial.println(response);

  client.flush();

  client.print(response);

  delay(10);
}

/**
 * Send unlock signal for 10 seconds - Move to user settings?
 */
void unlock() {
  
  unsigned long currentMillis = millis();

  if (currentMillis - _previousMillis < _interval) {
    if (_ledState == LOW) {
      _ledState = HIGH;
      digitalWrite(D1, _ledState);  
    }
  }
  else {
    
    _ledState = LOW;
    _unlockInProgress = false;
    digitalWrite(D1, _ledState);
  }
}





