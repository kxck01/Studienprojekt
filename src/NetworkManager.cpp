#include "NetworkManager.h"
#include <HTTPClient.h>

// WiFi initialisieren (Stand gestern)
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

bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * Sendet die Aufnahme an n8n (Upload von gestern)
 * Lädt die Antwort dann von Google Drive herunter
 */
int sendAudioToN8n(const char* uploadPath, const char* downloadPath) {
    if (!isConnected()) return -1;

    // 1. Aufnahme-Datei von SD öffnen
    File uploadFile = SD.open(uploadPath, FILE_READ);
    if (!uploadFile) {
        Serial.println("[n8n] Fehler: Aufnahme nicht gefunden.");
        return -2;
    }

    // --- UPLOAD PART (Gestern funktionierend) ---
    HTTPClient http;
    http.begin(N8N_WEBHOOK_URL);
    http.setTimeout(60000); // 60 Sek, da n8n nun den Drive-Upload machen muss
    http.addHeader("Content-Type", "audio/wav");

    Serial.println("[n8n] Upload läuft...");
    int httpCode = http.sendRequest("POST", &uploadFile, uploadFile.size());
    uploadFile.close();
    // ---------------------------------------------

    if (httpCode == HTTP_CODE_OK) {
        // n8n antwortet mit dem Google Drive Link
        String googleLink = http.getString();
        http.end(); // Verbindung zu n8n schließen

        Serial.println("[Cloud] Link erhalten: " + googleLink);

        // --- DOWNLOAD PART (Neu über Google Drive) ---
        if (googleLink.startsWith("http")) {
            // Wichtig: Google nutzt HTTPS. Falls es hier hakt, 
            // brauchen wir für den Download WiFiClientSecure.
            http.begin(googleLink);
            http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
            int downloadCode = http.GET();

            if (downloadCode == HTTP_CODE_OK) {
                File responseFile = SD.open(downloadPath, FILE_WRITE);
                if (responseFile) {
                    // Schreibt den Stream von Google Drive direkt auf SD
                    http.writeToStream(&responseFile);
                    responseFile.close();
                    Serial.println("[n8n] Antwort erfolgreich von Drive geladen.");
                } else {
                    Serial.println("[SD] Fehler: Konnte Datei auf SD nicht öffnen.");
                }
            } else {
                Serial.printf("[Cloud] Download Fehler: %d\n", downloadCode);
            }
        } else {
            Serial.println("[n8n] Fehler: Antwort von n8n war kein gültiger Link.");
        }
    } else {
        Serial.printf("[n8n] HTTP Fehler: %d - %s\n", httpCode, http.errorToString(httpCode).c_str());
    }

    http.end();
    return httpCode;
}