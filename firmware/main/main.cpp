// Fox Lab v10 — no flicker (sprite buffer), no color test flash,
// backlight once on Wire1, Echo on Wire, BasaltSoftworks splash.
#include <Arduino.h>
#include <M5Unified.h>
#include <Wire.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <nvs_flash.h>
#include "fox_face.h"
#include "fox_audio.h"

static const char MARKER[] = "FOXLAB_v10_splash";

extern "C" void initArduino();

// LP5562 once — Wire1 only (Echo owns Wire 38/39)
static void backlight_on() {
    Wire1.end();
    Wire1.begin(45, 0, 400000);
    delay(1);
    auto wr = [](uint8_t reg, uint8_t val) {
        Wire1.beginTransmission(0x30);
        Wire1.write(reg);
        Wire1.write(val);
        Wire1.endTransmission();
    };
    Wire1.beginTransmission(0x30);
    Wire1.write(0x00);
    Wire1.write(0x40);
    if (Wire1.endTransmission() != 0) {
        ESP_LOGW("FOXLAB", "LP5562 no ACK");
        return;
    }
    delay(1);
    wr(0x08, 0x01);
    wr(0x70, 0x00);
    wr(0x0E, 200);  // steady, not max thrash
    ESP_LOGI("FOXLAB", "backlight on (Wire1)");
}

static void boot_chirp() {
    audio_tone(660, 70);
    while (audio_is_playing()) delay(2);
    audio_tone(880, 90);
    while (audio_is_playing()) delay(2);
}

extern "C" void app_main(void) {
    esp_rom_printf("\r\nFOXLAB: app_main %s\r\n", MARKER);
    ESP_LOGI("FOXLAB", "app_main %s", MARKER);

    nvs_flash_erase();
    nvs_flash_init();
    initArduino();

    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    cfg.internal_mic = false;
    cfg.clear_display = true;
    M5.begin(cfg);

    backlight_on();
    M5.Display.setBrightness(200);

    face_begin();
    face_draw_splash();
    delay(1200);

    face_draw_happy();
    ESP_LOGI("FOXLAB", "face up");

    if (!audio_begin(70)) {
        ESP_LOGE("FOXLAB", "audio fail");
    } else {
        boot_chirp();
        ESP_LOGI("FOXLAB", "chirp done");
    }

    ESP_LOGI("FOXLAB", "ready");
    esp_rom_printf("FOXLAB: ready\r\n");

    uint32_t last = 0;
    int phase = 0;
    float last_mouth = -1;
    bool last_listen = false;

    for (;;) {
        M5.update();
        uint32_t now = millis();
        if (now - last > 80) {
            last = now;
            phase = (phase + 1) % 40;
            float mouth = (phase < 8) ? (phase / 8.0f) * 0.6f : 0.0f;
            bool listen = M5.BtnA.isPressed();
            // Only redraw when something visible changed
            if (mouth != last_mouth || listen != last_listen) {
                face_set_mouth(mouth);
                if (listen) face_draw_listen();
                else face_draw_idle();
                last_mouth = mouth;
                last_listen = listen;
            }
        }
        if (M5.BtnA.wasClicked()) {
            face_draw_happy();
            last_mouth = -1;
            audio_tone(520 + (esp_random() % 400), 80);
        }
        delay(5);
    }
}
