#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "Config.h"
#include "SD.h"


void initWiFi();
void initTimeSync();
bool isConnected();
int sendAudioToN8n(const char* filePath, const char* downloadPath);

#endif