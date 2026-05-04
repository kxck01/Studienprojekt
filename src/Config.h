#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- WiFi & n8n ---
const char* const WIFI_SSID = "It hurts when IP";
const char* const WIFI_PASS = "Roger!2010";
//const char* const WIFI_SSID = "Bettmeralp";
//const char* const WIFI_PASS = "TannEck2011";
const char* const N8N_WEBHOOK_URL = "https://n8n.keck1.com/webhook-test/c0da9e5e-1062-4191-9acf-3c0de4705036";


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