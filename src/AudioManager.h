#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <Arduino.h>
#include "Audio.h" // ESP32-audioI2S
#include "Config.h"

extern int16_t* psram_buffer;
extern const size_t buffer_size;

bool initAudio();
void updateAudio(); // Muss in den loop()
void playResponse(const char* url);
void startRecording(const char* path);
void stopRecording();
bool listenForWakeWord();

#endif