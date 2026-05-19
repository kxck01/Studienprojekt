#include <Arduino.h>
#include "AudioManager.h"
#include "Config.h"
#include "DisplayManager.h"
#include "NetworkManager.h"
#include "SDCardManager.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n=== SYSTEM START ===");

    // 1. SD-Karte fuer den Display-Test voruebergehend deaktiviert
    // if (!SD.begin(SD_CS)) {
    //     Serial.println("[ERROR] SD-Karte konnte nicht geladen werden!");
    //     while (true); // Stoppen, wenn keine SD da ist
    // }
    // Serial.println("[OK] SD-Karte bereit.");

    // 2. WiFi verbinden (Nutzt die Funktion aus deinem NetworkManager)
    initWiFi();

    // 2a. Uhrzeit per NTP holen, damit das Display Datum/Uhrzeit anzeigen kann
    initTimeSync();

    // 2b. Display starten
    if (!initDisplay()) {
        Serial.println("[WARN] Display konnte nicht initialisiert werden.");
    }

    // 3. Audio-, Aufnahme- und n8n-Teil fuer den Display-Test voruebergehend deaktiviert
    // if (!initAudio()) {
    //     Serial.println("[ERROR] Audio-Initialisierung fehlgeschlagen!");
    //     while (true);
    // }
    //
    // Serial.println("\n--- TEST-SEQUENZ STARTET IN 5 SEKUNDEN ---");
    // delay(5000);
    //
    // startRecording("/test_aufnahme.wav");
    //
    // if (isConnected()) {
    //     Serial.println("\n[n8n] Starte Datentransfer...");
    //     int result = sendAudioToN8n("/test_aufnahme.wav", "/antwort.wav");
    //
    //     if (result == 200) {
    //         Serial.println("[OK] n8n hat geantwortet und Datei wurde auf SD gespeichert.");
    //     } else {
    //         Serial.printf("[ERROR] n8n Kommunikation fehlgeschlagen. Code: %d\n", result);
    //     }
    // } else {
    //     Serial.println("[ERROR] Keine WiFi-Verbindung fuer n8n Upload.");
    // }
    //
    // Serial.println("\n--- TEST BEENDET ---");
    // Serial.println("Du kannst die SD-Karte jetzt am PC prüfen.");
    // Serial.println("Die Datei 'test_aufnahme.wav' sollte jetzt abspielbar sein.");
}

void loop() {
    updateDisplay();
    // updateAudio();
    delay(10);
}