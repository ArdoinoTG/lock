/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <vector>

#include "NetworkManager.h"
#include "RequestHandler.h"
#include "Helper.h"

WiFiServer _server(80);

NetworkManager _nm;
Helper _helper;
RequestHandler _rh;
int _serverResult;
boolean _unlockInProgress = false;

/* 
 * Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */

void setup() {
  delay(500);
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(500);

  Serial.println("Initializing server ... ");

  /* Check for saved wifi connection. If there is no saved connection Access point will will be created.  */
  _serverResult = _nm.Init();
 
  _server.begin();

  Serial.println("HTTP server started ... ");

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

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  String response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body><h1>Unknown request</h1><hr /></body></html>";
  
  int action = _rh.ProcessRequest(req, _serverResult);
  Serial.print("action: ");
  Serial.print(action);
  switch (action) {
    case Entities::Welcome:
      {
        Serial.println(" - Welcome");
        response = _helper.BuildWelcomeResponse();
      }
      break;
    case Entities::WelcomeWithRegisterLink:
      {
        Serial.println(" - WelcomeWithRegisterLink");
        response = _helper.BuildWelcomeRegisterResponse();
      }
      break;
    case Entities::Register:
      {
        Serial.println(" - Register");
        std::vector<Entities::WiFiNetwork> networks = _nm.ScanForWiFiNetworks();
        response =_helper.BuildAvailableWiFiReponse(networks);
      } 
      break;
    case Entities::RegisterWiFi:
      {
        Serial.println(" - RegisterWiFi");

        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>WiFi network is registered. Connecting to selected network ... </body></html>";

        Entities::WiFiParameters params = _rh.GetWiFiParameters();
        
        Serial.println("------------");
        Serial.println(params.ssid);
        Serial.println(params.password);
        Serial.println("------------");
        
        //params.ssid = "Les Couleurs";
        //params.password = "1nt3rn3t";
       
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
      }
      break;
    case Entities::Unlock:
      {
        Serial.println(" - Unlock");

        String PIN = _rh.GetUnlockPIN();

        Serial.print("PIN; ");
        Serial.println(PIN);

        _unlockInProgress = PIN == "1234"; // Get 1234 from EEPROM

        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>Unlocking </body></html>";
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

  delay(1);
}

unsigned long previousMillis = 0;
const long interval = 10000;
int ledState = LOW; 

void unlock() {
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   
    if (ledState == LOW) {
      ledState = HIGH;  // Note that this switches the LED *off*
      _unlockInProgress = true;
    }
    else {
      ledState = LOW;   // Note that this switches the LED *on*
      _unlockInProgress = false;
      previousMillis = 0;
    }
    
    digitalWrite(D1, ledState);
  }
}

