#!/bin/bash

# Usage:
#   ./build.sh        Build a .hex suitable for flashing to real hardware
#   ./build.sh -s     Build a .hex suitable for SimulIDE simulation
#
# Links across three libraries: Eventuino, TestTool, and BareMetalHAL.
# Mirrors examples/button_basic_avr/build.sh's shape, plus TestTool's
# TestInvocation.cpp (mirroring TestTool's own basic-avr/build.sh).

set -euo pipefail

SIM_MODE=false
while getopts "s" opt; do
  case $opt in
    s) SIM_MODE=true ;;
  esac
done

find_avr_tool() {
  local tool="$1"
  local found

  if command -v "$tool" >/dev/null 2>&1; then
    command -v "$tool"
    return
  fi

  local search_roots=(
    "$HOME/Library/Arduino15/packages/arduino/tools/avr-gcc"   # macOS
    "$HOME/.arduino15/packages/arduino/tools/avr-gcc"          # Linux
    "$HOME/.platformio/packages/toolchain-atmelavr"
    "/opt/homebrew/opt/avr-gcc"
    "/opt/homebrew/Cellar/avr-gcc"
    "/usr/local/opt/avr-gcc"
    "/usr/local/Cellar/avr-gcc"
    "/opt/local"
    "/usr/local/avr"
    "/opt/avr"
    "/usr/avr"
  )

  for root in "${search_roots[@]}"; do
    found=$(find "$root" -name "$tool" -type f 2>/dev/null | sort -V | tail -1)
    if [ -n "$found" ]; then echo "$found"; return; fi
  done

  echo "ERROR: $tool not found on PATH or in any of the usual install locations (Arduino15, PlatformIO, Homebrew, MacPorts, /usr/local/avr, /opt/avr, /usr/avr)" >&2
  exit 1
}

AVRGXX="$(find_avr_tool avr-g++)"
AVROBJCOPY="$(find_avr_tool avr-objcopy)"
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$DIR/build"

BAREMETALHAL_SRC="${BAREMETALHAL_SRC:-$HOME/Arduino/libraries/BareMetalHAL/src}"
if [ ! -f "$BAREMETALHAL_SRC/BareMetalHAL.h" ]; then
  echo "ERROR: BareMetalHAL.h not found under $BAREMETALHAL_SRC - set BAREMETALHAL_SRC to its src/ directory" >&2
  exit 1
fi

TESTTOOL_SRC="${TESTTOOL_SRC:-$HOME/Arduino/libraries/TestTool/src}"
if [ ! -f "$TESTTOOL_SRC/TestTool.h" ]; then
  echo "ERROR: TestTool.h not found under $TESTTOOL_SRC - set TESTTOOL_SRC to its src/ directory" >&2
  exit 1
fi

mkdir -p "$BUILD_DIR"

"$AVRGXX" -std=gnu++11 -Wall -Wextra -Os -DNO_ARDUINO -DHAL_AVR -DF_CPU=16000000UL -mmcu=atmega2560 \
  -I "$DIR/../../src" \
  -I "$BAREMETALHAL_SRC" \
  -I "$TESTTOOL_SRC" \
  "$DIR/test-suite-avr.cpp" \
  "$DIR/EventuinoTestHelper_avr.cpp" \
  "$DIR/../../src/Eventuino.cpp" \
  "$DIR/../../src/eventuino/DigitalPinSource.cpp" \
  "$DIR/../../src/eventuino/Button.cpp" \
  "$DIR/../../src/eventuino/Toggle.cpp" \
  "$DIR/../../src/hal/EventuinoHal.cpp" \
  "$TESTTOOL_SRC/TestInvocation.cpp" \
  "$BAREMETALHAL_SRC/avr/TimingHAL.cpp" \
  "$BAREMETALHAL_SRC/avr/MemoryHAL.cpp" \
  -o "$BUILD_DIR/test-suite-avr.elf"

"$AVROBJCOPY" -O ihex -R .eeprom "$BUILD_DIR/test-suite-avr.elf" "$BUILD_DIR/test-suite-avr.hex"

echo "Built $BUILD_DIR/test-suite-avr.hex"

if $SIM_MODE; then
  HEX="$BUILD_DIR/test-suite-avr.hex"
  SIM_HEX="${HEX%.hex}.sim.hex"
  python3 - "$HEX" "$SIM_HEX" << 'EOF'
import sys

def checksum(data_bytes):
    return (0x100 - sum(data_bytes) % 0x100) % 0x100

with open(sys.argv[1]) as f_in, open(sys.argv[2], 'w') as f_out:
    for line in f_in:
        line = line.strip()
        if line[7:9] == '02':  # Extended Segment Address record
            segment = int(line[9:13], 16)
            upper16 = segment >> 12
            b = [0x02, 0x00, 0x00, 0x04, upper16 >> 8, upper16 & 0xFF]
            f_out.write(f':{b[0]:02X}{b[1]:02X}{b[2]:02X}{b[3]:02X}{b[4]:02X}{b[5]:02X}{checksum(b):02X}\n')
        else:
            f_out.write(line + '\n')
EOF
  echo "SimulIDE-compatible hex: $SIM_HEX"
fi
