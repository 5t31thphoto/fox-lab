// Fox Lab v9 — stable glass + audio
// Display works via LP5562. Audio broke because backlight used Wire (Echo needs Wire).
// Fix: LP5562 on Wire1 (SDA45/SCL0), Echo keeps Wire (38/39).
#include <Arduino.h>
#include <M5Unified.h>
#include <Wire.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <nvs_flash.h>
#include "fox_face.h"
#include "fox_audio.h"

static const char MARKER[] = "FOXLAB_v9_stable";

extern "C" void initArduino();

static bool s_bl_ok = false;

// Backlight only — never touch Wire (Echo owns it)
static bool lp5562_set(uint8_t brightness) {
    Wire1.end();
    Wire1.begin(45, 0, 400000);
    delay(1);
    auto wr = [](uint8_t reg, uint8_t val) -> bool {
        Wire1.beginTransmission(0x30);
        Wire1.write(reg);
        Wire1.write(val);
        return Wire1.endTransmission() == 0;
    };
    if (!wr(0x00, 0x40)) {
        ESP_LOGE("FOXLAB", "LP5562 NO ACK on Wire1");
        esp_rom_printf("FOXLAB: LP5562 NO ACK\r\n");
        return false;
    }
    delay(1);
    wr(0x08, 0x01);
    wr(0x70, 0x00);
    wr(0x0E, brightness);
    s_bl_ok = true;
    return true;
}

static void boot_chirp() {
    audio_tone(660, 80);
    while (audio_is_playing()) delay(2);
    audio_tone(880, 120);
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

    ESP_LOGI("FOXLAB", "M5.begin board=%d %dx%d",
             (int)M5.getBoard(), M5.Display.width(), M5.Display.height());

    // Backlight once on Wire1 — leave Wire alone for Echo
    lp5562_set(255);
    M5.Display.setBrightness(255);

    M5.Display.fillScreen(TFT_RED);
    delay(300);
    M5.Display.fillScreen(TFT_BLACK);

    face_begin();
    face_draw_happy();
    ESP_LOGI("FOXLAB", "face up bl=%d", s_bl_ok ? 1 : 0);

    // Echo Base: Wire 38/39 — do NOT call lp5562 after this except on Wire1
    if (!audio_begin(70)) {
        ESP_LOGE("FOXLAB", "audio fail");
        M5.Display.setCursor(4, 110);
        M5.Display.setTextColor(TFT_RED);
        M5.Display.print("audio fail");
    } else {
        boot_chirp();
        ESP_LOGI("FOXLAB", "chirp done");
        // refresh backlight without touching Echo's Wire
        lp5562_set(255);
    }

    ESP_LOGI("FOXLAB", "ready");
    esp_rom_printf("FOXLAB: ready\r\n");

    uint32_t last = 0;
    int phase = 0;
    for (;;) {
        M5.update();
        uint32_t now = millis();
        if (now - last > 80) {
            last = now;
            phase = (phase + 1) % 40;
            face_set_mouth(phase < 8 ? (phase / 8.0f) * 0.6f : 0.0f);
            if (M5.BtnA.isPressed()) face_draw_listen();
            else face_draw_idle();
        }
        if (M5.BtnA.wasClicked()) {
            face_draw_happy();
            // tone only — no panel reinit, no Wire steal
            audio_tone(520 + (esp_random() % 400), 80 + (esp_random() % 40));
        }
        delay(5);
    }
}
