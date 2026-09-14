#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

/*
===================================================================
                  RÉCAPITULATIF DES BRANCHEMENTS
===================================================================

1. Capteur DHT11 :
   - VCC  ---> Broche 3.3V (ou VIN / 5V pour plus de stabilité)
   - GND  ---> Broche GND de l'ESP32
   - DATA ---> Broche GPIO 4 de l'ESP32

2. Écran LCD 16x2 avec module I2C :
   - VCC  ---> Broche VIN / 5V (Nécessaire pour le rétroéclairage)
   - GND  ---> Broche GND de l'ESP32
   - SDA  ---> Broche GPIO 21 (Bus I2C par défaut)
   - SCL  ---> Broche GPIO 22 (Bus I2C par défaut)

===================================================================
*/

// --- Configuration du capteur DHT11 ---
#define DHTPIN 4       // Broche GPIO 4 reliée à la patte DATA du DHT11
#define DHTTYPE DHT11  // Modèle du capteur (DHT11)

DHT dht(DHTPIN, DHTTYPE);

// --- Configuration de l'écran LCD I2C ---
// Adresse I2C : 0x27 (ou 0x3F selon le module), 16 colonnes, 2 lignes
// Utilise automatiquement les broches I2C par défaut : GPIO 21 (SDA) et GPIO 22 (SCL)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);

  // Initialisation du capteur DHT11 (sur GPIO 4)
  dht.begin();

  // Initialisation du bus I2C (GPIO 21 & GPIO 22) et du LCD
  lcd.init();
  lcd.backlight(); // Allume le rétroéclairage de l'écran
  
  // Message d'accueil au démarrage
  lcd.setCursor(0, 0);
  lcd.print("Station Météo");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation..");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Pause de 2 secondes entre deux mesures (limite physique du DHT11)
  delay(2000);

  // Lecture des valeurs
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Vérification de la validité de la lecture
  if (isnan(h) || isnan(t)) {
    Serial.println("Erreur de lecture du DHT11 sur le GPIO 4 !");
    lcd.setCursor(0, 0);
    lcd.print("Erreur Capteur  ");
    return;
  }

  // Affichage sur le moniteur série (USB)
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(" °C | Hum: ");
  Serial.print(h);
  Serial.println(" %");

  // Affichage sur l'écran LCD
  lcd.setCursor(0, 0); // Ligne 1
  lcd.print("Temp: ");
  lcd.print(t, 1);     // Affichage avec 1 décimale
  lcd.print((char)223); // Caractère pour le symbole degré (°)
  lcd.print("C   ");

  lcd.setCursor(0, 1); // Ligne 2
  lcd.print("Hum:  ");
  lcd.print(h, 1);
  lcd.print("%   ");
}
