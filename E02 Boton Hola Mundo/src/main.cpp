/*******************************************************************************
  P02B: Práctica botón y pantalla OLED
  
  Este ejemplo permite comprobar el funcionamiento de los botones y pantalla.
  -Pulsando un boton externo se muestra en pantalla.
  -Pulsando el botón PROG se muestra en pantalla.

  Conexiones:
  Botón           Placa Heltec LoRa32
  Pin -     ----- Pin GND
  Pin medio ----- Pin 3V3
  Pin S     ----- Pin 23
*******************************************************************************/
#include <Arduino.h>
#include <SSD1306Wire.h>

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED); 

void setup() {
  Serial.begin(115200);

  // Configuramos el pin 23 como entrada para el boton
  #define BUTTON_PIN 23
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Configuramos el boton integrado en la placa como modificador del SF
  pinMode(KEY_BUILTIN, INPUT_PULLUP);

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
  delay(1000);

  // Actualizamos pantalla
  display.clear();
  display.drawString(128/2, 64/2-16/2, "Esperando...");
  display.display();
}

void loop() {

  if (!digitalRead(BUTTON_PIN)) 
  {
    Serial.println("Boton externo pulsado!");
    // Actualizamos pantalla
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(128/2, 64/2-16/2, "Boton externo pulsado!");
    display.display();

    delay(3000); // esperamos un poco para evitar rebotes del botón y ver pantalla

    // Actualizamos pantalla
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(128/2, 64/2-16/2, "Esperando...");
    display.display();
  }

  if (!digitalRead(KEY_BUILTIN))
  {
    // Actualizamos pantalla
    Serial.println("Boton integrado pulsado!");
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(128/2, 64/2-16/2, "Boton integrado pulsado!");
    display.display();

    delay(3000); // esperamos un poco para evitar rebotes del boton y ver pantalla

    // Actualizamos pantalla
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(128/2, 64/2-16/2, "Esperando...");
    display.display();
  }
}