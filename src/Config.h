#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- WiFi & n8n ---
const char* const WIFI_SSID = "Your WiFi SSID";
const char* const WIFI_PASS = "Your WiFi password";
const char* const N8N_WEBHOOK_URL = "your-n8n-webhook-url";


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
// Sorgt für die Sprachausgabe der n8n-Antwort
#define SPK_I2S_BCK   1
#define SPK_I2S_LRCK  2
#define SPK_I2S_DOUT  42

#endif