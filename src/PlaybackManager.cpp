#include "PlaybackManager.h"
#include "AudioManager.h"

// Ein statisches, globales Audio-Objekt, das nur in dieser Datei existiert
extern Audio audio;

void initPlayback() {
    audio.setPinout(SPK_I2S_BCK, SPK_I2S_LRCK, SPK_I2S_DOUT); //
    audio.setVolume(21); // Lautstärke 0-21
    audio.forceMono(true); // Mono-Ausgabe für besseren Klang bei Sprachdateien
    audio.setTone(10, 20, 15); //
    Serial.println("[Audio] Player bereit."); //
}

void playWav(const char* filename) {
    // Übergebe den Pfad direkt. Die Library nutzt das Standard 'SD' Objekt.
    if(!audio.connecttoFS(SD, filename)) { //
        Serial.println("[Audio] Fehler beim Öffnen der Datei!"); //
    }
}

void handlePlaybackLoop() {
    audio.loop(); 
}

bool isAudioPlaying() {
    return audio.isRunning(); //
}