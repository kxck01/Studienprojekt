#include <Arduino.h>
#include "AudioManager.h"
#include "Config.h"
#include "NetworkManager.h"
#include "SDCardManager.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n=== SYSTEM START ===");

    // 1. SD-Karte initialisieren (Wichtig für das Speichern der WAV)
    // Achte darauf, dass SD_CS in deiner Config.h korrekt ist (meist GPIO 10 oder 13 beim S3)
    if (!SD.begin(SD_CS)) {
        Serial.println("[ERROR] SD-Karte konnte nicht geladen werden!");
        while (true); // Stoppen, wenn keine SD da ist
    }
    Serial.println("[OK] SD-Karte bereit.");

    // 2. WiFi verbinden (Nutzt die Funktion aus deinem NetworkManager)
    initWiFi();

    // 3. Audio-System (Mic & Speaker) starten
    if (!initAudio()) {
        Serial.println("[ERROR] Audio-Initialisierung fehlgeschlagen!");
        while (true);
    }

    Serial.println("\n--- TEST-SEQUENZ STARTET IN 5 SEKUNDEN ---");
    delay(5000);

    // 4. Test-Aufnahme starten
    // Die Funktion nutzt jetzt deine Silence Detection
    startRecording("/test_aufnahme.wav");

    // 5. Datei an n8n senden & Antwort empfangen
    if (isConnected()) {
        Serial.println("\n[n8n] Starte Datentransfer...");
        // uploadPath: deine frische Aufnahme
        // downloadPath: wo n8n die KI-Antwort hinspeichern soll
        int result = sendAudioToN8n("/test_aufnahme.wav", "/antwort.wav");

        if (result == 200) {
            Serial.println("[OK] n8n hat geantwortet und Datei wurde auf SD gespeichert.");
        } else {
            Serial.printf("[ERROR] n8n Kommunikation fehlgeschlagen. Code: %d\n", result);
        }
    } else {
        Serial.println("[ERROR] Keine WiFi-Verbindung für n8n Upload.");
    }

    Serial.println("\n--- TEST BEENDET ---");
    Serial.println("Du kannst die SD-Karte jetzt am PC prüfen.");
    Serial.println("Die Datei 'test_aufnahme.wav' sollte jetzt abspielbar sein.");
}

void loop() {
    // Da wir nur einmal im Setup testen, lassen wir die Loop leer.
    // Später kommt hier audio.loop() für den Lautsprecher rein.
    delay(1000);
}