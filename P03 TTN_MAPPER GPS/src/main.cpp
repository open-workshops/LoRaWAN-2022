/*******************************************************************************
  P03: Práctica TTN MAPPER GPS
  
  Este ejemplo mapea la cobertura de la red TTN utilizando LoRaWAN y GPS.
  -La placa está siempre leyendo el estado del GPS.
  -En la pantalla se proporciona información en cuanto está disponible.
  -Mientras el GPS no obtiene fix mantiene su led encendido.
  -Cuando el GPS recibe fix correcto, hace parpadear su led.
  -Utiliza tipo de sesión ABP y SF7

  Conexiones:
  GPS       Placa Heltec LoRa32
  VCC ----- Pin 3V3
  GND ----- Pin GND
  TX  ----- PIN 13
  RX  ----- PIN 12
  PPS No conectar

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes

  # PAYLOAD FORMATER para TTN #

  function Decoder(bytes, port) {
    var decoded = {};

    decoded.latitude = ((bytes[0]<<16)>>>0) + ((bytes[1]<<8)>>>0) + bytes[2];
    decoded.latitude = (decoded.latitude / 16777215.0 * 180) - 90;
  
    decoded.longitude = ((bytes[3]<<16)>>>0) + ((bytes[4]<<8)>>>0) + bytes[5];
    decoded.longitude = (decoded.longitude / 16777215.0 * 360) - 180;
  
    var altValue = ((bytes[6]<<8)>>>0) + bytes[7];
    var sign = bytes[6] & (1 << 7);
    if(sign)
    {
        decoded.altitude = 0xFFFF0000 | altValue;
    }
    else
    {
        decoded.altitude = altValue;
    }
  
    decoded.hdop = bytes[8] / 10.0;

    return decoded;
  }

*******************************************************************************/
#include <Arduino.h>
#include <TTN_esp32.h> //https://github.com/rgot-org/TheThingsNetwork_esp32
#include "gps.h"
#include "oled.h"

const char *devAddr = "XXXXXXX";                          // Rellena con el Device Address de TTN
const char *nwkSKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Network Session Key de TTN
const char *appSKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Application Session Key de TTN

TTN_esp32 ttn;
uint8_t txBuffer[9];

void setup()
{
  Serial.begin(115200);
  setupOLED();
  setupGPS();
  Serial.println("Iniciando");
  ttn.begin();
  ttn.personalize(devAddr, nwkSKey, appSKey);
  ttn.setDataRate(5); //configura DataRate a 5 (SF7)
  ttn.showStatus();
}

void loop()
{
  smartGPSdelay(5000);
  if (checkGPSFix())
  {
    buildGPSpacket(txBuffer);
    Serial.println("Enviando paquete LoRaWAN");
    ttn.sendBytes(txBuffer, sizeof(txBuffer));
  }
  printGPSdata();
}