#include <Arduino.h>
#include "Config.h"
#include "SDCardManager.h"
#include "NetworkManager.h"
#include "DisplayManager.h"
#include "PlaybackManager.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n=== AUDIO PLAYER TEST ===");

    // 1) SD-Karte initialisieren (wird für Playback und Recording benötigt)
    if (!initSDCard()) {
        Serial.println("[ERROR] SD-Karte konnte nicht gestartet werden!");
        for (;;) { delay(1000); }
    }

    // 2) Audio/Player konfigurieren
    player.begin();

    // 3) Netzwerk und Zeit (WiFi muss für NTP verfügbar sein)
    initWiFi();
    initTimeSync();

    // 4) Display initialisieren (zeigt Zeit/Status)
    if (!initDisplay()) {
        Serial.println("[WARN] Display konnte nicht initialisiert werden.");
    }

    // 5) Prüfen, ob Test-Datei existiert und abspielen
    // Hinweis: PlaybackManager unterstützt WAV via playWav(), daher .wav verwenden
    const char* testFile = "/antwort.wav";
    if (SD.exists(testFile)) {
        Serial.println("[OK] Datei gefunden. Starte Wiedergabe...");
        player.playWav(testFile);
    } else {
        Serial.printf("[ERROR] Datei %s nicht auf SD gefunden!\n", testFile);
        Serial.println("Stelle sicher, dass eine WAV-Datei mit diesem Namen existiert.");
    }
}

void loop() {
    // PlaybackManager benötigt regelmäßige Verarbeitung
    player.loop();

    // Status-Update alle 5 Sekunden im Serial Monitor
    static unsigned long lastStatusMillis = 0;
    if (millis() - lastStatusMillis > 5000) {
        if (player.isPlaying()) {
            Serial.println("[INFO] Audio spielt noch...");
        } else {
            Serial.println("[INFO] Nichts wird abgespielt.");
        }
        lastStatusMillis = millis();
    }

    updateDisplay();
    delay(10);
}