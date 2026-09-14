#include <WiFi.h>
#include <WebServer.h>

/*
===================================================================
                  BRANCHEMENT MATÉRIEL
===================================================================
- Relais VCC ---> Broche VIN / 5V de l'ESP32
- Relais GND ---> Broche GND de l'ESP32
- Relais IN  ---> Broche GPIO 23 de l'ESP32
===================================================================
*/

// --- Paramètres du réseau Wi-Fi autonome ---
const char* ssid = "ESP32_Relais_Chamss";
const char* password = "12345678password";

// Broche connectée au signal du relais
const int relayPin = 23;

WebServer server(80);

String relayState = "Eteint";

// ===================================================================
//             PAGE HTML & CSS ULTRA SIMPLE
// ===================================================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Commande Relais ESP32</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
            background-color: #f0f0f0;
        }
        .btn {
            display: inline-block;
            padding: 15px 30px;
            font-size: 18px;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            margin: 10px;
        }
        .on { background-color: green; }
        .off { background-color: red; }
    </style>
</head>
<body>

    <h1>Commande Relais ESP32</h1>
    <p>Etat du relais : <b>%STATE%</b></p>
    
    <p>
        <a href="/on" class="btn on">ALLUMER LE RELAIS</a>
        <a href="/off" class="btn off">ETEINDRE LE RELAIS</a>
    </p>

</body>
</html>
)rawliteral";

// ===================================================================
//                      FONCTIONS DU SERVEUR
// ===================================================================

void handleRoot() {
  String page = index_html;
  page.replace("%STATE%", relayState);
  server.send(200, "text/html", page);
}

// Activer le relais
void handleRelayOn() {
  // Remarque : Si le relais s'active au niveau BAS (Active LOW), mettre LOW ici
  digitalWrite(relayPin, LOW);  // Active la bobine du relais
  relayState = "Allumé (Fermé)";
  
  server.sendHeader("Location", "/");
  server.send(303);
}

// Désactiver le relais
void handleRelayOff() {
  digitalWrite(relayPin, HIGH); // Désactive la bobine du relais
  relayState = "Éteint (Ouvert)";
  
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  // État initial : relais désactivé au démarrage
  digitalWrite(relayPin, HIGH); 

  // Démarrage du réseau Wi-Fi de l'ESP32
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Point d'accès démarré ! Adresse IP : ");
  Serial.println(myIP);

  // Configuration des routes
  server.on("/", handleRoot);
  server.on("/on", handleRelayOn);
  server.on("/off", handleRelayOff);

  server.begin();
}

void loop() {
  server.handleClient();
}
