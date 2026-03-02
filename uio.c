/* Feature test macros must come before any includes */
#if !defined(_WIN32)
  #if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
  #endif
  #ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
  #endif
#endif
#if defined(__APPLE__) && !defined(_DARWIN_C_SOURCE)
  #define _DARWIN_C_SOURCE
#endif

#define UIO_IMPLEMENTATION
#include "uio.h"
