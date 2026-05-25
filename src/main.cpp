#include <Arduino.h>
#include "Config.h"
#include "AudioManager.h"
#include "SDCardManager.h"
#include "NetworkManager.h"
#include "DisplayManager.h"
#include "PlaybackManager.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    pinMode(ButtonPin, INPUT_PULLUP);

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
        int result = sendAudioToN8n("/test_aufnahme.wav", "/antwort.mp3");

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


    // 6. Audio/Player konfigurieren
    initPlayback();


    /*// 4) Display initialisieren (zeigt Zeit/Status)
    if (!initDisplay()) {
        Serial.println("[WARN] Display konnte nicht initialisiert werden.");
    }
*/
    // 7. Prüfen, ob Test-Datei existiert und abspielen
    const char* testFile = "/antwort.mp3";
    if (SD.exists(testFile)) {
        Serial.println("[OK] Datei gefunden. Starte Wiedergabe...");
        playWav(testFile);
    } else {
        Serial.printf("[ERROR] Datei %s nicht auf SD gefunden!\n", testFile);
        Serial.println("Stelle sicher, dass eine WAV-Datei mit diesem Namen existiert.");
    }
}

void loop() {
    // PlaybackManager benötigt regelmäßige Verarbeitung
    handlePlaybackLoop();

    // Status-Update alle 5 Sekunden im Serial Monitor
    static unsigned long lastStatusMillis = 0;
    if (millis() - lastStatusMillis > 5000) {
        if (isAudioPlaying()) {
            Serial.println("[INFO] Audio spielt noch...");
        } else {
            Serial.println("[INFO] Nichts wird abgespielt.");
        }
        lastStatusMillis = millis();
    }

   // updateDisplay();
    delay(10);
}