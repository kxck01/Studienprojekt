#include "NetworkManager.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h> 
#include <driver/i2s.h> // WICHTIG: Ermöglicht uns das harte Stoppen der Audio-Hardware
#include <time.h>

extern int16_t* psram_buffer; 

// WiFi initialisieren
void initWiFi() {
    Serial.println("[WIFI] Verbinde mit " + String(WIFI_SSID));
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WIFI] Verbunden! IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("\n[WIFI] Verbindung fehlgeschlagen!");
    }
}

// Zeitsynchronisation (NTP)
void initTimeSync() {
    if (!isConnected()) {
        Serial.println("[NTP] Kein WiFi, Zeitsync wird übersprungen.");
        return;
    }

    Serial.println("[NTP] Synchronisiere Uhrzeit...");
    configTzTime(TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);

    struct tm timeInfo;
    unsigned long startAttempt = millis();
    while (!getLocalTime(&timeInfo) && millis() - startAttempt < 15000) {
        delay(500);
        Serial.print(".");
    }

    if (getLocalTime(&timeInfo)) {
        Serial.println("\n[NTP] Uhrzeit synchronisiert.");
    } else {
        Serial.println("\n[NTP] Zeitsynchronisation fehlgeschlagen.");
    }
}

bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

int sendAudioToN8n(const char* uploadPath, const char* downloadPath) {
    if (!isConnected()) return -1;

    File uploadFile = SD.open(uploadPath, FILE_READ);
    if (!uploadFile) {
        Serial.println("[n8n] Fehler: Aufnahme-Datei nicht gefunden.");
        return -2;
    }

    // --- UPLOAD ZU N8N ---
    HTTPClient http;
    http.begin(N8N_WEBHOOK_URL);
    http.setTimeout(60000); 
    http.addHeader("Content-Type", "audio/wav");

    Serial.println("[n8n] Upload läuft...");
    int httpCode = http.sendRequest("POST", &uploadFile, uploadFile.size());
    uploadFile.close(); 

    if (httpCode == HTTP_CODE_OK) {
        String googleLink = http.getString();
        http.end(); 

        Serial.println("[Cloud] Google-Link erhalten: " + googleLink);

        if (googleLink.startsWith("http")) {
            WiFiClientSecure secureClient;
            secureClient.setInsecure(); 

            http.begin(secureClient, googleLink);
            http.setTimeout(45000); 
            http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
            
            Serial.println("[Cloud] Starte HTTPS-Verbindung zu Google...");
            int downloadCode = http.GET();

            if (downloadCode == HTTP_CODE_OK) {
                WiFiClient* stream = http.getStreamPtr();
                size_t totalSize = http.getSize(); 
                size_t bytesDownloaded = 0;

                Serial.printf("[Cloud] Lade %d Bytes in den PSRAM...\n", totalSize);

                while (http.connected() && (bytesDownloaded < totalSize || totalSize == -1)) {
                    while (stream->available() > 0) {
                        int bytesRead = stream->read(((uint8_t*)psram_buffer) + bytesDownloaded, stream->available());
                        if (bytesRead > 0) {
                            bytesDownloaded += bytesRead;
                        }
                    }
                    delay(1); 
                }
                http.end(); 

                // =================================================================
                // CRITICAL HARDWARE FIX: AUDIO-HARDWARE STOPPEN FÜR SD-ZUGRIFF
                // =================================================================
                Serial.println("[Hardware] Stoppe Audio-Hardware (I2S) für ungestörten SD-Zugriff...");
                i2s_stop(I2S_NUM_0); // Stoppt den Lautsprecher
                i2s_stop(I2S_NUM_1); // Stoppt das Mikrofon (Sicherheitshalber)
                delay(100);          // Gibt den DMA-Kanälen Zeit, sich zu beruhigen
                // =================================================================

                // Jetzt schreiben wir den Block auf die SD-Karte
                File responseFile = SD.open(downloadPath, FILE_WRITE);
                if (responseFile) {
                    Serial.println("[SD] Schreibe PSRAM-Block auf die Karte...");
                    
                    size_t written = responseFile.write((uint8_t*)psram_buffer, bytesDownloaded);
                    
                    responseFile.flush(); 
                    delay(50);
                    responseFile.close(); 
                    
                    Serial.printf("[n8n] Download beendet. %d von %d Bytes (geschrieben: %d) auf SD.\n", 
                                  bytesDownloaded, totalSize, written);
                    
                    if (written == 0) {
                        Serial.println("[FATAL] Die SD-Karte hat das Schreiben verweigert (0 Bytes geschrieben)!");
                        httpCode = -6;
                    } else {
                        httpCode = 200; // Voller Erfolg!
                    }
                } else {
                    Serial.println("[SD] Fehler: Konnte Antwort-Datei auf SD nicht erstellen.");
                    httpCode = -4;
                }

                // =================================================================
                // AUDIO-HARDWARE WIEDER STARTEN
                // =================================================================
                Serial.println("[Hardware] Starte Audio-Hardware wieder...");
                i2s_start(I2S_NUM_0); // Reaktiviert den Lautsprecher für die Wiedergabe
                delay(50);
                // =================================================================

            } else {
                Serial.printf("[Cloud] Download abgebrochen. Google HTTP-Code: %d\n", downloadCode);
                httpCode = downloadCode; 
                http.end();
            }
        } else {
            Serial.println("[n8n] Fehler: Antwort von n8n war kein gültiger HTTP-Link.");
            httpCode = -3;
        }
    } else {
        Serial.printf("[n8n] HTTP Fehler beim Upload: %d - %s\n", httpCode, http.errorToString(httpCode).c_str());
        http.end();
    }

    return httpCode;
}