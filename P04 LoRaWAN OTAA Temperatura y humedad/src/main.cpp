/***************************************************************************
  P04: Práctica de LoRaWAN y Sensor de temperatura y humedad
  
  Este ejemplo permite enviar datos de un sensor con activacion por OTAA
  - Inicalmente se hace un JOIN por OTAA a una red LoRaWAN
  - La placa lee el estado de un sensor SHT30 (temperatura y humedad)
  - Los valores de temperatura y humedad se imprimen por serial
  - Se formatean los valores anteriores en un array para enviarlos por red
  - Se realiza el uplink por LoRaWAN

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes.
  
  Los siguientes payload formatters se corresponden con las diferentes
  formas de enviar los payloads comentadas en la parte final del loop

    # PAYLOAD FORMATER para TEXTO #

    function Decoder(bytes, port) {
      // Decode plain text; for testing only 
      return {
        myTestValue: String.fromCharCode.apply(null, bytes)
      };
    }


    # PAYLOAD FORMATER para NUMEROS ENTEROS #

    function Decoder(bytes, port) {
      // Decode plain text; for testing only 
      return {
        temperature: bytes[0],
        humidity: bytes[1]
      };
    }


    # PAYLOAD FORMATER para PAYLOAD ESCALADO #

    function Decoder(bytes, port) {
      return {
        temperature: bytes[0] / 5.0 - 12.0,
        humidity: bytes[1]
      };
    }

 ****************************************************************************/
#include <Arduino.h>
#include <TTN_esp32.h>
#include "SHT85.h"

// claves OTAA
const char* devEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Device EUI de TTN
const char* appEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Application EUI de TTN
const char* appKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Application Key de TTN

TTN_esp32 ttn;
SHT30 sht;

int SDA_PIN = 4;
int SCL_PIN = 15;

void handleDownlink(const uint8_t* payload, size_t size, int rssi)
{
    Serial.println("-- NUEVO DOWNLINK");
    Serial.println("Recibidos " + String(size) + " bytes RSSI=" + String(rssi) + "db");

    // Imprimimos todos los bytes del downlink
    for (int i = 0; i < size; i++)
    {
        Serial.print(payload[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando...");

    // Inicialización del sensor SHT30
    Wire.begin(SDA_PIN, SCL_PIN, 100000U);
    sht.begin(0x44);

    ttn.begin();
    ttn.join(devEui, appEui, appKey); // OTAA
    ttn.onMessage(handleDownlink);

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
  if (!sht.read())  // solicitamos medición de temperatura y humedad al sensor
  {
    // si ha habido un error, no continuamos
    Serial.print("Error leyendo el sensor: ");
    Serial.println(sht.getError());
    delay(10000);
  }
  
  // Texto: 22.56C 56.21%
  // String payloadSrt = String(sht.getTemperature(), 2) + "C " + String(sht.getHumidity(), 2) + "%";
  // Serial.print(payloadSrt);
  // uint8_t *payload = (uint8_t *) payloadSrt.c_str();
  // ttn.sendBytes(payload, 13);

  // Numeros enteros: [22,56]
  // uint8_t payload[2];
  // payload[0] = sht.getTemperature();
  // payload[1] = sht.getHumidity();
  // Serial.print(payload[0]); 
  // Serial.print(" "); 
  // Serial.print(payload[1]);
  // ttn.sendBytes(payload, 2);

  // Payload escalado
  uint8_t payload[2];
  payload[0] = (uint8_t) ((sht.getTemperature() + 12) * 5);
  payload[1] = (uint8_t) sht.getHumidity();
  Serial.print(payload[0]); 
  Serial.print(" "); 
  Serial.print(payload[1]);
  ttn.sendBytes(payload, 2);
  
  Serial.println(" Enviando uplink...");
  delay(15000);
}
