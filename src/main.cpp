#include <Arduino.h>
#include "Config.h"
#include "SDCardManager.h"
#include "PlaybackManager.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n=== AUDIO PLAYER TEST ===");

    // 1. SD-Karte über deinen Manager initialisieren
    if (!initSDCard()) {
        Serial.println("[ERROR] SD-Karte konnte nicht gestartet werden!");
        while (true); 
    }

    // 2. Lautsprecher-Pins konfigurieren
    player.begin();

    // 3. Prüfen, ob Test-Datei existiert
    if (SD.exists("/antwort.mp3")) {
        Serial.println("[OK] Datei gefunden. Starte Wiedergabe...");
        player.playWav("/antwort.mp3");
    } else {
        Serial.println("[ERROR] Datei /antwort.mp3 nicht auf SD gefunden!");
        Serial.println("Stelle sicher, dass eine Datei mit diesem Namen existiert.");
    }
}

void loop() {
    // Ganz wichtig: Der PlaybackManager braucht Rechenzeit!
    player.loop();

    // Status-Update alle 5 Sekunden im Serial Monitor
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 5000) {
        if (player.isPlaying()) {
            Serial.println("[INFO] Audio spielt noch...");
        } else {
            Serial.println("[INFO] Nichts wird abgespielt.");
        }
        lastCheck = millis();
    }
}