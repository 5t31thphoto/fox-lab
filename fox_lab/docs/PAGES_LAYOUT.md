# Pages layout the flasher expects

After deploy, the **live site root** must look like:

```
/ (Pages root)
  index.html
  firmware/
    manifest.json
    bootloader.bin
    partition-table.bin
    foxlab.bin
  .nojekyll
```

`index.html` button: `manifest="./firmware/manifest.json"`

`manifest.json` paths are **relative to the manifest file itself** (same folder as the bins):

```json
{
  "name": "Fox Lab",
  "version": "1.0.0",
  "new_install_prompt_erase": true,
  "builds": [{
    "chipFamily": "ESP32-S3",
    "parts": [
      { "path": "bootloader.bin", "offset": 0 },
      { "path": "partition-table.bin", "offset": 32768 },
      { "path": "foxlab.bin", "offset": 131072 }
    ]
  }]
}
```

## CI stage steps that create this (paste into build-firmware)

```yaml
      - name: Stage Pages site
        run: |
          set -e
          rm -rf site
          mkdir -p site/firmware
          cp web/index.html site/index.html
          cp dist/bootloader.bin dist/partition-table.bin dist/foxlab.bin dist/manifest.json site/firmware/
          # sanity: must exist or Pages will 404 the manifest
          test -f site/index.html
          test -f site/firmware/manifest.json
          test -f site/firmware/foxlab.bin
          touch site/.nojekyll
          echo "=== site tree ==="
          find site -type f | sort
          echo "=== manifest ==="
          cat site/firmware/manifest.json

      - uses: actions/upload-pages-artifact@v3
        with:
          path: site
```

## Debug “failed to download manifest”

1. Open `https://<user>.github.io/<repo>/firmware/manifest.json`
   - **404** → deploy never put files there (stage path wrong or deploy-pages skipped)
   - **200** → button path wrong or adblock; try the link above in the same browser
2. Actions → latest green build → artifact **foxlab-firmware** must contain the four files
3. Project site URL includes the **repo name** (`/repo/`), not the user root only
