#include "PlaybackManager.h"
#include "AudioManager.h"

PlaybackManager player;

PlaybackManager::PlaybackManager() {
}

void PlaybackManager::begin() {
    audio.setPinout(SPK_I2S_BCK, SPK_I2S_LRCK, SPK_I2S_DOUT);
    audio.setVolume(21); // Lautstärke 0-21
    audio.forceMono(true); // Mono-Ausgabe für besseren Klang bei Sprachdateien
    audio.setTone(10, 20, 15);
    Serial.println("[Audio] Player bereit.");
}

void PlaybackManager::playWav(const char* filename) {
    // Übergebe den Pfad direkt. Die Library nutzt das Standard 'SD' Objekt, 
    // welches du bereits in SDcardManager.cpp mit deinem spiSD initialisiert hast.
    if(!audio.connecttoFS(SD, filename)) {
        Serial.println("[Audio] Fehler beim Öffnen der Datei!");
    }
}


void PlaybackManager::loop() {
    audio.loop(); // Diese Funktion verarbeitet die Audio-Datenpakete
}

bool PlaybackManager::isPlaying() {
    return audio.isRunning();
}