/*******************************************************************************
  P02C: Práctica de LoRa P2P con botón y pantalla OLED
  
  Este ejemplo permite recibir y enviar tramas LoRa utilizando boton y pantalla
  -Pulsando un botón externo se envía una trama LoRa.
  -Pulsando el botón PROG de la placa Heltec se incrementa el Spreading Factor.
  -La placa está siempre escuchando por una posible trama LoRa.
  -En la pantalla se proporciona información.

  Conexiones:
  Botón           Placa Heltec LoRa32
  Pin -     ----- Pin GND
  Pin medio ----- Pin 3V3
  Pin S     ----- Pin 23
*******************************************************************************/
#include <Arduino.h>
#include <RadioLib.h>
#include <SSD1306Wire.h>

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED); 
SX1276 radio = new Module(SS, DIO0, RST_LoRa, DIO1);

int currentSF = 7;

// flag para indicar que el paquete se ha recibido
volatile bool receivedFlag = false;

// flag para desactivar la interrupción cuando no se necesita
volatile bool enableInterrupt = true;

// Función manejadora de la interrupción
// Esta funcion deber ser void y no debe contener delays ni prints!
void setFlag(void) {
  if(!enableInterrupt) 
  {
    return;
  }
  receivedFlag = true;
}

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

  // Iniciamos radio
  int state = radio.begin(868.0, 125, currentSF);

  if (state == ERR_NONE)
  {
    Serial.println("OK!");
  } 
  else
  {
    Serial.print("Error, codigo: ");
    Serial.println(state);
    while (true);
  }

  // asignamos la funcion que se llamará al recibir un paquete
  radio.setDio0Action(setFlag);

  // Nos ponemos a escuchar asíncronamente
  Serial.println("Escuchando a la espera de una transmisión... ");
  state = radio.startReceive();
  if (state != ERR_NONE) {
    Serial.print("failed, code ");
    Serial.println(state);

    // Actualizamos pantalla
    display.clear();
    display.drawString(128/2, 64/2-16/2, "Error de LoRa...");
    display.display();

    while (true);
  }

  // Actualizamos pantalla
  display.clear();
  display.drawString(128/2, 64/2-16/2, "Escuchando...");
  display.display();
}

void loop() {
  if(receivedFlag) 
  {
    // desactivamos la interrupción mientras procesamos los datos
    enableInterrupt = false;

    // reseteamos el flag
    receivedFlag = false;

    String str;
    int state = radio.readData(str);

    if (state == ERR_NONE)
    {
      Serial.println("Paquete recibido!");

      Serial.print("Data:\t\t\t");
      Serial.println(str);

      Serial.print("RSSI:\t\t\t");
      Serial.print(radio.getRSSI());
      Serial.println(" dBm");

      Serial.print("SNR:\t\t\t");
      Serial.print(radio.getSNR());
      Serial.println(" dB");

      // Actualizamos pantalla
      display.clear();
      display.drawString(128/2, 64/2-16/2, "Paquete recibido!");
      display.setFont(ArialMT_Plain_10);
      display.drawString(128/2, 64/2+10, str);
      display.setFont(ArialMT_Plain_16);
      display.display();
      delay(4000); // esperamos para que se lea la pantalla
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(128/2, 64/2-16/2, "Escuchando...");
      display.display();
    } else 
    {
      Serial.print("Error, codigo: ");
      Serial.println(state);
    }

    // volvemos a congigurar el transceptor para recepción
    radio.startReceive();

    // activamos las interrupciones nuevamente
    enableInterrupt = true;
  }

  if (!digitalRead(BUTTON_PIN)) 
  {
    enableInterrupt = false;

    String datos = "ESCRIBE AQUI TU NOMBRE";
    int state = radio.transmit(datos);

    if (state == ERR_NONE)
    {
      Serial.println(" ENVIADO!");

      // Actualizamos pantalla
      display.clear();
      display.drawString(128/2, 64/2-16/2, "Enviando...");
      display.display();
    }
    else
    {
      Serial.print("Error, codigo: ");
      Serial.println(state);
    }

    radio.startReceive();
    enableInterrupt = true;
    
    delay(1000); // esperamos un poco para evitar rebotes del boton

    // Actualizamos pantalla
    display.clear();
    display.drawString(128/2, 64/2-16/2, "Escuchando...");
    display.display();
  }

  if (!digitalRead(KEY_BUILTIN))
  {
    if (++currentSF > 12)
      currentSF = 7;

    radio.setSpreadingFactor(currentSF);
    radio.startReceive();

    Serial.print("Cambiando a SF ");
    Serial.println(currentSF);

    // Actualizamos la pantalla
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(128/2, 64/2-16/2, String("SF: ") + currentSF);
    display.display();
    delay(3000); // esperamos para que se lea la pantalla
    display.clear();
    display.drawString(128/2, 64/2-16/2, "Escuchando...");
    display.display();
  }
}