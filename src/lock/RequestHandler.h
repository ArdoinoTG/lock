#ifndef RequestHandler_h
#define RequestHandler_h

#include "Arduino.h"
#include "Entities.h"
#include <vector>

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
    int ProcessRequest(String&, int);

    /**
     * Get WiFi parameters from query string
     */
    Entities::WiFiParameters GetWiFiParameters();

    /**
     * Get unlock PIN
     */
    String GetUnlockPIN();

    /**
     * Get new PIN parameters
     */
    Entities::PINParameters GetPINParameters();

    /**
     * Get NFC parameters
     */
    Entities::NFCParameters GetNFCParameters();

    //
    // -------------------------------------
    // RESPONSES

    /**
     * Build main response
     * 
     * Responses:
     * 1. Main
     * 2. Main - Incorrect PIN
     * 3. Main - Unlocking
     * 
     * @param1: unlock
     * @param2: incorrect pin
     */
    String BuildMainResponse(boolean, boolean);

    /**
     * Build available WiFi netwroks response
     * 
     * Reponses:
     * 1. Register
     * 
     * @param1 - list of available wifi networks
     */
    String BuildAvailableWiFiReponse(std::vector<Entities::WiFiNetwork>&);

    /**
     * Build registered to WiFi network response
     * 
     * Responses:
     * 1. Registering to WiFi - TODO: find out why this is not returned to user
     */
    String BuildRegisteredToWiFiNetworkResponse();

    /**
     * Build administration response
     * 
     * Responses:
     * 1. Administration
     * 2. Administration - Set new PIN
     * 3. Administration - Incorrect PIN
     * 4. Administration - NFC TODO: Card detecting
     * 
     * Administration Type
     * 1. Main administration page
     * 2. Set PIN
     * 
     * @param1: administation type
     * @param2: error [incorrect (old) PIN]
     */
    String BuildAdministrationResponse(int, boolean);

    /**
     * Build NFC administration response
     * 
     * Responses:
     * 1. Administration - NFC
     * 
     * @param1: error [0: no errors][1: device name is empty][2: device name already exists][3: something went wrong. please try again]
     * @param2: registered NFC devices
     */
    String BuildNFCAdministrationResponse(int, Entities::NFC[10]);

    /**
     * Build unknown request response
     * 
     * Responses:
     * 1. Unknown request
     */
    String BuildUnknownRequestReponse();

    /**
     * Build OK response
     */
    String BuildOK();

    /**
     * CSS
     */
    const char *_css = "html,body{font-size:20px;margin:0}.c{border-left:2px solid #cccccc;border-right:2px solid #cccccc;background-color:#fdfdff;height:100%;margin:0 auto;padding:0 12px;width:700px}.ch1{font-size:34px}.ch2{font-size:28px}.cw{padding:4px 0;width:100%}.r{color:#FF0000}.p20{padding:20px 0 0 0}.p40{padding:40px 0 0 0}.cwl{float:left;width:300px}.cwr{margin-left:350px}.ctrlw{width:250px}.a{display:block;text-align:center;text-decoration:none;color:#000000;background-color:#F0F0F0;font-size:20px !important}.i{border:2px solid #cccccc;border-radius:6px;font-size:18px;padding:8px;width:100%}.p{cursor:pointer}.f18{font-size:16px}table{border-collapse:collapse;min-width:250px;}th,td{border-bottom:1px solid #cccccc;text-align:right;padding:4px;}.hidden{visibility:hidden}";

  private:

    /**
     * Return start tags of HTML page
     * 
     * @param1: title name
     */
    String HtmlStart(String);

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

    /**
     * Begin of the response
     */
    const char *_start = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";//<!DOCTYPE HTML>\r\n;
};

#endif

