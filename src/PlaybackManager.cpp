#include "PlaybackManager.h"

PlaybackManager player;

PlaybackManager::PlaybackManager() {
}

void PlaybackManager::begin() {
    audio.setPinout(SPK_I2S_BCK, SPK_I2S_LRCK, SPK_I2S_DOUT);
    audio.setVolume(21); // Lautstärke 0-21
    Serial.println("[Audio] Player bereit.");
}

void PlaybackManager::playWav(const char* filename) {
    Serial.printf("[Audio] Spiele Datei: %s\n", filename);
    audio.connecttoFS(SD, filename);
}

void PlaybackManager::loop() {
    audio.loop(); // Diese Funktion verarbeitet die Audio-Datenpakete
}

bool PlaybackManager::isPlaying() {
    return audio.isRunning();
}