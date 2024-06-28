#pragma once

#include "oge/core/logging.h"

// ~ Debug break
#if _MSC_VER
  #include <intrin.h>
  #define debugBreak() __debugbreak()
#else
  #define debugBreak() __builtin_trap()
#endif

#ifdef OGE_DEBUG 
  #define OGE_ASSERT(x, msg, ...) \
  if (!(x)) { OGE_FATAL(msg, ...); debugBreak(); }
#else
  #define OGE_ASSERT(x, msg, ...)
#endif
