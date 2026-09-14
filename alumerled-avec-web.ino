#include <WiFi.h>
#include <WebServer.h>

/*
===================================================================
                  BRANCHEMENT MATÉRIEL
===================================================================
- LED Anode (+)  ---> Résistance 220Ω ---> GPIO 23
- LED Cathode (-)---> GND de l'ESP32
===================================================================
*/

// --- Paramètres du réseau Wi-Fi créé par l'ESP32 ---
const char* ssid = "ESP32_Meteo_Chamss"; // Nom du réseau Wi-Fi
const char* password = "12345678password"; // Mot de passe (minimum 8 caractères)

const int ledPin = 23;
WebServer server(80);

String ledState = "Eteinte";

// ===================================================================
//             PAGE HTML & CSS ULTRA SIMPLE
// ===================================================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>ESP32 LED AP</title>
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

    <h1>Réseau Autonome ESP32</h1>
    <p>Etat actuel : <b>%STATE%</b></p>
    
    <p>
        <a href="/on" class="btn on">ALLUMER</a>
        <a href="/off" class="btn off">ETEINDRE</a>
    </p>

</body>
</html>
)rawliteral";

// ===================================================================
//                      FONCTIONS DU SERVEUR
// ===================================================================

void handleRoot() {
  String page = index_html;
  page.replace("%STATE%", ledState);
  server.send(200, "text/html", page);
}

void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  ledState = "Allumee";
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLedOff() {
  digitalWrite(ledPin, LOW);
  ledState = "Eteinte";
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Configuration de l'ESP32 en mode Point d'Accès (AP)
  Serial.println("Création du réseau Wi-Fi...");
  WiFi.softAP(ssid, password);

  // En mode Access Point, l'adresse IP par défaut de l'ESP32 est TOUJOURS 192.168.4.1
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Point d'accès créé ! Adresse IP : ");
  Serial.println(myIP);

  // Configuration des routes Web
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);

  server.begin();
  Serial.println("Serveur Web démarré.");
}

void loop() {
  server.handleClient();
}
