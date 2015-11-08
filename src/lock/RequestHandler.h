#ifndef RequestHandler_h
#define RequestHandler_h

#include "Arduino.h"
#include "Entities.h"

class RequestHandler {

  public:
  
    /**
    * Initializes a new instance of the RequestHandler class.
    */
    RequestHandler();

    /**
     * Handle client request and return action for next steps.
     * @param1: request
     * @param2: server initialize result
     */
    int ProcessRequest(String, int);

    /**
     * Get WiFi parameters from query string
     */
    Entities::WiFiParameters GetWiFiParameters();

    /**
     * Get unlock PIN
     */
    String GetUnlockPIN();

  private:

    /**
     * Decode URI
     * 
     * @param1: encoded char array
     * @param2: decoded char array
     */
    void DecodeURI(const char *, char *);

    /**
     * Query string field.
     */
    String _queryString;
};

#endif

