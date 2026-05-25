#ifndef PLAYBACK_MANAGER_H
#define PLAYBACK_MANAGER_H

#include "Config.h"
#include <Arduino.h>
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Freie Funktionen im selben prozeduralen Stil wie die anderen Manager
void initPlayback();
void playWav(const char* filename);
void handlePlaybackLoop(); // Umbenannt, um Namenskonflikte im Haupt-Loop zu vermeiden
bool isAudioPlaying();     // Umbenannt für klare, freie Lesbarkeit

#endif