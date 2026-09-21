// Fox Lab v6 — backlight is LP5562 I2C (NOT GPIO PWM).
// Exact sequence from M5GFX Light_M5StackAtomS3R:
//   i2c init SDA=45 SCL=0
//   reg 0x00 = 0x40  (chip enable)
//   reg 0x08 = 0x01
//   reg 0x70 = 0x00
//   reg 0x0E = brightness
#include <Arduino.h>
#include <M5Unified.h>
#include <Wire.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <nvs_flash.h>
#include "fox_face.h"
#include "fox_audio.h"

static const char MARKER[] = "FOXLAB_v6_lp5562";

extern "C" void initArduino();

// M5GFX Light_M5StackAtomS3R — same writes, via Arduino Wire
static bool lp5562_backlight(uint8_t brightness) {
    Wire.end();
    Wire.begin(45, 0, 400000);
    delay(2);

    auto wr = [](uint8_t reg, uint8_t val) -> bool {
        Wire.beginTransmission(0x30);
        Wire.write(reg);
        Wire.write(val);
        return Wire.endTransmission() == 0;
    };

    if (!wr(0x00, 0b01000000)) {
        ESP_LOGE("FOXLAB", "LP5562 no ACK @0x30 (enable)");
        esp_rom_printf("FOXLAB: LP5562 NO ACK\r\n");
        return false;
    }
    delay(1);
    wr(0x08, 0b00000001);
    wr(0x70, 0b00000000);
    wr(0x0E, brightness);
    ESP_LOGI("FOXLAB", "LP5562 backlight set %u", brightness);
    esp_rom_printf("FOXLAB: LP5562 OK bright=%u\r\n", brightness);
    return true;
}

static void boot_chirp() {
    audio_tone(660, 80);
    while (audio_is_playing()) delay(2);
    audio_tone(880, 100);
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

    ESP_LOGI("FOXLAB", "M5.begin ok board=%d w=%d h=%d",
             (int)M5.getBoard(), M5.Display.width(), M5.Display.height());
    esp_rom_printf("FOXLAB: begin w=%d h=%d\r\n",
                   M5.Display.width(), M5.Display.height());

    // Library path
    M5.Display.setBrightness(255);
    // Hard LP5562 path (same as M5GFX source)
    lp5562_backlight(255);

    M5.Display.fillScreen(TFT_RED);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextSize(2);
    int cx = M5.Display.width() > 0 ? M5.Display.width() / 2 : 64;
    int cy = M5.Display.height() > 0 ? M5.Display.height() / 2 : 64;
    M5.Display.drawString("RED", cx, cy);
    ESP_LOGI("FOXLAB", "RED flash");
    delay(1000);

    lp5562_backlight(255);
    M5.Display.fillScreen(TFT_GREEN);
    M5.Display.drawString("GREEN", cx, cy);
    delay(500);

    face_begin();
    face_draw_happy();
    ESP_LOGI("FOXLAB", "face up");

    // Echo Base reclaims Wire on 38/39 — re-poke LP5562 after if needed
    if (!audio_begin(70)) {
        ESP_LOGE("FOXLAB", "audio fail");
    } else {
        boot_chirp();
        // restore system I2C backlight after Echo Wire move
        lp5562_backlight(255);
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
            lp5562_backlight(255);
            M5.Display.setBrightness(255);
            audio_tone(520 + (esp_random() % 400), 60 + (esp_random() % 40));
        }
        delay(5);
    }
}
