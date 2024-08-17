#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <functional>

namespace openvsh
{
  class Button
  {
  public:
    using PressedHandler = std::function<void()>;
    using ReleasedHandler = std::function<void()>;
    using PressedForTenSecondsHandler = std::function<void()>;
    using PressedForFifteenSecondsHandler = std::function<void()>;

    int configure(gpio_dt_spec *spec);
    void set_pressed_handler(PressedHandler handler);
    void set_released_handler(ReleasedHandler handler);
    void set_pressed_for_ten_seconds_handler(PressedForTenSecondsHandler handler);
    void set_pressed_for_fifteen_seconds_handler(PressedForFifteenSecondsHandler handler);

  private:
    PressedHandler pressed_handler_;
    ReleasedHandler released_handler_;
    PressedForTenSecondsHandler pressed_for_ten_seconds_handler_;
    PressedForFifteenSecondsHandler pressed_for_fifteen_seconds_handler_;
    struct gpio_dt_spec *spec_;
    struct gpio_callback callback_;
    struct k_timer debounce_timer_;
    struct k_timer ten_seconds_timer_;
    struct k_timer fifteen_seconds_timer_;

    void pressed_handler();
    void pressed_for_ten_seconds_handler();
    void pressed_for_fifteen_seconds_handler();
    void released_handler();

    static void debounce_timer_expired(struct k_timer *timer);
    static void ten_seconds_timer_expired(struct k_timer *timer);
    static void fifteen_seconds_timer_expired(struct k_timer *timer);
    static void callback_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins);
  };
}
