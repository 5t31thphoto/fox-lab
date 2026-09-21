#!/usr/bin/env python3
"""Write a valid esp-web-tools manifest.json into dist/."""
import csv
import json
import os
import sys

def factory_offset(partitions_csv: str) -> int:
    with open(partitions_csv) as f:
        for row in csv.reader(f):
            row = [c.strip() for c in row if c.strip()]
            if not row or row[0].startswith("#"):
                continue
            if row[0] == "factory":
                return int(row[3], 0)
    return 0x20000

def main() -> int:
    dist = "dist"
    os.makedirs(dist, exist_ok=True)
    offset = factory_offset("firmware/partitions.csv")
    manifest = {
        "name": "Fox Lab",
        "version": "1.0.3",
        "new_install_prompt_erase": True,
        "builds": [
            {
                "chipFamily": "ESP32-S3",
                "parts": [
                    {"path": "bootloader.bin", "offset": 0},
                    {"path": "partition-table.bin", "offset": 32768},
                    {"path": "foxlab.bin", "offset": offset},
                ],
            }
        ],
    }
    path = os.path.join(dist, "manifest.json")
    with open(path, "w") as out:
        json.dump(manifest, out, indent=2)
        out.write("\n")
    # hard validate
    with open(path) as f:
        data = json.load(f)
    assert data["builds"][0]["parts"][2]["path"] == "foxlab.bin"
    assert isinstance(data["builds"][0]["parts"][2]["offset"], int)
    for name in ("bootloader.bin", "partition-table.bin", "foxlab.bin", "manifest.json"):
        p = os.path.join(dist, name)
        if not (os.path.isfile(p) and os.path.getsize(p) > 0):
            print(f"missing or empty: {p}", file=sys.stderr)
            return 1
        print(f"{p}: {os.path.getsize(p)} bytes")
    print(f"manifest ok, factory offset={offset} (0x{offset:x})")
    print(open(path).read())
    return 0

if __name__ == "__main__":
    sys.exit(main())
