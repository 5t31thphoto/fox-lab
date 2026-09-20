# Boot path (must match working mic-avatar)

```
initArduino()
M5.config(): serial_baudrate=115200, internal_mic=false
  // NEVER external_speaker.atomic_echo = true
M5.begin(cfg)
paint RED
face_begin / face_draw
M5.Speaker.end(); M5.Mic.end()
echobase.init(16000, 38, 39, 7, 6, 5, 8, Wire)
chirp / loop
```

Do not include `driver/i2s.h` alongside Arduino `ESP_I2S.h`.
