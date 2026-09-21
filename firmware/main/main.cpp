// Fox Lab v5 — pin M5GFX >=0.2.27 (ST7735 PCN + GC9107 low-clock re-probe)
#include <Arduino.h>
#include <M5Unified.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <nvs_flash.h>
#include "fox_face.h"
#include "fox_audio.h"

static const char MARKER[] = "FOXLAB_v5_gfx027";

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

    // Fresh autodetect every boot (no NVS board:18 pin)
    nvs_flash_erase();
    nvs_flash_init();

    initArduino();
    ESP_LOGI("FOXLAB", "initArduino ok");

    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    cfg.internal_mic = false;
    cfg.clear_display = true;
    // Do NOT force cfg.board — let M5GFX 0.2.27+ probe GC9107 vs ST7735
    // at low clock (the fix the working launcher binary ships).

    M5.begin(cfg);

    ESP_LOGI("FOXLAB", "M5.begin ok board=%d w=%d h=%d",
             (int)M5.getBoard(), M5.Display.width(), M5.Display.height());
    esp_rom_printf("FOXLAB: begin board=%d w=%d h=%d\r\n",
                   (int)M5.getBoard(), M5.Display.width(), M5.Display.height());

    M5.Display.setBrightness(255);
    delay(20);

    M5.Display.fillScreen(TFT_RED);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextSize(2);
    int cx = M5.Display.width() > 0 ? M5.Display.width() / 2 : 64;
    int cy = M5.Display.height() > 0 ? M5.Display.height() / 2 : 64;
    M5.Display.drawString("RED", cx, cy);
    ESP_LOGI("FOXLAB", "RED flash");
    delay(800);

    M5.Display.fillScreen(TFT_GREEN);
    M5.Display.drawString("GREEN", cx, cy);
    delay(400);

    face_begin();
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
    for (;;) {
        M5.update();
        if (millis() - last > 80) {
            last = millis();
            phase = (phase + 1) % 40;
            face_set_mouth(phase < 8 ? (phase / 8.0f) * 0.6f : 0.0f);
            if (M5.BtnA.isPressed()) face_draw_listen();
            else face_draw_idle();
        }
        if (M5.BtnA.wasClicked()) {
            face_draw_happy();
            M5.Display.setBrightness(255);
            audio_tone(520 + (esp_random() % 400), 60 + (esp_random() % 40));
        }
        delay(5);
    }
}
