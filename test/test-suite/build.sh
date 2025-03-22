#!/bin/bash

arduino-cli compile -e -b arduino:avr:mega \
  --libraries ~/Arduino/libraries \
  --build-property build.extra_flags="-DDEBUG" .
