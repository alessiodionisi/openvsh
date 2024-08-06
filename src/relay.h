#ifndef RELAY_H
#define RELAY_H 1

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "macro.h"

#define RELAY_DECLARE(NAME, SPEC)                         \
  static void NAME##_toggle()                             \
  {                                                       \
    int err;                                              \
                                                          \
    err = gpio_pin_toggle_dt(SPEC);                       \
    if (err != 0)                                         \
    {                                                     \
      LOG_ERR(#NAME " GPIO toggle failed (err %d)", err); \
      return;                                             \
    }                                                     \
  }                                                       \
                                                          \
  static void NAME##_set(int value)                       \
  {                                                       \
    int err;                                              \
                                                          \
    err = gpio_pin_set_dt(SPEC, value);                   \
    if (err != 0)                                         \
    {                                                     \
      LOG_ERR(#NAME " GPIO set failed (err %d)", err);    \
      return;                                             \
    }                                                     \
  }

#define RELAY_CONFIGURE(NAME, SPEC)                          \
  {                                                          \
    int err;                                                 \
                                                             \
    if (!gpio_is_ready_dt(SPEC))                             \
    {                                                        \
      LOG_ERR(#NAME " GPIO is not ready");                   \
      return 1;                                              \
    }                                                        \
                                                             \
    err = gpio_pin_configure_dt(SPEC, GPIO_OUTPUT_INACTIVE); \
    if (err != 0)                                            \
    {                                                        \
      LOG_ERR(#NAME " GPIO configure failed (err %d)", err); \
      return err;                                            \
    }                                                        \
                                                             \
    LOG_DBG(#NAME " configuration completed");               \
  }

#endif /* RELAY_H */
