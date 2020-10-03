//Get library from https://github.com/ljos/MFRC522
//Sketch: https://github.com/guttih
//Modifications:https://github.com/wildeng

/*
Pins    SPI        UNO    Mega2560  Leonardo
1 (NSS) SDA (SS)   10     53        10
2       SCK        13     52        SCK1
3       MOSI       11     51        MOSI1
4       MISO       12     50        MISO1
5       IRQ        *      *         *
6       GND        GND     GND       GND
7       RST        9      ?         Reset
8      +3.3V (VCC) 3V3     3V3       3.3V
*/
#include <MFRC522.h>
#include <SPI.h>
#include "Keyboard.h"

#define SAD 10
#define RST 9

MFRC522 nfc(SAD, RST);

boolean Opening = false;
String pwd = "";

void setup() {
  SPI.begin();
  nfc.begin();
  byte version = nfc.getFirmwareVersion();
}

#define AUTHORIZED_COUNT 2 /*If you want more Authorized of cards set the count here, and then add the serials below*/

byte Authorized[AUTHORIZED_COUNT][4] = {
  {0xD0, 0x46, 0x54, 0x80, },
  {0x2A, 0xEF, 0x8D, 0xAB }
  };
                          
boolean isSame(byte *key, byte *serial);
boolean isAuthorized(byte *serial);

void loop() {
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  String tagCode;
    
  status = nfc.requestTag(MF1_REQIDL, data);


  if (status == MI_OK) {
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
  
    /* The purpose of this line is reading the RFID tag code
     * and print it on serial, so that it can be added to authorized ones
     */
    tagCode = getTag(serial);

    /*
     *  if RFID tag is authorized then the code is printed on serial port
     *  so that a custom Ruby script can read it and use it to get the password from 
     *  a secret YAML file
     */


   
    
    if(isAuthorized(serial))
    { 
      Opening = !Opening; // ON-OFF switch
      delay(200);

      // printing on serial

      Serial.println(tagCode);
      delay(500);

      // getting back password from ruby script
      pwd = Serial.readString();
      delay(500);

      // using the password to unlock the pc
      Keyboard.print(pwd);
      delay(500);
      Keyboard.press(KEY_RETURN);
      delay(200);
      Keyboard.releaseAll();
    }
    else
    { 
      Opening = false;
    }
    
    nfc.haltTag();
    delay(2000);
  }
  delay(500);
}

/*
 *  @author wildeng@hotmail.com
 *  this function converts a byte array in an 4 bytes HEX string representing it
 *  @param [byte*] pointer to tag byte array
 *  @return [String] HEX string representing the tag
 */
String getTag(byte* packet)
{
  uint8_t* bytes = (uint8_t*)packet;
  int i = 0;
  char hex[10];
  char hexstring[10] = "";
  
  
  for (i=0; i<4; i++){
      sprintf(hex, "%02X", bytes[i]);
      strcat(hexstring, hex);
      fflush(stdout);
  }
  return hexstring;
}

/*
 *  this functions checks if the read byte array is
 *  the same that we have in our authorized array byte by byte
 *  @param [byte*] key pointer to memorized byte array
 *  @param [byte*] serial pointer to the read byte array
 *  @return [boolean] 
 */
boolean isSame(byte *key, byte *serial)
{
    for (int i = 0; i < 4; i++)
    {
      if (key[i] != serial[i])
      { 
        return false; 
      }
    }
    return true;
}

/*
 *  this functions reads each key in authorized array and compares it to the
 *  read one
 *  @param [byte*] serial pointer to read byte array
 *  @return [boolean] 
 */
boolean isAuthorized(byte *serial)
{
    for(int i = 0; i < AUTHORIZED_COUNT; i++)
    {
      if(isSame(serial, Authorized[i]))
      return true;
    }
    return false;
}
