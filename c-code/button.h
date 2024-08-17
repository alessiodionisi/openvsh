#ifndef BUTTON_H
#define BUTTON_H 1

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "macro.h"

typedef void (*ovsh_button_pressed_cb_t)();
typedef void (*ovsh_button_released_cb_t)();
typedef void (*ovsh_button_pressed_for_ten_seconds_cb_t)();
typedef void (*ovsh_button_pressed_for_fifteen_seconds_cb_t)();

typedef struct ovsh_button_callbacks_s
{
  ovsh_button_pressed_cb_t pressed;
  ovsh_button_released_cb_t released;
  ovsh_button_pressed_for_ten_seconds_cb_t pressed_for_ten_seconds;
  ovsh_button_pressed_for_fifteen_seconds_cb_t pressed_for_fifteen_seconds;
} ovsh_button_callbacks_t;

#define BUTTON_DECLARE(NAME, SPEC, CALLBACKS)                                                 \
  static struct gpio_callback NAME##_callback;                                                \
  static struct k_timer NAME##_ten_seconds_timer;                                             \
  static struct k_timer NAME##_fifteen_seconds_timer;                                         \
  static struct k_timer NAME##_debounce_timer;                                                \
                                                                                              \
  static void NAME##_callback_handler(const struct device *port,                              \
                                      struct gpio_callback *cb,                               \
                                      gpio_port_pins_t pins)                                  \
  {                                                                                           \
    if (gpio_pin_get_dt(SPEC))                                                                \
    {                                                                                         \
      k_timer_start(&NAME##_debounce_timer, K_MSEC(50), K_NO_WAIT);                           \
    }                                                                                         \
    else                                                                                      \
    {                                                                                         \
      k_timer_stop(&NAME##_ten_seconds_timer);                                                \
      k_timer_stop(&NAME##_fifteen_seconds_timer);                                            \
      k_timer_stop(&NAME##_debounce_timer);                                                   \
                                                                                              \
      if (PTR_TO_VAL(CALLBACKS).released)                                                     \
      {                                                                                       \
        PTR_TO_VAL(CALLBACKS).released();                                                     \
      }                                                                                       \
    }                                                                                         \
  }                                                                                           \
                                                                                              \
  static void NAME##_debounce_timer_expired(struct k_timer *timer)                            \
  {                                                                                           \
    if (PTR_TO_VAL(CALLBACKS).pressed)                                                        \
    {                                                                                         \
      PTR_TO_VAL(CALLBACKS).pressed();                                                        \
    }                                                                                         \
                                                                                              \
    k_timer_start(&NAME##_ten_seconds_timer, K_SECONDS(10), K_NO_WAIT);                       \
    k_timer_start(&NAME##_fifteen_seconds_timer, K_SECONDS(15), K_NO_WAIT);                   \
  }                                                                                           \
                                                                                              \
  static void NAME##_ten_seconds_timer_expired(struct k_timer *timer)                         \
  {                                                                                           \
    if (PTR_TO_VAL(CALLBACKS).pressed_for_ten_seconds)                                        \
    {                                                                                         \
      PTR_TO_VAL(CALLBACKS).pressed_for_ten_seconds();                                        \
    }                                                                                         \
  }                                                                                           \
                                                                                              \
  static void NAME##_fifteen_seconds_timer_expired(struct k_timer *timer)                     \
  {                                                                                           \
    if (PTR_TO_VAL(CALLBACKS).pressed_for_fifteen_seconds)                                    \
    {                                                                                         \
      PTR_TO_VAL(CALLBACKS).pressed_for_fifteen_seconds();                                    \
    }                                                                                         \
  }                                                                                           \
                                                                                              \
  int NAME##_configure()                                                                      \
  {                                                                                           \
    int err;                                                                                  \
                                                                                              \
    LOG_INF("Configuring " #NAME);                                                            \
                                                                                              \
    if (!gpio_is_ready_dt(SPEC))                                                              \
    {                                                                                         \
      LOG_ERR(#NAME " GPIO is not ready");                                                    \
      return 1;                                                                               \
    }                                                                                         \
                                                                                              \
    err = gpio_pin_configure_dt(SPEC, GPIO_INPUT);                                            \
    if (err != 0)                                                                             \
    {                                                                                         \
      LOG_ERR(#NAME " GPIO configure failed (err: %d)", err);                                 \
      return err;                                                                             \
    }                                                                                         \
                                                                                              \
    err = gpio_pin_interrupt_configure_dt(SPEC, GPIO_INT_EDGE_BOTH);                          \
    if (err != 0)                                                                             \
    {                                                                                         \
      LOG_ERR(#NAME " GPIO interrupt configure failed (err: %d)", err);                       \
      return err;                                                                             \
    }                                                                                         \
                                                                                              \
    gpio_init_callback(&NAME##_callback, NAME##_callback_handler, BIT(PTR_TO_VAL(SPEC).pin)); \
                                                                                              \
    err = gpio_add_callback(PTR_TO_VAL(SPEC).port, &NAME##_callback);                         \
    if (err != 0)                                                                             \
    {                                                                                         \
      LOG_ERR(#NAME " GPIO add callback failed (err: %d)", err);                              \
      return err;                                                                             \
    }                                                                                         \
                                                                                              \
    k_timer_init(&NAME##_ten_seconds_timer, NAME##_ten_seconds_timer_expired, NULL);          \
    k_timer_init(&NAME##_fifteen_seconds_timer, NAME##_fifteen_seconds_timer_expired, NULL);  \
    k_timer_init(&NAME##_debounce_timer, NAME##_debounce_timer_expired, NULL);                \
                                                                                              \
    LOG_DBG(#NAME " configured");                                                             \
  }

#endif /* BUTTON_H */
