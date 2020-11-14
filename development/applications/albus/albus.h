#ifndef ALBUS_H
#define ALBUS_H

#include "nrf_log.h"

#ifdef DEBUG
#define DEBUG_LOG(fmt, ...)              \
  do {                                   \
    NRF_LOG_INFO((fmt), ## __VA_ARGS__); \
  } while (0)
#else
#define DEBUG_LOG(...) do {} while (0)
#endif // DEBUG

#endif // ALBUS_H

