#include <zboss_api.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "bluetooth.h"
#include "button.h"
#include "rgb.h"

LOG_MODULE_REGISTER(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

static const struct gpio_dt_spec relay_1 = GPIO_DT_SPEC_GET(DT_ALIAS(relay_1), gpios);
static const struct gpio_dt_spec relay_2 = GPIO_DT_SPEC_GET(DT_ALIAS(relay_2), gpios);

static struct k_work_delayable enable_bluetooth_work;
static struct k_work_delayable disable_bluetooth_work;
static bool bluetooth_enabled = false;
static bool powered_on = false;

void zboss_signal_handler(zb_bufid_t param)
{
}

static void enable_bluetooth_work_handler(struct k_work *work)
{
  bluetooth_enable();
}

static void disable_bluetooth_work_handler(struct k_work *work)
{
  bluetooth_disable();
}

static void set_rgb_based_on_state()
{
  if (bluetooth_enabled)
  {
    return;
  }

  LOG_INF("Setting RGB based on state");

  rgb_set_state(RGB_STATE_NONE);

  if (!powered_on)
  {
    rgb_set_state(RGB_STATE_WHITE_1);
  }
}

static void bluetooth_connected_handler()
{
  LOG_INF("Bluetooth connected");
  rgb_set_state(RGB_STATE_BLUE_3);
}

static void bluetooth_disconnected_handler()
{
  LOG_INF("Bluetooth disconnected, disabling it");
  bluetooth_disable();
}

static void bluetooth_enabled_handler()
{
  LOG_INF("Bluetooth enabled");
  bluetooth_enabled = true;
  rgb_set_state(RGB_STATE_BLUE_PULSE);
}

static void bluetooth_disabled_handler()
{
  LOG_INF("Bluetooth disabled");
  bluetooth_enabled = false;
  set_rgb_based_on_state();
}

static struct bluetooth_callbacks bluetooth_cb = {
    .connected = bluetooth_connected_handler,
    .disconnected = bluetooth_disconnected_handler,
    .enabled = bluetooth_enabled_handler,
    .disabled = bluetooth_disabled_handler,
};

#ifdef CONFIG_OPENVSH_DEVICE_LIGHT_SWITCH
static void button_pressed_for_10_seconds_handler(int btn)
{
  LOG_INF("Button 10sec timer expired");

  k_work_schedule(&enable_bluetooth_work, K_NO_WAIT);
}

static void button_pressed_handler(int btn)
{
  LOG_INF("Button pressed");

  powered_on = !powered_on;
  gpio_pin_set_dt(&relay_1, powered_on);
  gpio_pin_set_dt(&relay_2, powered_on);
  set_rgb_based_on_state();
}

static button_callbacks_t button_cb = {
    .pressed_for_10_seconds = button_pressed_for_10_seconds_handler,
    .pressed = button_pressed_handler,
};
#endif

int main(void)
{
  int err;

  if (!rgb_configure())
  {
    return 0;
  }

  if (!button_configure())
  {
    return 0;
  }

  button_register_callbacks(&button_cb);

  if (!bluetooth_configure())
  {
    return 0;
  }

  bluetooth_register_callbacks(&bluetooth_cb);

  k_work_init_delayable(&enable_bluetooth_work, enable_bluetooth_work_handler);
  k_work_init_delayable(&disable_bluetooth_work, disable_bluetooth_work_handler);

  if (!gpio_is_ready_dt(&relay_1))
  {
    LOG_ERR("GPIO port for relay_1 is not ready");
    return 0;
  }

  err = gpio_pin_configure_dt(&relay_1, GPIO_OUTPUT_INACTIVE);
  if (err < 0)
  {
    LOG_ERR("GPIO pin configure failed for relay_1 (err %d)", err);
    return 0;
  }

  if (!gpio_is_ready_dt(&relay_2))
  {
    LOG_ERR("GPIO port for relay_2 is not ready");
    return 0;
  }

  err = gpio_pin_configure_dt(&relay_2, GPIO_OUTPUT_INACTIVE);
  if (err < 0)
  {
    LOG_ERR("GPIO pin configure failed for relay_2 (err %d)", err);
    return 0;
  }

  set_rgb_based_on_state();

  return 0;
}
