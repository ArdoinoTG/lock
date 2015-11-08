#include "Arduino.h"
#include "Helper.h"
#include "Entities.h"

#include <EEPROM.h>
#include <vector>

/**
 * Initializes a new instance of the Helper class.
 */
Helper::Helper() {

}

/**
 * Public methods
 */
 
// Read parameters from EEPROM
// ---------------------

Entities::WiFiParameters Helper::GetWiFiParameters() {
  Entities::WiFiParameters params;

  // read from eeprom for ssid and pass
  Serial.println("Reading from EEPROM ...");

  String wifissid = "";
  for (int i = 0; i < 32; ++i)
  {
    if (EEPROM.read(i) != 255) {
      wifissid += char(EEPROM.read(i));
    }
  }
  
  Serial.print("SSID: ");
  Serial.println(wifissid);
  
  String wifipass = "";
  for (int i = 32; i < 96; ++i)
  {
    if (EEPROM.read(i) != 255) {
      wifipass += char(EEPROM.read(i));
    }
  }
  
  Serial.print("PASSWORD: ");
  Serial.println(wifipass);  
  
  //params.ssid = wifissid;
  //params.password = wifipass;

  // clear this
  params.ssid = "Les Couleurs";
  params.password = "1nt3rn3t";

  return params;
}

// Build welcome response
// ---------------------

String Helper::BuildWelcomeResponse() {
  String response = Helper::_start;
  response += "<html><head><title>Welcome</title><meta charset=\"utf-8\" /><style>body {font-size: 20px;} .row {width: 100%;padding: 4px 0;} .row .left{float:left; width: 200px;} .row .right {margin-left: 210px;} select {width: 146px;} input[type=text] {width: 150px;}</style></head><body>";
  response += "<form method=\"get\" action=\"/unlock\" id=\"unlock-form\"><div class=\"row\"><h1>Welcome</h1></div><hr />";
  response += "<div class=\"row\">You are connected to the network.</div>";
  response += "<div class=\"row\"><div class=\"left\">Enter pin</div><div class=\"right\"><div class=\"control\"><input type=\"password\" length=\"4\" name=\"pin\" id=\"pin\" /></div></div></div>";
  response += "<div class\"row\"><input type=\"submit\" value=\"Unlock\"></div></form>";
  response += "</form></body></html>";

  return response;
}

// Build welcome response with register to wifi network button
// ---------------------

String Helper::BuildWelcomeRegisterResponse() {
  String response = Helper::_start;
  response += "<html><head><title>Welcome</title><meta charset=\"utf-8\" /><style>body {font-size: 20px;} .row {width: 100%;padding: 4px 0;} .row .left{float:left; width: 200px;} .row .right {margin-left: 210px;} select {width: 146px;} input[type=text] {width: 150px;}</style></head><body>";
  response += "<div class=\"row\"><h1>Welcome</h1></div><hr />";
  response += "<div class=\"row\"><div class=\"left\">Register to WiFi network</div><div class=\"right\"><a href=\"/register\">Register</a></div></div>";
  response += "</body></html>";

  return response;
}

// Build Available WiFi response
// ---------------------

String Helper::BuildAvailableWiFiReponse(std::vector<Entities::WiFiNetwork> &networks) {
  
  String response = Helper::_start;
  response += "<html><head><title>Setup WiFi connection</title><meta charset=\"utf-8\" /><style>body {font-size: 20px;} .row {width: 100%;padding: 4px 0;} .row .left{float:left; width: 200px;} .row .right {margin-left: 210px;} .row .right .control {width: 200px;} select {width: 100%;} input[type=text] {width: 100%;}</style></head><body>";
  response += "<form method=\"get\" action=\"/register-wifi\" id=\"register-form\"><div class=\"row\"><h1>Select WiFi network</h1></div><hr />";
  response += "<div class=\"row\"><div class=\"left\">Available networks</div><div class=\"right\"><div class=\"control\"><select name=\"ssid\" id=\"ssid\">";
  
  for (int i = 0; i < networks.size(); i++) {
      Entities::WiFiNetwork network = networks[i];
      response += "<option value=\"" + network.ssid + "\">" + network.ssid + " (" + network.rssi + ")" + network.encType + "</option>";
  }

  response += "</select></div></div></div>"; 
  response += "<div class=\"row\"><div class=\"left\">Enter password</div><div class=\"right\"><div class=\"control\"><input type=\"password\" length=\"64\" name=\"password\" id=\"password\" /></div></div></div>";
  response += "<div class\"row\"><button type=\"button\" onclick=\"encodeUri();\">Save</button></div></form>";
  response += "<script>function encodeUri() { document.getElementById('register-form').submit();};</script>";
  response += "</body></html>";

  return response;
}




