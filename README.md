# Fox Lab — AtomS3R + Atomic Echo Base boot test

Standalone test repo. Same shape as the real Fox project (IDF firmware,
`drop_zip`, web flasher) but **minimal**: display + simple fox face + Echo Base
audio only. No MultiNet, no brain packs, no IR library.

## Goal

Prove the boot path that already works in the M5 mic-avatar:

1. Plain `M5.begin` (no `atomic_echo`)
2. Screen lights (red flash, then fox face)
3. Standalone `M5EchoBase` for mic/speaker

Lessons here port straight into the full Fox tree.

## Hardware

- M5Stack **AtomS3R**
- **Atomic Echo Base** (ES8311)

## Flash

After CI deploys Pages: open the site → **Flash** → configure over USB if prompted.

Or build locally:

```bash
cd firmware && idf.py set-target esp32s3 && idf.py build flash monitor
```

## Expected serial

```
FOXLAB: app_main
FOXLAB: M5.begin ok
FOXLAB: RED flash
FOXLAB: face up
FOXLAB: EchoBase ok
FOXLAB: ready
```

Screen: brief **red**, then a simple fox face. Speaker: short boot chirp.
