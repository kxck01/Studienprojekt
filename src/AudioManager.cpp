#include "AudioManager.h"

// Audio Objekt für Port 0 (Lautsprecher)
Audio audio(false, I2S_NUM_0); 

int16_t* psram_buffer = NULL;
const size_t buffer_size = 16000 * 15 * sizeof(int16_t); // 15 Sek. Puffer

bool initAudio() {
    if (psram_buffer == NULL) {
        // Sicherstellen, dass wir wirklich PSRAM nutzen
        psram_buffer = (int16_t*) heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        
        if (psram_buffer == NULL) {
            Serial.println("[CRITICAL] PSRAM konnte nicht reserviert werden!");
            return false;
        }
        // Den Puffer einmal mit Null füllen (stabilität)
        memset(psram_buffer, 0, buffer_size);
    }
    // 2. Mikrofon auf PORT 1 konfigurieren
    i2s_config_t mic_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false
    };

    i2s_pin_config_t mic_pins = {
        .bck_io_num = MIC_I2S_SCK,
        .ws_io_num = MIC_I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = MIC_I2S_SD
    };

    // Installation auf I2S_NUM_1
    i2s_driver_install(I2S_NUM_1, &mic_config, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &mic_pins);
    i2s_stop(I2S_NUM_1); // Erst bei Aufnahme starten
    return true;
}

void updateAudio() {
    player.loop();
}

void writeWavHeader(File &file, size_t dataSize) {
    uint32_t sampleRate = 16000;
    uint16_t channels = 1; // Mono
    uint16_t bitsPerSample = 16;
    uint32_t byteRate = sampleRate * channels * bitsPerSample / 8;
    uint16_t blockAlign = channels * bitsPerSample / 8;
    uint32_t fileSize = dataSize + 44 - 8;

    file.write((const uint8_t*)"RIFF", 4);
    file.write((const uint8_t*)&fileSize, 4);
    file.write((const uint8_t*)"WAVE", 4);
    file.write((const uint8_t*)"fmt ", 4);
    
    uint32_t subChunk1Size = 16;
    uint16_t audioFormat = 1; // PCM
    file.write((const uint8_t*)&subChunk1Size, 4);
    file.write((const uint8_t*)&audioFormat, 2);
    file.write((const uint8_t*)&channels, 2);
    file.write((const uint8_t*)&sampleRate, 4);
    file.write((const uint8_t*)&byteRate, 4);
    file.write((const uint8_t*)&blockAlign, 2);
    file.write((const uint8_t*)&bitsPerSample, 2);
    
    file.write((const uint8_t*)"data", 4);
    file.write((const uint8_t*)&dataSize, 4);
}

void startRecording(const char* path) {
    // Port 1 frisch starten
    i2s_start(I2S_NUM_1);
    delay(100); 

    Serial.println("[AUDIO] Aufnahme auf Port 1 startet...");
    
    size_t total_bytes_read = 0;
    int silence_counter = 0;
    const int silence_threshold = 2000; 
    const int max_silence_blocks = 60; 

    const size_t max_bytes = buffer_size - 2048;
    
    const size_t chunk_size = 1024; 
    int32_t raw_samples[chunk_size / 4]; // Wir lesen jetzt 32-bit (4 Bytes pro Sample)

    while (total_bytes_read < max_bytes) {
        size_t bytes_read = 0;
        // Wir lesen den Port 1 (32-bit)
        esp_err_t res = i2s_read(I2S_NUM_1, raw_samples, chunk_size, &bytes_read, 1000 / portTICK_PERIOD_MS);
        
        if (res == ESP_OK && bytes_read > 0) {
            size_t samples_count = bytes_read / 4;
            int16_t processed_samples[samples_count];

            for (size_t i = 0; i < samples_count; i++) {
                // Das INMP441 "Fix": 
                // 1. Schiebe die Bits, um das Knacken zu entfernen
                // 2. Multipliziere (* 8), um die Lautstärke zu erhöhen
                int32_t val = raw_samples[i] >> 14; 
                val = val * 10; 

                // Clipping verhindern (Sicherheits-Check)
                if (val > 32767) val = 32767;
                if (val < -32768) val = -32768;

                processed_samples[i] = (int16_t)val;
            }

            // Kopiere die sauberen 16-bit Daten in den PSRAM
            memcpy((uint8_t*)psram_buffer + total_bytes_read, processed_samples, samples_count * 2);
            total_bytes_read += (samples_count * 2);

            // Lautstärke-Check für Stille-Erkennung
            int32_t max_amp = 0;
            for (size_t i = 0; i < samples_count; i++) {
                if (abs(processed_samples[i]) > max_amp) max_amp = abs(processed_samples[i]);
            }

            if (max_amp < 1500) silence_counter++; // Threshold etwas höher wegen Gain
            else silence_counter = 0;

            if (total_bytes_read > 32000 && silence_counter > max_silence_blocks) break;
        }
    }

    // Speichern
    delay(100); // Kurze Pause vor dem Schreiben
    File file = SD.open(path, FILE_WRITE);
    if (file) {
        writeWavHeader(file, total_bytes_read);
        file.write((uint8_t*)psram_buffer, total_bytes_read);
        file.close();
        Serial.printf("[AUDIO] Datei gespeichert! Größe: %d Bytes\n", total_bytes_read);
    }

    i2s_stop(I2S_NUM_1);
}