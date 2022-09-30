/*******************************************************************************
  P07a: Hola mundo sensor de ultrasonidos HC-SR04
  
  Este ejemplo permite leer la distancia obtenida por el sensor de ultrasonidos
  - Se inicializa la pantalla OLED
  - La placa lee la distancia del sensor HC-SR04 (ultrasonidos)
  - La distancia obtenida se imprime por serial y en la pantalla

  Conexiones:
    Sensor de temperatura     Placa Heltec
    VCC                       5V
    GND                       GND
    TRIG                      PIN 12
    ECHO                      PIN 13
  
 ******************************************************************************/

#include <Arduino.h>
#include <Ultrasonic.h>
#include <SSD1306Wire.h>

// Definimos el sensor con dos pines TRIGER y ECHO
Ultrasonic ultrasonic(12, 13);

// Definimos la pantalla OLED
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED); 

int distance;

void setup() {
  Serial.begin(115200);

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
  delay(3000);
}

void loop() {
  // Solicitamos la distancia al sensor
  distance = ultrasonic.read();
  
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Actualizamos pantalla
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(128/2, 0, "DISTANCIA");
  char text[8];
  sprintf(text, " %d cm", distance);
  display.drawString(128/2,40, text);
  display.display();


  delay(1000);
}