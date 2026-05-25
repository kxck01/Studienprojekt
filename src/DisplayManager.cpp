/*#include "DisplayManager.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <WiFi.h>
#include <time.h>

#include "Config.h"

static Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
static bool displayReady = false;
static unsigned long lastDrawMs = 0;

static void drawCenteredText(const String &text, int16_t y, uint16_t color, uint8_t size) {
    int16_t x1, y1;
    uint16_t w, h;
    tft.setTextSize(size);
    tft.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
    int16_t x = (tft.width() - w) / 2;
    tft.setCursor(x, y);
    tft.setTextColor(color);
    tft.print(text);
}

static void renderClock(bool force) {
    if (!displayReady) {
        return;
    }

    unsigned long now = millis();
    if (!force && now - lastDrawMs < 1000) {
        return;
    }
    lastDrawMs = now;

    struct tm timeInfo;
    bool timeValid = getLocalTime(&timeInfo);

    tft.fillScreen(ST77XX_BLACK);
    tft.drawRect(8, 8, tft.width() - 16, tft.height() - 16, ST77XX_CYAN);

    drawCenteredText("Voice Assistant", 20, ST77XX_CYAN, 2);

    if (timeValid) {
        char dateBuffer[24];
        char timeBuffer[16];
        strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y", &timeInfo);
        strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);

        drawCenteredText(String(dateBuffer), 86, ST77XX_WHITE, 3);
        drawCenteredText(String(timeBuffer), 132, ST77XX_YELLOW, 4);
        drawCenteredText(WiFi.isConnected() ? "WiFi verbunden" : "Kein WiFi", 198, ST77XX_GREEN, 2);
    } else {
        drawCenteredText("Warte auf Zeit...", 110, ST77XX_YELLOW, 3);
        drawCenteredText(WiFi.isConnected() ? "NTP Sync laeuft" : "Kein WiFi", 160, ST77XX_ORANGE, 2);
    }
}

bool initDisplay() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    SPI.begin(TFT_SCLK, -1, TFT_SDA, TFT_CS);
    tft.init(240, 240);
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextWrap(false);

    displayReady = true;
    renderClock(true);
    Serial.println("[DISPLAY] ST7789 bereit.");
    return true;
}

void updateDisplay() {
    renderClock(false);
}*/