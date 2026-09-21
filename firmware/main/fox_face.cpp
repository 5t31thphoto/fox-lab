#include "fox_face.h"

// Off-screen sprite: one pushSprite per frame — no fillScreen tear/flicker.
static M5Canvas s_canvas(&M5.Display);
static float s_mouth = 0.0f;
static bool s_ready = false;

static constexpr uint16_t COL_BG    = 0x08A6;
static constexpr uint16_t COL_FOX   = 0xFB43;
static constexpr uint16_t COL_CREAM = 0xF618;
static constexpr uint16_t COL_EYE   = 0x0000;
static constexpr uint16_t COL_NOSE  = 0x0000;
static constexpr uint16_t COL_TEXT  = 0xFFFF;
static constexpr uint16_t COL_ACCENT= 0xFD20;  // amber

void face_begin() {
    if (s_ready) return;
    M5.Display.setColorDepth(16);
    s_canvas.setColorDepth(16);
    s_canvas.createSprite(128, 128);
    s_ready = true;
}

void face_set_mouth(float level01) {
    if (level01 < 0) level01 = 0;
    if (level01 > 1) level01 = 1;
    s_mouth = level01;
}

static void draw_base_to_canvas() {
    auto& d = s_canvas;
    d.fillSprite(COL_BG);
    d.fillCircle(64, 70, 48, COL_FOX);
    d.fillTriangle(28, 40, 40, 8, 52, 40, COL_FOX);
    d.fillTriangle(76, 40, 88, 8, 100, 40, COL_FOX);
    d.fillTriangle(34, 38, 40, 16, 48, 38, COL_CREAM);
    d.fillTriangle(80, 38, 88, 16, 94, 38, COL_CREAM);
    d.fillCircle(64, 88, 22, COL_CREAM);
    d.fillCircle(48, 62, 8, 0xFFFF);
    d.fillCircle(80, 62, 8, 0xFFFF);
    d.fillCircle(48, 64, 4, COL_EYE);
    d.fillCircle(80, 64, 4, COL_EYE);
    d.fillCircle(64, 82, 4, COL_NOSE);
}

static void push() {
    if (!s_ready) return;
    s_canvas.pushSprite(0, 0);
}

void face_draw_idle() {
    if (!s_ready) return;
    draw_base_to_canvas();
    int h = 3 + (int)(s_mouth * 14);
    s_canvas.fillEllipse(64, 98, 14, h, COL_NOSE);
    push();
}

void face_draw_listen() {
    if (!s_ready) return;
    draw_base_to_canvas();
    s_canvas.drawCircle(48, 62, 12, COL_CREAM);
    s_canvas.drawCircle(80, 62, 12, COL_CREAM);
    int h = 4 + (int)(s_mouth * 16);
    s_canvas.fillEllipse(64, 98, 16, h, COL_NOSE);
    push();
}

void face_draw_happy() {
    if (!s_ready) return;
    draw_base_to_canvas();
    s_canvas.fillCircle(48, 60, 3, COL_EYE);
    s_canvas.fillCircle(80, 60, 3, COL_EYE);
    int h = 5 + (int)(s_mouth * 14);
    s_canvas.fillEllipse(64, 98, 16, h, COL_NOSE);
    push();
}

void face_draw_splash() {
    if (!s_ready) return;
    auto& d = s_canvas;
    d.fillSprite(0x0000);
    // Minimal fox mark
    d.fillCircle(64, 52, 28, COL_FOX);
    d.fillTriangle(42, 36, 50, 14, 58, 36, COL_FOX);
    d.fillTriangle(70, 36, 78, 14, 86, 36, COL_FOX);
    d.fillCircle(64, 62, 12, COL_CREAM);
    d.fillCircle(54, 48, 4, 0xFFFF);
    d.fillCircle(74, 48, 4, 0xFFFF);
    d.fillCircle(54, 49, 2, COL_EYE);
    d.fillCircle(74, 49, 2, COL_EYE);
    d.fillCircle(64, 58, 2, COL_NOSE);

    d.setTextDatum(middle_center);
    d.setTextColor(COL_TEXT);
    d.setTextSize(1);
    d.drawString("BasaltSoftworks", 64, 92);
    d.setTextColor(COL_ACCENT);
    d.drawString("FoxBot", 64, 106);
    d.setTextColor(0x8410);
    d.drawString("AtomS3R", 64, 118);
    push();
}
