/*******************************************************************************
  E07: Ejemplo de LoRaWAN con OTAA, sensor SHT30 y OLED
  
  Este ejemplo permite enviar datos de un sensor con activacion por OTAA
  - Inicalmente se hace un JOIN por OTAA a una red LoRaWAN
  - La placa lee el estado de un sensor SHT30 (temperatura y humedad)
  - Los valores de temperatura y humedad se imprimen por serial
  - Se formatean los valores anteriores en un array para enviarlos por red
  - Se realiza el uplink por LoRaWAN

  Crea un dispositivo en TTN y rellena los campos de abajo con
  las claves correspondientes

  Conexiones:
    Sensor de temperatura     Placa Heltec
    VIN                       3.3
    GND                       GND
    SCL                       PIN 15
    SDA                       PIN 4

    # PAYLOAD FORMATER para TTN #

    function Decoder(bytes, port) {
      return {
        temperature: bytes[0] / 5.0 - 12.0,
        humidity: bytes[1]
      };
    }
  
 ******************************************************************************/

#include <Arduino.h>
#include "SHT85.h"
#include <TTN_esp32.h>
#include <SSD1306Wire.h>

const char* devEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Device EUI de TTN
const char* appEui = "XXXXXXXXXXXXXXXX"; // Rellena con el Application EUI de TTN
const char* appKey = "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Rellena con la Application Key de TTN

TTN_esp32 ttn;
SHT30 sht;
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED); 

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Reseteamos la OLED
  pinMode(RST_OLED,OUTPUT);
  digitalWrite(RST_OLED, LOW);     
  delay(50);
  digitalWrite(RST_OLED, HIGH);   

  // Iniciamos OLED
  Serial.println("Iniciando pantalla...");
  display.init();
  display.flipScreenVertically();
  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(128/2, 64/2-16/2, "Iniciando...");
  display.display();

  // Inicialización del sensor SHT30
  // Importante: Esta instrucción debe estar depués de display.init(),
  // ya que ambos usan i2c y display ha sido declarado con los pines
  // correctos en el constructor.
  sht.begin(0x44);

  delay(3000);

  // Iniciamos LoRaWAN
  ttn.begin();
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

void loop() {
  if (!sht.read())  // solicitamos medición de temperatura y humedad al sensor
  {
    // si ha habido un error, no continuamos
    Serial.print("Error leyendo el sensor: ");
    Serial.println(sht.getError());
    delay(10000);
  }

  Serial.print("Temperatura: ");
  Serial.print(sht.getTemperature());
  Serial.println("º C");

  Serial.print("Humedad: ");
  Serial.print(sht.getHumidity());
  Serial.println(" %");
  
  Serial.println();

  // Enviamos por LoRaWAN
  uint8_t payload[2];
  payload[0] = (uint8_t) ((sht.getTemperature() + 12) * 5);
  payload[1] = (uint8_t) sht.getHumidity();

  ttn.sendBytes(payload, 2);

  // Actualizamos pantalla
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "  TEMP     HUM");
  char text[32];
  sprintf(text, " %.1f°C    %.1f%%", sht.getTemperature(), sht.getHumidity());
  display.drawString(0,40, text);
  display.display();

  delay(30000);
}
