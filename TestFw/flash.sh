#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
firmware="${1:-"$project_dir/build/TestFw_for_OtaUpdate.bin"}"

if ! command -v openocd >/dev/null 2>&1; then
  echo "Error: openocd was not found in PATH." >&2
  exit 1
fi

if [[ ! -f "$firmware" ]]; then
  echo "Error: firmware binary not found: $firmware" >&2
  echo "Build it first with ./build.sh, or pass the binary path as an argument." >&2
  exit 1
fi

exec openocd \
  -f interface/stlink.cfg \
  -f target/stm32wbx.cfg \
  -c "program \"$firmware\" 0x08010000 verify reset exit"
