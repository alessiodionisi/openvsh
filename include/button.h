#ifndef BUTTON_H
#define BUTTON_H 1

#include "macro.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef void (*ovsh_button_pressed_handler_t)();
typedef void (*ovsh_button_released_handler_t)();
typedef void (*ovsh_button_pressed_for_ten_seconds_handler_t)();
typedef void (*ovsh_button_pressed_for_fifteen_seconds_handler_t)();

typedef struct ovsh_button_handlers_s
{
  ovsh_button_pressed_handler_t pressed;
  ovsh_button_released_handler_t released;
  ovsh_button_pressed_for_ten_seconds_handler_t pressed_for_ten_seconds;
  ovsh_button_pressed_for_fifteen_seconds_handler_t pressed_for_fifteen_seconds;
} ovsh_button_handlers_t;

#define OVSH_BUTTON_DECLARE_HEADER(NAME) \
  int NAME##_configure();                \
  void NAME##_register_handlers(ovsh_button_handlers_t *handlers);

#define OVSH_BUTTON_DECLARE(NAME, SPEC)                                                             \
  static struct gpio_callback NAME##_callback;                                                      \
  static struct k_timer NAME##_ten_seconds_timer;                                                   \
  static struct k_timer NAME##_fifteen_seconds_timer;                                               \
  static struct k_timer NAME##_debounce_timer;                                                      \
  static ovsh_button_handlers_t *NAME##_registered_handlers;                                        \
  static struct k_work NAME##_pressed_work;                                                         \
  static struct k_work NAME##_pressed_ten_seconds_work;                                             \
  static struct k_work NAME##_pressed_fifteen_seconds_work;                                         \
                                                                                                    \
  static void NAME##_callback_handler(const struct device *port,                                    \
                                      struct gpio_callback *cb,                                     \
                                      gpio_port_pins_t pins)                                        \
  {                                                                                                 \
    if (gpio_pin_get_dt(SPEC))                                                                      \
    {                                                                                               \
      k_timer_start(&NAME##_debounce_timer, K_MSEC(50), K_NO_WAIT);                                 \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
      k_timer_stop(&NAME##_ten_seconds_timer);                                                      \
      k_timer_stop(&NAME##_fifteen_seconds_timer);                                                  \
      k_timer_stop(&NAME##_debounce_timer);                                                         \
                                                                                                    \
      if (NAME##_registered_handlers && NAME##_registered_handlers->released)                       \
      {                                                                                             \
        NAME##_registered_handlers->released();                                                     \
      }                                                                                             \
    }                                                                                               \
  }                                                                                                 \
                                                                                                    \
  static void NAME##_debounce_timer_expired(struct k_timer *timer)                                  \
  {                                                                                                 \
    k_work_submit(&NAME##_pressed_work);                                                            \
    k_timer_start(&NAME##_ten_seconds_timer, K_SECONDS(10), K_NO_WAIT);                             \
    k_timer_start(&NAME##_fifteen_seconds_timer, K_SECONDS(15), K_NO_WAIT);                         \
  }                                                                                                 \
                                                                                                    \
  static void NAME##_pressed_work_handler()                                                         \
  {                                                                                                 \
    if (NAME##_registered_handlers && NAME##_registered_handlers->pressed)                          \
    {                                                                                               \
      NAME##_registered_handlers->pressed();                                                        \
    }                                                                                               \
  }                                                                                                 \
                                                                                                    \
  static void NAME##_ten_seconds_timer_expired(struct k_timer *timer)                               \
  {                                                                                                 \
    k_work_submit(&NAME##_pressed_ten_seconds_work);                                                \
  }                                                                                                 \
                                                                                                    \
  static void NAME##_pressed_ten_seconds_work_handler()                                             \
  {                                                                                                 \
    if (NAME##_registered_handlers && NAME##_registered_handlers->pressed_for_ten_seconds)          \
    {                                                                                               \
      NAME##_registered_handlers->pressed_for_ten_seconds();                                        \
    }                                                                                               \
  }                                                                                                 \
                                                                                                    \
  static void NAME##_fifteen_seconds_timer_expired(struct k_timer *timer)                           \
  {                                                                                                 \
    k_work_submit(&NAME##_pressed_fifteen_seconds_work);                                            \
  }                                                                                                 \
                                                                                                    \
  static void NAME##_pressed_fifteen_seconds_work_handler()                                         \
  {                                                                                                 \
    if (NAME##_registered_handlers && NAME##_registered_handlers->pressed_for_fifteen_seconds)      \
    {                                                                                               \
      NAME##_registered_handlers->pressed_for_fifteen_seconds();                                    \
    }                                                                                               \
  }                                                                                                 \
                                                                                                    \
  void NAME##_register_handlers(ovsh_button_handlers_t *handlers)                                   \
  {                                                                                                 \
    NAME##_registered_handlers = handlers;                                                          \
  }                                                                                                 \
                                                                                                    \
  int NAME##_configure()                                                                            \
  {                                                                                                 \
    int err;                                                                                        \
                                                                                                    \
    LOG_INF("Configuring button " #NAME);                                                           \
                                                                                                    \
    if (!gpio_is_ready_dt(SPEC))                                                                    \
    {                                                                                               \
      LOG_ERR("Button " #NAME " GPIO is not ready");                                                \
      return 1;                                                                                     \
    }                                                                                               \
                                                                                                    \
    err = gpio_pin_configure_dt(SPEC, GPIO_INPUT);                                                  \
    if (err != 0)                                                                                   \
    {                                                                                               \
      LOG_ERR("Button " #NAME " GPIO configure failed (err: %d)", err);                             \
      return err;                                                                                   \
    }                                                                                               \
                                                                                                    \
    err = gpio_pin_interrupt_configure_dt(SPEC, GPIO_INT_EDGE_BOTH);                                \
    if (err != 0)                                                                                   \
    {                                                                                               \
      LOG_ERR("Button " #NAME " GPIO interrupt configure failed (err: %d)", err);                   \
      return err;                                                                                   \
    }                                                                                               \
                                                                                                    \
    gpio_init_callback(&NAME##_callback, NAME##_callback_handler, BIT(PTR_TO_VAL(SPEC).pin));       \
                                                                                                    \
    err = gpio_add_callback(PTR_TO_VAL(SPEC).port, &NAME##_callback);                               \
    if (err != 0)                                                                                   \
    {                                                                                               \
      LOG_ERR("Button " #NAME " GPIO add callback failed (err: %d)", err);                          \
      return err;                                                                                   \
    }                                                                                               \
                                                                                                    \
    k_timer_init(&NAME##_ten_seconds_timer, NAME##_ten_seconds_timer_expired, NULL);                \
    k_timer_init(&NAME##_fifteen_seconds_timer, NAME##_fifteen_seconds_timer_expired, NULL);        \
    k_timer_init(&NAME##_debounce_timer, NAME##_debounce_timer_expired, NULL);                      \
    k_work_init(&NAME##_pressed_work, NAME##_pressed_work_handler);                                 \
    k_work_init(&NAME##_pressed_ten_seconds_work, NAME##_pressed_ten_seconds_work_handler);         \
    k_work_init(&NAME##_pressed_fifteen_seconds_work, NAME##_pressed_fifteen_seconds_work_handler); \
                                                                                                    \
    LOG_DBG("Button " #NAME " configured");                                                         \
                                                                                                    \
    return 0;                                                                                       \
  }

#endif /* BUTTON_H */
