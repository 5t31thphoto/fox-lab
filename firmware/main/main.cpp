// Fox Lab — minimal boot test for AtomS3R + Atomic Echo Base.
// Boot order copied from the working M5 mic-avatar:
//   1. M5.begin (no atomic_echo, internal_mic=false)
//   2. paint display
//   3. M5.Speaker.end(); then M5EchoBase.init(...)
//   4. simple fox face + chirp
//
// CONFIG_AUTOSTART_ARDUINO=n — we own app_main.

#include <Arduino.h>
#include <M5Unified.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include "fox_face.h"
#include "fox_audio.h"

static const char MARKER[] = "FOXLAB_v1_boot";

extern "C" void initArduino();

static void boot_chirp() {
    audio_tone(660, 80);
    while (audio_is_playing()) delay(2);
    audio_tone(880, 100);
    while (audio_is_playing()) delay(2);
}

extern "C" void app_main(void) {
    esp_rom_printf("\r\nFOXLAB: app_main %s\r\n", MARKER);
    ESP_LOGI("FOXLAB", "app_main %s", MARKER);

    initArduino();
    ESP_LOGI("FOXLAB", "initArduino ok");

    // --- same as working avatar ---
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    cfg.internal_mic = false;
    // never: cfg.external_speaker.atomic_echo = true;
    M5.begin(cfg);
    ESP_LOGI("FOXLAB", "M5.begin ok");
    esp_rom_printf("FOXLAB: M5.begin ok\r\n");

    // RED flash first — must see this if the panel works
    M5.Display.setBrightness(200);
    M5.Display.fillScreen(TFT_RED);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("FOX LAB", 64, 64);
    ESP_LOGI("FOXLAB", "RED flash");
    esp_rom_printf("FOXLAB: RED flash\r\n");
    delay(400);

    face_begin();
    face_draw_happy();
    ESP_LOGI("FOXLAB", "face up");
    esp_rom_printf("FOXLAB: face up\r\n");

    if (!audio_begin(70)) {
        M5.Display.setCursor(20, 110);
        M5.Display.setTextColor(TFT_RED);
        M5.Display.print("audio fail");
        ESP_LOGE("FOXLAB", "audio fail — face only");
    } else {
        boot_chirp();
        ESP_LOGI("FOXLAB", "chirp done");
    }

    ESP_LOGI("FOXLAB", "ready");
    esp_rom_printf("FOXLAB: ready\r\n");

    uint32_t last = 0;
    float mouth = 0;
    int phase = 0;
    for (;;) {
        M5.update();
        uint32_t now = millis();
        if (now - last > 80) {
            last = now;
            // idle mouth bob so the face is clearly "alive"
            phase = (phase + 1) % 40;
            mouth = (phase < 8) ? (phase / 8.0f) * 0.6f : 0.0f;
            face_set_mouth(mouth);
            if (M5.BtnA.isPressed()) face_draw_listen();
            else face_draw_idle();
        }
        // hold button: extra chirp
        if (M5.BtnA.wasClicked()) {
            face_draw_happy();
            audio_tone(520 + (esp_random() % 400), 60 + (esp_random() % 40));
        }
        delay(5);
    }
}
