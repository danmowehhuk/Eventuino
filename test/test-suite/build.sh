#!/bin/bash

# Usage:
#   ./build.sh        Build a .hex suitable for flashing to real hardware
#   ./build.sh -s     Build a .hex suitable for SimulIDE simulation
#
# The -s (simulator) flag post-processes the .hex with avr-objcopy to convert
# extended address records (Intel HEX type 02/04) to a plain format that
# SimulIDE accepts. The avr-objcopy path is extracted from the verbose compiler
# output, so it is always the correct binary for the toolchain and MCU in use.

SIM_MODE=false
while getopts "s" opt; do
  case $opt in
    s) SIM_MODE=true ;;
  esac
done

COMPILE_CMD="arduino-cli compile -e -b arduino:avr:mega \
  --library ../.. \
  --libraries ~/Arduino/libraries \
  --build-property build.extra_flags=\"-DDEBUG\""

if $SIM_MODE; then
  # Capture verbose output to extract the avr-objcopy path, but still display it
  VERBOSE_LOG=$(mktemp)
  eval "$COMPILE_CMD --verbose ." 2>&1 | tee "$VERBOSE_LOG"
  AVR_OBJCOPY=$(grep -o '[^ ]*avr-objcopy' "$VERBOSE_LOG" | head -1)
  rm -f "$VERBOSE_LOG"
else
  eval "$COMPILE_CMD ."
fi

if $SIM_MODE; then
  # Convert to binary and back to strip extended address records (type 02/04)
  # that SimulIDE does not support. Uses the avr-objcopy from the toolchain
  # that was just used to compile, so it is always version- and arch-correct.
  HEX=$(ls build/arduino.avr.mega/*.hex | grep -v bootloader | head -1)
  SIM_HEX="${HEX%.hex}.sim.hex"
  # avr-objcopy regenerates type 02 (Extended Segment Address) records when
  # writing ihex for AVR targets, so the binary round-trip doesn't help.
  # Instead, rewrite type 02 records as type 04 (Extended Linear Address)
  # records directly. SimulIDE accepts type 04 but not type 02.
  # Conversion: for type 02 segment value S, type 04 upper16 = S >> 12
  # (because segment base = S*16, and upper16 = (S*16) >> 16 = S >> 12).
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
fi
