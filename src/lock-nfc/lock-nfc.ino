/**
   NFC I2C - PN532
*/

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <TrueRandom.h>

#include "Settings.h"

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

Settings _s;

void setupNFC() {
  Serial.begin(115200);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();
}

void setup() {
  setupNFC();
}

String printHex(byte number) {
  int topDigit = number >> 4;
  int bottomDigit = number & 0x0f;

  String top("0123456789ABCDEF"[topDigit]);
  String bottom("0123456789ABCDEF"[bottomDigit]);

  return top + bottom;
}

String getUuid(byte* uuidNumber) {
  String uid;
  for (int i = 0; i < 16; i++) {
    if (i == 4 || i == 6 || i == 8 || i == 10) { 
      uid += "-";
    }
    
    uid += printHex(uuidNumber[i]);
  }

  return uid;
}

void loopNFC() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ...
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      // Start with block 4 (the first block of sector 1) since sector 0
      // contains the manufacturer data and it's probably better just
      // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        
       
        // If you want to write something to block 4 to test with, uncomment
        // the following line and this text should be read back in a minute
        uint8_t data[36];
        
        
        byte uuidNumber[16];
        // Generate a new UUID
        TrueRandom.uuid(uuidNumber);

        String uid = getUuid(uuidNumber);
        Serial.print("The UUID number is ");
        Serial.print(uid);
        Serial.println();

        for (int i = 0; i < uid.length(); i++) {
          data[i] = uid[i];
        }

        success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 36);
          Serial.println("");

          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }

    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");

      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");

        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
}

void writeUid() {
  
}

void checkUid() {
  
}

boolean _init = false;
void loop() {

  if (!_init) {
    _s.Init();
    _init = true;

    /*
    strcpy(_s.Configuration.devices[0].deviceName, "device-name1");
    strcpy(_s.Configuration.devices[0].uid, "123-123-13");
    strcpy(_s.Configuration.devices[1].deviceName, "device-name2");
    strcpy(_s.Configuration.devices[1].uid, "223-123-13");
    strcpy(_s.Configuration.devices[2].deviceName, "device-name3");
    strcpy(_s.Configuration.devices[2].uid, "323-123-13");
    strcpy(_s.Configuration.devices[3].deviceName, "device-name4");
    strcpy(_s.Configuration.devices[3].uid, "423-123-13");
    _s.SaveSettings();*/
  }

  writeUid();
  checkUid();
  loopNFC();

  delay(3000);
}

