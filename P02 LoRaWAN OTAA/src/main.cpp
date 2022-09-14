/***************************************************************************
  P02: Práctica de LoRaWAN Hola Mundo (Con modificaciones para OTAA)
  
  Este ejemplo permite enviar uplinks
  - Cada 15 segundos se envía un uplink LoRaWAN con un hola mundo

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes

    # PAYLOAD FORMATER para TTN #

    function Decoder(bytes, port) {
      // Decode plain text; for testing only 
      return {
        myTestValue: String.fromCharCode.apply(null, bytes)
      };
    }

 ****************************************************************************/
#include <Arduino.h>
#include <TTN_esp32.h>

// claves OTAA
const char* devEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Device EUI de TTN
const char* appEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Application EUI de TTN
const char* appKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Application Key de TTN

TTN_esp32 ttn;

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando...");

    ttn.begin();
    ttn.join(devEui, appEui, appKey); // OTAA

    Serial.print("TTN Join");
    while (!ttn.isJoined())
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\njoined!");

    ttn.showStatus();
}

void loop()
{
  uint8_t *payload = (uint8_t *)"hola OTAA";
  ttn.sendBytes(payload, 9);
  
  Serial.println("Enviando uplink...");
  delay(15000);
}