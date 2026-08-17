/*

  hal/bits.h

  Portable bitRead/bitWrite - pure bitwise arithmetic with no hardware
  dependency at all, so this isn't a HAL_xxx branch selection in the
  usual sense. On Arduino, Arduino.h already defines these as macros;
  off Arduino, this file defines the same behavior directly so callers
  never need to depend on <Arduino.h> transitively just for arithmetic.

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef EVENTUINO_HAL_BITS_H
#define EVENTUINO_HAL_BITS_H

#ifndef NO_ARDUINO
#include <Arduino.h>  // already defines bitRead/bitWrite
#else
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? ((value) |= (1UL << (bit))) : ((value) &= ~(1UL << (bit))))
#endif

#endif
