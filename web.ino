#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

/*
===================================================================
                  RÉCAPITULATIF DES BRANCHEMENTS
===================================================================

1. Capteur DHT11 :
   - VCC  ---> Broche 3.3V (ou VIN / 5V)
   - GND  ---> Broche GND
   - DATA ---> Broche GPIO 4

2. Écran LCD 16x2 I2C :
   - VCC  ---> Broche VIN / 5V
   - GND  ---> Broche GND
   - SDA  ---> Broche GPIO 21
   - SCL  ---> Broche GPIO 22

===================================================================
*/

// --- Identifiants Wi-Fi ---
const char* ssid = "VOTRE_NOM_DE_RESEAU_WIFI";
const char* password = "VOTRE_MOT_DE_PASSE_WIFI";

// --- Configuration Matérielle ---
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer server(80);

// ===================================================================
//          PAGE HTML & CSS PURS (Structure HTML + Style CSS)
// ===================================================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="refresh" content="5">
    <title>Station Météo ESP32</title>
    
    <style>
        /* Style global de la page */
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #f0f2f5;
            margin: 0;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 90vh;
        }

        /* Carte principale */
        .card {
            background-color: #ffffff;
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
            text-align: center;
            width: 100%;
            max-width: 350px;
        }

        h1 {
            color: #333333;
            font-size: 22px;
            margin-bottom: 20px;
            border-bottom: 2px solid #eee;
            padding-bottom: 10px;
        }

        /* Blocs d'affichage des mesures */
        .box {
            margin: 15px 0;
            padding: 15px;
            border-radius: 10px;
            font-size: 18px;
            font-weight: bold;
        }

        .temp-box {
            background-color: #ffebe6;
            color: #d9381e;
            border-left: 5px solid #d9381e;
        }

        .hum-box {
            background-color: #e6f4ff;
            color: #0073e6;
            border-left: 5px solid #0073e6;
        }

        .value {
            font-size: 26px;
            display: block;
            margin-top: 5px;
        }
    </style>
</head>
<body>

    <div class="card">
        <h1>🌡️ Station Météo IoT</h1>
        
        <div class="box temp-box">
            Température
            <span class="value">%TEMP% °C</span>
        </div>

        <div class="box hum-box">
            Humidité
            <span class="value">%HUM% %</span>
        </div>
    </div>

</body>
</html>
)rawliteral";

// ===================================================================
//                      FONCTIONS DU SERVEUR
// ===================================================================

void handleRoot() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  String page = index_html;

  if (isnan(temp) || isnan(hum)) {
    page.replace("%TEMP%", "--");
    page.replace("%HUM%", "--");
  } else {
    page.replace("%TEMP%", String(temp, 1));
    page.replace("%HUM%", String(hum, 1));
  }

  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);

  dht.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Connexion WiFi..");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté !");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP Serveur:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
