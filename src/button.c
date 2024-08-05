#include "button.h"

LOG_MODULE_REGISTER(openvsh_button, CONFIG_OPENVSH_LOG_LEVEL);

static button_callbacks_t *registered_callbacks;

static bool configure_gpio(char *alias, const struct gpio_dt_spec *spec,
                           struct gpio_callback *callback, gpio_callback_handler_t handler)
{
  int err;

  if (!gpio_is_ready_dt(spec))
  {
    LOG_ERR("GPIO port for %s is not ready", alias);
    return false;
  }

  err = gpio_pin_configure_dt(spec, GPIO_INPUT);
  if (err < 0)
  {
    LOG_ERR("GPIO pin configure failed for %s (err %d)", alias, err);
    return false;
  }

  err = gpio_pin_interrupt_configure_dt(spec, GPIO_INT_EDGE_BOTH);
  if (err != 0)
  {
    LOG_ERR("GPIO pin interrupt configure failed for %s (err %d)", alias, err);
    return false;
  }

  gpio_init_callback(callback, handler, BIT(spec->pin));

  err = gpio_add_callback(spec->port, callback);
  if (err != 0)
  {
    LOG_ERR("GPIO add callback failed for %s (err %d)", alias, err);
    return false;
  }

  LOG_DBG("Configuration of %s GPIO completed", alias);

  return true;
}

void button_register_callbacks(button_callbacks_t *callbacks)
{
  registered_callbacks = callbacks;
}

#ifdef CONFIG_OPENVSH_DEVICE_LIGHT_SWITCH
static const struct gpio_dt_spec button_1 = GPIO_DT_SPEC_GET(DT_ALIAS(button_1), gpios);
static struct gpio_callback button_1_cb;
static struct k_timer button_1_10_seconds_timer;
static struct k_timer button_1_debounce_timer;

static void button_1_cb_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
  if (gpio_pin_get_dt(&button_1))
  {
    LOG_DBG("Button 1 pressed");

    k_timer_start(&button_1_debounce_timer, K_MSEC(50), K_NO_WAIT);
  }
  else
  {
    LOG_DBG("Button 1 released");

    if (registered_callbacks && registered_callbacks->released)
    {
      registered_callbacks->released(1);
    }

    k_timer_stop(&button_1_10_seconds_timer);
    k_timer_stop(&button_1_debounce_timer);
  }
}

static void button_1_debounce_timer_expired(struct k_timer *timer)
{
  LOG_DBG("Button 1 debounce timer expired");

  if (registered_callbacks && registered_callbacks->pressed)
  {
    registered_callbacks->pressed(1);
  }

  k_timer_start(&button_1_10_seconds_timer, K_SECONDS(10), K_NO_WAIT);
}

static void button_1_10_seconds_timer_expired(struct k_timer *timer)
{
  LOG_DBG("Button 1 10 seconds timer expired");

  if (registered_callbacks && registered_callbacks->pressed_for_10_seconds)
  {
    registered_callbacks->pressed_for_10_seconds(1);
  }
}

bool button_configure()
{
  if (!configure_gpio("button_1", &button_1, &button_1_cb, button_1_cb_handler))
  {
    return false;
  }

  k_timer_init(&button_1_10_seconds_timer, button_1_10_seconds_timer_expired, NULL);
  k_timer_init(&button_1_debounce_timer, button_1_debounce_timer_expired, NULL);

  return true;
}
#endif
