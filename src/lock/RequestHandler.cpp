#include "Arduino.h"
#include "RequestHandler.h"

/**
* Initializes a new instance of the RequestHandler class.
*/
RequestHandler::RequestHandler() {

}

/**
 * Public methods
 */

// Handle client request and return action for next steps.
// ---------------------

int RequestHandler::ProcessRequest(String &request, int serverResult) {

  // -----------------
  // PARSE REQUEST
  int addr_start = request.indexOf(' ');
  int addr_end = request.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    return Entities::UnknownRequest;
  }

  String req = request.substring(addr_start + 1, addr_end);
  Serial.print("Parsed request: ");
  Serial.println(req);
  // PARSE REQUEST END
  // -----------------

  if (req == "/") {
    if (serverResult == Entities::ConnectedToWiFi) {
      return Entities::Welcome;  
    }
    else if (serverResult == Entities::AccessPointCreated) {
      return Entities::Register;
    }
  }
  else if (req.startsWith("/register-wifi?")) {
    String qs = req.substring(req.indexOf('?') + 1);
    qs.replace('+', ' ');
    
    char *out = new char[qs.length() + 1];
    RequestHandler::DecodeURI(qs.c_str(), out);
    out[qs.length()] = '\0'; // don't forget the terminating 0
    
    String temp(out);    
    RequestHandler::_queryString = temp;
    delete[] out;

    Serial.print("Decoded request: ");
    Serial.println(RequestHandler::_queryString);
    
    return Entities::RegisterWiFi;
  }
  else if (req.startsWith("/unlock?")) {
    RequestHandler::_queryString = req.substring(req.indexOf('?') + 1);
    Serial.println(RequestHandler::_queryString);
    
    return Entities::Unlock;
  }
  else if (req == "/administration") {
    return Entities::Administration;
  }
  else if (req == "/set-wifi") {
    return Entities::Register;
  }
  else if (req == "/set-pin") {
    return Entities::AdministrationPIN;
  }
  else if (req.startsWith("/set-pin?oldpin=")) {
    RequestHandler::_queryString = req.substring(req.indexOf('?') + 1);
    Serial.println(RequestHandler::_queryString);

    return Entities::AdministrationSetPIN;
  }
  else if (req == "/set-nfc") {
    return Entities::AdministrationNFC;
  }
  
  return Entities::UnknownRequest;
}

// Get WiFi parameters from query string.
// ---------------------

Entities::WiFiParameters RequestHandler::GetWiFiParameters() {
  Entities::WiFiParameters params;

  int pos = 0;
  int len = _queryString.length();
  
  for (int i = 0; i < len ; i++) {
    if (_queryString[i] == '&' || i == (len - 1)) {
      int sLen = (i == (len - 1)) ? (i + 1) :  i;
      
      String string = _queryString.substring(pos, sLen);
      
      int separatorPos = string.indexOf('=');

      String left = string.substring(0, separatorPos);
      String right = string.substring(separatorPos + 1);

      if (left == "ssid") {
        params.ssid = right;
      }
      else if (left == "password") { 
        params.password = right;
      }
      
      pos = i + 1;
    }
  }

  return params;
}

// Get unlock PIN.
// ---------------------

String RequestHandler::GetUnlockPIN() {
  return _queryString.substring(_queryString.indexOf('=') + 1);
}

// Get new PIN parameters
// ---------------------
Entities::PINParameters RequestHandler::GetPINParameters() {
  Entities::PINParameters params;

  int pos = 0;
  int len = _queryString.length();
  
  for (int i = 0; i < len ; i++) {
    if (_queryString[i] == '&' || i == (len - 1)) {
      int sLen = (i == (len - 1)) ? (i + 1) :  i;
      
      String string = _queryString.substring(pos, sLen);
      
      int separatorPos = string.indexOf('=');

      String left = string.substring(0, separatorPos);
      String right = string.substring(separatorPos + 1);

      if (left == "oldpin") {
        strcpy(params.oldpin, right.c_str());
      }
      else if (left == "newpin") { 
        strcpy(params.newpin, right.c_str());
      }
      else if (left == "cnewpin") { 
        strcpy(params.cnewpin, right.c_str());
      }
      
      pos = i + 1;
    }
  }

  return params;
}

// Build main response
// Responses:
// 1. Main
// 2. Main - Incorrect PIN
// 3. Main - Unlocking
// 
// @param1: unlock
// @param2: incorrect pin
// ---------------------

String RequestHandler::BuildMainResponse(boolean unlock, boolean incorrectPin) {
  String response = RequestHandler::HtmlStart("Welcome");
  response += "<div class=\"ch1 r\">Your Smart Phone lock system is now ready</div>";
  
  if (unlock) {
    response += "<div class=\"cw r\">Unlocking ...</div><div class=\"cw p40\"><div class=\"ctrlw\"><a href=\"/\" class=\"i a\">Back</a></div></div>";
  }
  else {    
    response += "<div class=\"cw r\">Enter your pin and press unlock to get acces to your door</div><form method=\"get\" action=\"/unlock\">";
    response += "<div class=\"cw p40\">Enter your PIN</div><div class=\"cw\"><div class=\"cwl\"><div class=\"ctrlw\"><input type=\"password\" maxlength=\"4\" name=\"password\" id=\"password\" class=\"i\" /></div></div><div class=\"cwr r\"><div class=\"cw ";
    response += incorrectPin ? "" : "hidden";  
    response += "\" style=\"height:35px;\">Incorrect PIN</div></div></div>";
    response += "<div class=\"cw p20\"><div class=\"cwl\"><div class=\"ctrlw\"><input type=\"submit\" value=\"Unlock\" class=\"i p\" /></div></div>";
    response += "<div class=\"cwr\"><div class=\"ctrlw\"><a href=\"/administration\" class=\"i a\">Administration</a></div></div></div></form>";
  }
  
  response += "<hr /><div class=\"cw f18\">Your Pin and password are only for your personal use and you should not share it to unknown Persons.</div>";
  response += "</div></body></html>";

  return response;
}

// Build Available WiFi response
// Responses:
// 1. Register
//
// @param1 - list of available wifi networks
// ---------------------

String RequestHandler::BuildAvailableWiFiReponse(std::vector<Entities::WiFiNetwork> &networks) {

  String response = RequestHandler::HtmlStart("Setup WiFi connection");
  response += "<form method=\"get\" action=\"/register-wifi\"><div class=\"ch1 r\">Welcome to your Smart Phone lock system</div><div class=\"cw r\">We are searching for WiFi in range ...</div><div class=\"ch2 cw p40\">Register to your home WiFi network</div>";
  response += "<div class=\"cw\">Please pick up your WiFi, enter your password and click login</div><div class=\"cw p40\">";
  response += "<div class=\"cwl\">Pick your home WiFi Network</div><div class=\"cwr\">Enter your SSID Password</div></div><div class=\"cw\"><div class=\"cwl\"><div class=\"ctrlw\"><select name=\"ssid\" id=\"ssid\" class=\"i\">";
   
  for (int i = 0; i < networks.size(); i++) {
      Entities::WiFiNetwork network = networks[i];
      response += "<option value=\"" + network.ssid + "\">" + network.ssid + " " + network.encType + "</option>";
  }

  response += "</select></div></div><div class=\"cwr\"><div class=\"ctrlw\"><input type=\"password\" length=\"64\" name=\"password\" id=\"password\" class=\"i\" /></div></div></div>"; 
  response += "<div class=\"cw p20\"><div class=\"ctrlw\"><input type=\"submit\" value=\"Save\" class=\"i p\" /></div></div></form></div></body></html>";

  return response;
}

// Build registered to WiFi network response
// Responses:
// 1. Registering to WiFi - TODO: find out why this is not returned to user
//
// ---------------------

String RequestHandler::BuildRegisteredToWiFiNetworkResponse() {
  String response = RequestHandler::HtmlStart("Setup WiFi connection - Registering");
  response += "<div class=\"ch1 r\">Welcome to your Smart Phone lock system</div><div class=\"cw r\">Registering to selected WiFi network ...</div>";
  response += "<div class=\"cw p40\">Lock system will automatically connect to selected network. You can access the lock system by entering following url to browser 192.168.1.143.</div></div></body></html>";
  
  return response;
}



// Build administration response
// Responses:
// 1. Administration
// 2. Administration - Set new PIN
// 3. Administration - Incorrect PIN
// 4. Administration - NFC TODO: Card detecting
//
// Administration Type
// 1. Main administration page
// 2. Set PIN
// 3. NFC
//
// @param1: administation type
// @param2: incorrect (old) PIN
// ---------------------

String RequestHandler::BuildAdministrationResponse(int administrationType, boolean incorrectPin) {
  if (administrationType < 1 || administrationType > 3)
  {
    // TODO: unknown request 
  }
  
  String response;
  if (administrationType == 1)
  {
    response += RequestHandler::HtmlStart("Administration");  
  }
  else if (administrationType == 2) 
  {
    response += RequestHandler::HtmlStart("Administration - Set PIN");
  }
  else if (administrationType == 3) 
  {
    response += RequestHandler::HtmlStart("Administration - NFC");
  }

  response += "<div class=\"ch1 r\">Smart Key Lock Settings</div>";

  if (administrationType == 1)
  {
    response += "<div class=\"cw r\">Select setting that you will like to change</div><div class=\"cw p40\"><div class=\"ctrlw\"><a href=\"/set-wifi\" class=\"i a\">Register new WiFi</a></div></div>";
    response += "<div class=\"cw p20\"><div class=\"ctrlw\"><a href=\"/set-pin\" class=\"i a\">Set new PIN</a></div></div><div class=\"cw p20\"><div class=\"ctrlw\"><a href=\"/set-nfc\" class=\"i a\">Register NFC card</a></div></div><div class=\"cw p20\"><div class=\"ctrlw\"><a href=\"/\" class=\"i a\">Back</a></div></div></div>";
  }
  else if (administrationType == 2) 
  {
    response += "<form method=\"get\" action=\"/set-pin\" id=\"pin\"><div class=\"cw r\">Set new PIN</div><div class=\"cw p40\">Old PIN</div>";
    response += "<div class=\"cw\"><div class=\"cwl\"><div class=\"ctrlw\"><input type=\"password\" maxlength=\"4\" name=\"oldpin\" id=\"oldpin\" class=\"i\" required /></div></div><div class=\"cwr r\"><div class=\"cw ";
    response += incorrectPin ? "" : "hidden";  
    response += "\" style=\"height:35px;\">Incorrect PIN</div></div></div><div class=\"cw p20\">New PIN</div>";
    response += "<div class=\"cw\"><div class=\"ctrlw\"><input type=\"password\" length=\"4\" name=\"newpin\" id=\"newpin\" class=\"i\" required /></div></div><div class=\"cw p20\">Confirm new PIN</div>";
    response += "<div class=\"cw\"><div class=\"cwl\"><div class=\"ctrlw\"><input type=\"password\" length=\"4\" name=\"cnewpin\" id=\"cnewpin\" class=\"i\" required /></div></div>";
    response += "<div class=\"cwr r\"><div class=\"cw\" style=\"height:35px;\"><div class=\"hidden\" id=\"e\">New PIN and Confirm new PIN does not match!</div></div></div></div>";
    response += "<div class=\"cw p20\"><div class=\"ctrlw\"><input type=\"button\" value=\"Save\" class=\"i p\" onclick=\"send();\" /></div></div></form></div>";
    response += "<script>function send(){var np = document.getElementById('newpin');var cnp=document.getElementById('cnewpin');if(np.value != cnp.value){document.getElementById('e').className = '';return false;}document.getElementById('pin').submit();}</script></body></html>";
  }
  else if (administrationType == 3) 
  {
    response += "<form method=\"get\" action=\"/nfc\"><div class=\"cw r\">NFC card administration</div><div class=\"cw p40\"><table><tr><th></th><th>Card Number</th></tr></table>";
    // TODO: build NFC card table
    response += "</div><div class=\"cw\"><div class=\"ctrlw\"><input type=\"submit\" value=\"Remove\" class=\"i p\" /></div></div><hr /><div class=\"cw p40\">NFC card number</div><div class=\"cw\"><div class=\"ctrlw\">";
    response += "<input type=\"text\" length=\"64\" name=\"nfc\" id=\"nfc\" class=\"i\" /></div></div><div class=\"cw p20\"><div class=\"ctrlw\"><input type=\"submit\" value=\"Save\" class=\"i p\" /></div></div></form></div></body></html>";
  }

  response += "</body></html>";

  return response;
}

// Build unknown request response
// Responses:
// 1. Unknown request
// ---------------------

String RequestHandler::BuildUnknownRequestReponse() {
  String response = RequestHandler::HtmlStart("Unknown request");
  response += "<div class=\"ch1 r\">Unknown request</div><div class=\"cw r\">Page does not exists</div><div class=\"cw p40\"><div class=\"ctrlw\"><a href=\"/\" class=\"i a\">Back</a></div></div></div></body></html>";
  
  return response;
}


/**
 * Private methods
 */

// Return start tags of HTML page
// ---------------------
String RequestHandler::HtmlStart(String title) 
{
  String start = RequestHandler::_start;
  start += "<html><head><title>" + title + "</title><meta charset=\"utf-8\" />";
  start += RequestHandler::_css;
  start += "</head><body><div class=\"c\">";

  return start;
}

// Decode URI
// ---------------------

void RequestHandler::DecodeURI(const char *src, char *dst)
{
  char a, b;
  while (*src) {
    if ((*src == '%') &&
      ((a = src[1]) && (b = src[2])) &&
      (isxdigit(a) && isxdigit(b))) {
      if (a >= 'a')
        a -= 'a'-'A';
      if (a >= 'A')
        a -= ('A' - 10);
      else
        a -= '0';
      if (b >= 'a')
        b -= 'a'-'A';
      if (b >= 'A')
        b -= ('A' - 10);
      else
        b -= '0';
      *dst++ = 16*a+b;
      src+=3;
    } 
    else {
      *dst++ = *src++;
    }
  }
  *dst++ = '\0';
}


