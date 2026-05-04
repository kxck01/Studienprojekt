#ifndef SDCARDMANAGER_H
#define SDCARDMANAGER_H

#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Config.h"

bool initSDCard();
bool deleteFile(const char* path);

#endif