/*******************************************************************************
  E04: Ejemplo de LoRaWAN con OTAA y sensor de puerta
  
  Este ejemplo permite enviar uplinks y recibir downlinks con activacion por OTAA
  - La placa lee continuamente el estado del sensor de puerta
  - En el momento en que el estado cambia se envia un uplink
  - Tras cada uplink se escucha por si llega un downlink
  - Si llega un downlink se actualiza el estado del led
      - Puerta bloqueada (LED rojo)
      - Puerta desbloqueada (LED verde)

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes

  Conexiones:
    Sensor de puerta    Placa Heltec
    terminal 1          GND
    terminal 2          PIN 13

    LED                 Placa Heltec
    GND                 GND
    R                   PIN 23
    G                   PIN 22

    # PAYLOAD FORMATER para TTN #

    function Decoder(bytes, port) {
      // Decode plain text; for testing only 
      return {
        estadoPuerta: String.fromCharCode.apply(null, bytes)
      };
    }
  
 ******************************************************************************/
#include <Arduino.h>
#include <TTN_esp32.h>

const char* devEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Device EUI de TTN
const char* appEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Application EUI de TTN
const char* appKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Application Key de TTN

TTN_esp32 ttn;
#define DOOR_SENSOR 13
#define LED_RED 23
#define LED_GREEN 22

#define DOOR_CLOSED LOW
#define DOOR_OPEN HIGH

int lastStatus = DOOR_OPEN;

void message(const uint8_t* payload, size_t size, int rssi)
{
    Serial.println("-- NUEVO DOWNLINK");
    Serial.println("Recibidos " + String(size) + " bytes RSSI=" + String(rssi) + "db");
    
    if (payload[0] == 0)
    {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        Serial.println("Puerta desbloqueada");
    }
    else
    {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
        Serial.println("Puerta bloqueada");
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando...");

    pinMode(DOOR_SENSOR, INPUT_PULLUP);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);

    ttn.begin();
    ttn.onMessage(message); // declaramos la función de callback para manejar los downlinks desde el servidor
    ttn.join(devEui, appEui, appKey);
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
    int status = digitalRead(DOOR_SENSOR);

    if (status != lastStatus)
    {
        lastStatus = status;

        if (status == DOOR_CLOSED)
        {
            Serial.println("Enviando puerta CERRADA");
            uint8_t* payload = (uint8_t*) "CERRADA";
            ttn.sendBytes(payload, 7);
        }
        else 
        {
            Serial.println("Enviando puerta ABIERTA");
            uint8_t* payload = (uint8_t*) "ABIERTA";
            ttn.sendBytes(payload, 7);
        }

        delay(10000);
    }
}
