#ifndef RELAY_H
#define RELAY_H 1

#include "macro.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define OVSH_RELAY_DECLARE_HEADER(NAME) \
  void NAME##_toggle();                 \
  void NAME##_set(bool value);          \
  int NAME##_configure();

#define OVSH_RELAY_DECLARE(NAME, SPEC)                                 \
  void NAME##_toggle()                                                 \
  {                                                                    \
    int err;                                                           \
                                                                       \
    err = gpio_pin_toggle_dt(SPEC);                                    \
    if (err != 0)                                                      \
    {                                                                  \
      LOG_ERR("Relay " #NAME " GPIO toggle failed (err: %d)", err);    \
      return;                                                          \
    }                                                                  \
  }                                                                    \
                                                                       \
  void NAME##_set(bool value)                                          \
  {                                                                    \
    int err;                                                           \
                                                                       \
    err = gpio_pin_set_dt(SPEC, value);                                \
    if (err != 0)                                                      \
    {                                                                  \
      LOG_ERR("Relay " #NAME " GPIO set failed (err: %d)", err);       \
      return;                                                          \
    }                                                                  \
  }                                                                    \
                                                                       \
  int NAME##_configure()                                               \
  {                                                                    \
    int err;                                                           \
                                                                       \
    LOG_INF("Configuring relay " #NAME);                               \
                                                                       \
    if (!gpio_is_ready_dt(SPEC))                                       \
    {                                                                  \
      LOG_ERR("Relay " #NAME " GPIO is not ready");                    \
      return 1;                                                        \
    }                                                                  \
                                                                       \
    err = gpio_pin_configure_dt(SPEC, GPIO_OUTPUT_INACTIVE);           \
    if (err != 0)                                                      \
    {                                                                  \
      LOG_ERR("Relay " #NAME " GPIO configure failed (err: %d)", err); \
      return err;                                                      \
    }                                                                  \
                                                                       \
    LOG_DBG("Relay " #NAME " configured");                             \
                                                                       \
    return 0;                                                          \
  }

#endif /* RELAY_H */
