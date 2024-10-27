// Bibliothèques utilisées
#include "Arduino.h"
#include "WiFi.h"
#include "esp_wpa2.h" // Libraire wpa2 pour se connecter au réseau d'une entreprise
#include "HX711.h"
#include "PubSubClient.h"

// Identifiants numériques au sein de l'organisation
#define EAP_IDENTITY "enzo.bernault@etu.univ-amu.fr"
#define EAP_PASSWORD "Mot de passe AMU" // Mot de passe EDUROAM
#define EAP_USERNAME "enzo.bernault@etu.univ-amu.fr"

// SSID
const char* ssid = "eduroam";

#define CONNECTION_TIMEOUT 10
int timeout_counter = 0;

const int LOADCELL_DOUT_PIN = 32;
const int LOADCELL_SCK_PIN = 33;
const int Calibration_Weight = 400;
//Le constructeur préconise un poids de calibration de 400g. Cela correspond à notre milieu de plage de mesure


const int LED_PIN = 2;
//Déclaration d'une LED qui indiquera quand effectuer l'étape de calibration.

//Permet d'accéder à toutes les fonctions du capteur HX711
HX711 scale;

//Déclaration de la variable qui stocke la valeur lue par la balance 
// Elle est également la donnée qui est envoyée sur Node-red
float reading;



const char* mqtt_broker = "172.23.9.162"; // Identifiant du broker (Adresse IP)
const char* topic = "Masse"; // Nom du topic sur lequel les données seront envoyées.
const char* mqtt_username = ""; // Identifiant dans le cas d'une liaison sécurisée
const char* mqtt_password = ""; // Mot de passe dans le cas d'une liaison sécurisée
const int mqtt_port = 1883; // Port : 1883 dans le cas d'une liaison non sécurisée

WiFiClient espClient;
PubSubClient client(espClient);

// Fonction permettant de récupérer les informations associées au réseau.
void get_network_info() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("[*] Informations - SSID : ");
        Serial.println(ssid);
        Serial.println((String)"[+] RSSI : " + WiFi.RSSI() + " dB");
    }
}

// Fonction réception du message MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Le message a été envoyé sur le topic : ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

// Initialisation du programme
void setup() {
    Serial.begin(9600);
    delay(10);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

// Connexion au réseau WiFi eduroam
    Serial.print(F("Connexion au réseau : "));
    Serial.println(ssid);
    WiFi.disconnect(true);
    delay(500);
    WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
    //Connexion au réseau éduroam en utilisant l'adresse email AMU et le mot de passe associé

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
        timeout_counter++;
        if (timeout_counter >= CONNECTION_TIMEOUT * 5) {
            ESP.restart();
        }
    }

    Serial.println("");
    Serial.println(F("Connecté au réseau WiFi."));
    get_network_info();

    // Connexion au broker MQTT
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);

    while (!client.connected()) { //Boucle indéfiniment tant que le client MQTT n'est pas connecté au broker
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("La chaîne de mesure se connecte au broker MQTT", client_id.c_str());

        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("La chaîne de mesure est connectée au broker.");
        } else {
            Serial.print("La chaîne de mesure n'a pas réussi à se connecter ... ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // Initialisation du capteur HX711
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); //Initialise le capteur avec les broches spécifiées
    scale.set_scale();//Initialise le facteur de calibration du capteur. Ce dernier est prêt à recevoir une valeur de calibration
    scale.tare();//Effectue une tare de la balance 
    delay(5000);
    Serial.println("Calibration");
    Serial.println("Placez un poids connu sur la balance");
    digitalWrite(LED_PIN, HIGH);// La LED s'allume pour informer l'utilisateur de placer la masse de 400 g sur la balance
    delay(10000);

    float x = scale.get_units(10);//Effectue une mesure moyenne de 10 lectures que l'on stocke dans la variable "x"
    x = x / Calibration_Weight;//On définit Le facteur de calibration x comme le rapport entre la moyenne des 10 mesures brutes et la masse de calibration de 400g.
    scale.set_scale(x); //Le facteur de calibration réglé est "x", calculé à la ligne précédente.
    Serial.println("Calibration terminée...");
    digitalWrite(LED_PIN, LOW);// La LED s'éteint : l'opérateur retire la masse de calibration, la balance est désormais calibrée.
    delay(3000);
}

// Boucle principale
void loop() {
    if (scale.is_ready()) { //Vérifie si le capteur est prêt à fournir des données
        reading = scale.get_units(10); //Stocke 10 mesures de masse dans la variable "reading"
        Serial.print("Lecture HX711: ");
        Serial.println(reading);
    } else {
        Serial.println("HX711 non trouvé.");
    }

    client.publish(topic, String(reading).c_str()); // Publication de la valeur "reading" sur le topic
    client.subscribe(topic); // S'abonne au topic pour recevoir des messages
    client.loop(); // Gère les messages MQTT
    delay(5000); // Pause de 5 secondes entre chaque envoi
}
