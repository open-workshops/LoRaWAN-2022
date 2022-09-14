/*******************************************************************************
  E01: Ejemplo Hola Mundo pantalla OLED
  
  Este ejemplo permite comprobar el funcionamiento de la pantalla.
  -En la pantalla se muestra el texto Hola Mundo
 ******************************************************************************/
#include <Arduino.h>
#include "SSD1306Wire.h" 

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED); 

void setup() {
  Serial.begin(115200);

  // Reseteamos la OLED
  pinMode(RST_OLED,OUTPUT);
  digitalWrite(RST_OLED, LOW);     
  delay(50);
  digitalWrite(RST_OLED, HIGH);   

  Serial.println("Iniciando pantalla...");
  display.init();
  display.flipScreenVertically();

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  // La pantalla mide 128 de ancho por 64 de alto
  // El origen está en la parte superior izq
  // centramos primero horizontalmente dividiendo el ancho a la mitad
  // centramos verticalmente dividiendo el alto a la mitad
  // y restando la mitad del tamaño de la letra
  display.drawString(128/2, 64/2-16/2, "Hola Mundo");
  display.display();
}

void loop() {
  
}
