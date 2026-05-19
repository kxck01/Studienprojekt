#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- WiFi & n8n ---
const char* const WIFI_SSID = "Enter SSID here";
const char* const WIFI_PASS = "Enter password here";
const char* const N8N_WEBHOOK_URL = "WEBHOOK_URL_HERE"; // Ersetze durch deine n8n Webhook URL


// --- SD-Karte (SPI Modus) ---
#define SD_CS         10
#define SD_MOSI       11
#define SD_SCK        12
#define SD_MISO       13

// --- I2S Mikrofon (INMP441) ---
#define MIC_I2S_WS    4
#define MIC_I2S_SD    5
#define MIC_I2S_SCK   6

// --- I2S Lautsprecher (MAX98357A) ---
#define SPK_I2S_BCK   17
#define SPK_I2S_LRCK  18
#define SPK_I2S_DOUT  8

#endif