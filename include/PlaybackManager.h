#ifndef PLAYBACK_MANAGER_H
#define PLAYBACK_MANAGER_H

#include "Config.h"
#include <Arduino.h>
#include "Audio.h"
#include "SD.h"
#include "FS.h"

class PlaybackManager {
public:
    PlaybackManager();
    void begin();
    void playWav(const char* filename);
    void loop(); // Muss in der main loop() aufgerufen werden
    bool isPlaying();

private:
    Audio audio;
};

extern PlaybackManager player;

#endif