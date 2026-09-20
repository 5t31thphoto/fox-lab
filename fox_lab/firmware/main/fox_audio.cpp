// Echo Base only — no driver/i2s.h (conflicts with ESP_I2S on IDF 5).
#include "fox_audio.h"
#include <math.h>
#include <stdlib.h>
#include <esp_log.h>

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0))
M5EchoBase g_echo;
#else
M5EchoBase g_echo(I2S_NUM_0);
#endif

bool g_echo_ok = false;
static uint32_t s_play_until = 0;

bool audio_begin(uint8_t volume) {
    // Avatar order: end Unified speaker before taking the bus.
    M5.Speaker.end();
    M5.Mic.end();

    if (!g_echo.init(16000, 38, 39, 7, 6, 5, 8, Wire)) {
        ESP_LOGE("FOXLAB", "EchoBase init failed");
        g_echo_ok = false;
        return false;
    }
    g_echo.setMicGain(ES8311_MIC_GAIN_6DB);
    if (volume > 100) volume = 100;
    g_echo.setSpeakerVolume(volume);
    g_echo.setMute(false);
    g_echo_ok = true;
    ESP_LOGI("FOXLAB", "EchoBase ok");
    return true;
}

void audio_set_volume(uint8_t volume) {
    if (!g_echo_ok) return;
    if (volume > 100) volume = 100;
    g_echo.setSpeakerVolume(volume);
}

bool audio_play_pcm16(const int16_t* buf, size_t samples, int rate) {
    if (!g_echo_ok || !buf || !samples) return false;
    g_echo.setMute(false);
    uint32_t ms = (uint32_t)((samples * 1000ULL) / (rate > 0 ? rate : 16000));
    s_play_until = millis() + ms + 20;
    return g_echo.play((uint8_t*)buf, (int)(samples * sizeof(int16_t)));
}

void audio_tone(int freq_hz, int duration_ms) {
    if (!g_echo_ok || duration_ms <= 0) return;
    const int sr = 16000;
    int n = (sr * duration_ms) / 1000;
    if (n > sr / 5) n = sr / 5;
    if (n < 1) return;
    int16_t* buf = (int16_t*)malloc(n * sizeof(int16_t));
    if (!buf) return;
    for (int i = 0; i < n; ++i) {
        float t = (float)i / (float)sr;
        float s = sinf(2.0f * 3.14159265f * (float)freq_hz * t);
        float env = (i < 32) ? i / 32.0f : (i > n - 32 ? (n - i) / 32.0f : 1.0f);
        buf[i] = (int16_t)(s * env * 12000.0f);
    }
    g_echo.setMute(false);
    g_echo.play((uint8_t*)buf, n * (int)sizeof(int16_t));
    s_play_until = millis() + (uint32_t)duration_ms + 20;
    free(buf);
}

bool audio_is_playing() { return millis() < s_play_until; }
