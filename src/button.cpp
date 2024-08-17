#include "button.h"

LOG_MODULE_REGISTER(openvsh_button, CONFIG_OPENVSH_LOG_LEVEL);

namespace openvsh
{
  // public

  int Button::configure(gpio_dt_spec *spec)
  {
    spec_ = spec;

    LOG_INF("Configuring button");

    int err;

    if (!gpio_is_ready_dt(spec_))
    {
      LOG_ERR("GPIO is not ready");
      return 1;
    }

    err = gpio_pin_configure_dt(spec_, GPIO_INPUT);
    if (err != 0)
    {
      LOG_ERR("GPIO configure failed (err %d)", err);
      return err;
    }

    err = gpio_pin_interrupt_configure_dt(spec_, GPIO_INT_EDGE_BOTH);
    if (err != 0)
    {
      LOG_ERR("GPIO interrupt configure failed (err %d)", err);
      return err;
    }

    gpio_init_callback(&callback_, callback_handler, BIT(spec_->pin));

    err = gpio_add_callback(spec_->port, &callback_);
    if (err != 0)
    {
      LOG_ERR("GPIO add callback failed (err %d)", err);
      return err;
    }

    k_timer_init(&debounce_timer_, debounce_timer_expired, NULL);
    k_timer_init(&ten_seconds_timer_, ten_seconds_timer_expired, NULL);

    LOG_DBG("Button configured");

    return 0;
  }

  // private

  void Button::pressed_ten_seconds_handler()
  {
    LOG_DBG("Button pressed for 10 seconds");
  }

  void Button::pressed_handler()
  {
    LOG_DBG("Button pressed");
  }

  void Button::released_handler()
  {
    LOG_DBG("Button released");
  }

  void Button::debounce_timer_expired(struct k_timer *timer)
  {
    Button *button = CONTAINER_OF(timer, class Button, debounce_timer_);
    button->pressed_handler();

    k_timer_start(&button->ten_seconds_timer_, K_SECONDS(10), K_NO_WAIT);
  }

  void Button::ten_seconds_timer_expired(struct k_timer *timer)
  {
    Button *button = CONTAINER_OF(timer, class Button, debounce_timer_);
    button->pressed_ten_seconds_handler();
  }

  void Button::callback_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
  {
    Button *button = CONTAINER_OF(cb, class Button, callback_);

    if (gpio_pin_get_dt(button->spec_))
    {
      k_timer_start(&button->debounce_timer_, K_MSEC(50), K_NO_WAIT);
    }
    else
    {
      k_timer_stop(&button->debounce_timer_);
      k_timer_stop(&button->ten_seconds_timer_);
      button->released_handler();
    }
  }
};
