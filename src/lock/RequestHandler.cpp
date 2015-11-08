#include "Arduino.h"
#include "RequestHandler.h"
#include "Entities.h"

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

int RequestHandler::ProcessRequest(String request, int serverResult) {

  int addr_start = request.indexOf(' ');
  int addr_end = request.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    return Entities::UnknownRequest;
  }

  String req = request.substring(addr_start + 1, addr_end);
  Serial.print("Parsed request: ");
  Serial.println(req);

  if (req == "/") {
    if (serverResult == Entities::ConnectedToWiFi) {
      return Entities::Welcome;  
    }
    else if (serverResult == Entities::AccessPointCreated) {
      return Entities::WelcomeWithRegisterLink;
    }
  }
  else if (req == "/register") {
    return Entities::Register;
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


