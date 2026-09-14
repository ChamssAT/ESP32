#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int ledPin = 23; // LED connectée sur le GPIO 23 (ou 2 pour la LED intégrée)

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // LED éteinte au démarrage
  
  // Nom du réseau Bluetooth recherché par le téléphone
  SerialBT.begin("ESP32_LED"); 
}

void loop() {
  // Si le téléphone envoie une donnée via Bluetooth
  if (SerialBT.available()) {
    char recu = SerialBT.read();

    if (recu == '1') {
      digitalWrite(ledPin, HIGH);          // Allume la LED
      SerialBT.println("LED Allumée");    // Envoie la confirmation au smartphone
    } 
    else if (recu == '0') {
      digitalWrite(ledPin, LOW);           // Éteint la LED
      SerialBT.println("LED Éteinte");    // Envoie la confirmation au smartphone
    }
  }
}
