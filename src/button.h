#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

namespace openvsh
{
  class Button
  {
  public:
    int configure(gpio_dt_spec *spec);

  private:
    struct gpio_dt_spec *spec_;
    struct gpio_callback callback_;
    struct k_timer debounce_timer_;
    struct k_timer ten_seconds_timer_;

    void pressed_handler();
    void pressed_ten_seconds_handler();
    void released_handler();

    static void debounce_timer_expired(struct k_timer *timer);
    static void ten_seconds_timer_expired(struct k_timer *timer);
    static void callback_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins);
  };
}
