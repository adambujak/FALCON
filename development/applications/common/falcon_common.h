#ifndef FALCON_COMMON_H
#define FALCON_COMMON_H

#include <string.h>
#include <stdio.h>

void error_handler(void);

#define FLN_OK  0
#define FLN_ERR -1

#define FLN_ERR_CHECK(x)    \
  do {                      \
    int retval = (x);       \
    if (retval != FLN_OK) { \
      error_handler();      \
    }                       \
  } while (0)

#ifdef ALBUS
#include "albus.h"
#endif // ALBUS

#ifdef HEDWIG

#define ASSERT(val)         \
  do {                      \
    if(!(val)) {            \
      error_handler();      \
    }                       \
  } while (0)


#include "hedwig.h"
#endif // HEDWIG

#endif  // FALCON_COMMON_H