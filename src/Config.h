#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- WiFi & n8n ---
const char* const WIFI_SSID = "FRITZ!Box WLAN 3370";
const char* const WIFI_PASS = "FlobbogoenntInternet";
const char* const N8N_WEBHOOK_URL = "your-n8n-webhook-url";

// --- NTP / Uhrzeit ---
const char* const NTP_SERVER_1 = "pool.ntp.org";
const char* const NTP_SERVER_2 = "de.pool.ntp.org";
const char* const TIMEZONE = "CET-1CEST,M3.5.0/2,M10.5.0/3";

// --- 1.3" IPS LCD (ST7789, SPI) ---
#define TFT_CS    14
#define TFT_DC    27
#define TFT_RST   33
#define TFT_BL    32
#define TFT_SCLK  18
#define TFT_SDA   23
#define TFT_ROTATION 1  


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