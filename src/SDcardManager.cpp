#include "SDCardManager.h"

// Wir definieren einen eigenen SPI-Bus für die SD-Karte (HSPI)
SPIClass spiSD(HSPI); 

bool initSDCard() {
    Serial.println("[SD] Initialisiere SPI Bus...");
    
    // Pins aus der Config.h zuweisen
    spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    
    // SD-Karte mit dem definierten SPI-Bus starten
    if (!SD.begin(SD_CS, spiSD, 16000000)) { 
    Serial.println("[SD] Fehler: Karte konnte nicht gemountet werden.");
    return false;
    }
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("[SD] Fehler: Keine Karte eingesteckt.");
        return false;
    }

    Serial.print("[SD] Kartentyp: ");
    if (cardType == CARD_MMC) Serial.println("MMC");
    else if (cardType == CARD_SD) Serial.println("SDSC");
    else if (cardType == CARD_SDHC) Serial.println("SDHC");
    else Serial.println("Unbekannt");

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("[SD] Größe: %llu MB\n", cardSize);
    
    return true;
}

bool deleteFile(const char* path) {
    if (SD.exists(path)) {
        return SD.remove(path);
    }
    return false;
}

// Hilfsfunktion: Prüfen ob Datei existiert
bool fileExists(const char* path) {
    return SD.exists(path);
}