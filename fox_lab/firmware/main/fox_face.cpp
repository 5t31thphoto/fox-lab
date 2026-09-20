#include "fox_face.h"

static float s_mouth = 0.0f;
static constexpr uint16_t COL_BG   = 0x08A6;  // deep indigo
static constexpr uint16_t COL_FOX  = 0xFB43;  // orange
static constexpr uint16_t COL_CREAM= 0xF618;
static constexpr uint16_t COL_EYE  = 0x0000;
static constexpr uint16_t COL_NOSE = 0x0000;

void face_begin() {
    M5.Display.setColorDepth(16);
    M5.Display.setBrightness(180);
}

void face_set_mouth(float level01) {
    if (level01 < 0) level01 = 0;
    if (level01 > 1) level01 = 1;
    s_mouth = level01;
}

static void draw_base() {
    auto& d = M5.Display;
    d.fillScreen(COL_BG);
    // head
    d.fillCircle(64, 70, 48, COL_FOX);
    // ears
    d.fillTriangle(28, 40, 40, 8, 52, 40, COL_FOX);
    d.fillTriangle(76, 40, 88, 8, 100, 40, COL_FOX);
    d.fillTriangle(34, 38, 40, 16, 48, 38, COL_CREAM);
    d.fillTriangle(80, 38, 88, 16, 94, 38, COL_CREAM);
    // snout
    d.fillCircle(64, 88, 22, COL_CREAM);
    // eyes
    d.fillCircle(48, 62, 8, 0xFFFF);
    d.fillCircle(80, 62, 8, 0xFFFF);
    d.fillCircle(48, 64, 4, COL_EYE);
    d.fillCircle(80, 64, 4, COL_EYE);
    // nose
    d.fillCircle(64, 82, 4, COL_NOSE);
}

void face_draw_idle() {
    draw_base();
    // closed-ish smile
    int h = 3 + (int)(s_mouth * 14);
    M5.Display.fillEllipse(64, 98, 14, h, COL_NOSE);
}

void face_draw_listen() {
    draw_base();
    M5.Display.drawCircle(48, 62, 12, COL_CREAM);
    M5.Display.drawCircle(80, 62, 12, COL_CREAM);
    int h = 4 + (int)(s_mouth * 16);
    M5.Display.fillEllipse(64, 98, 16, h, COL_NOSE);
}

void face_draw_happy() {
    draw_base();
    // simple happy: smaller pupils shifted up
    M5.Display.fillCircle(48, 60, 3, COL_EYE);
    M5.Display.fillCircle(80, 60, 3, COL_EYE);
    int h = 5 + (int)(s_mouth * 14);
    M5.Display.fillEllipse(64, 98, 16, h, COL_NOSE);
}
