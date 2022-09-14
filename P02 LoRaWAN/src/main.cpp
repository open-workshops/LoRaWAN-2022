/***************************************************************************
  P02: Práctica de LoRaWAN Hola Mundo
  
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

// claves ABP
const char *devAddr = "XXXXXXX";                          // Rellena con el Device Address de TTN
const char *nwkSKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Network Session Key de TTN
const char *appSKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Application Session Key de TTN

TTN_esp32 ttn;

void setup()
{
  Serial.begin(115200);
  Serial.println("Iniciando");

  ttn.begin();
  ttn.personalize(devAddr, nwkSKey, appSKey); // ABP

  // Codigo para OTAA
  //while (!ttn.isJoined())
  //{
  //    Serial.print(".");
  //    delay(500);
  //}
  //Serial.println("\njoined!");

  ttn.showStatus();
}

void loop()
{
  uint8_t *payload = (uint8_t *)"hola ABP";
  ttn.sendBytes(payload, 8);
  
  Serial.println("Enviando uplink...");
  delay(15000);
}












void handleDownlink(const uint8_t* payload, size_t size, int rssi)
{
    Serial.println("-- NUEVO DOWNLINK");
    Serial.println("Recibidos " + String(size) + " bytes RSSI=" + String(rssi) + "db");

    Serial.println(payload[0]);
}