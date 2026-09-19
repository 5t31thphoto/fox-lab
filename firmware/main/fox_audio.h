#pragma once
#include <Arduino.h>
#include <M5Unified.h>
#include "M5EchoBase.h"

extern M5EchoBase g_echo;
extern bool g_echo_ok;

bool audio_begin(uint8_t volume);
void audio_set_volume(uint8_t volume);
bool audio_play_pcm16(const int16_t* buf, size_t samples, int rate);
void audio_tone(int freq_hz, int duration_ms);
bool audio_is_playing();
