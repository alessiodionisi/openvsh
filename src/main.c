#include <zboss_api.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "button.h"
#include "relay.h"
#include "rgb.h"
#include "bluetooth.h"

LOG_MODULE_REGISTER(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
static struct pwm_dt_spec pwm_led_red_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_red));
static struct pwm_dt_spec pwm_led_green_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_green));
static struct pwm_dt_spec pwm_led_blue_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_blue));

static rgb_spec_t rgb_spec = {
    .pwm_red = &pwm_led_red_spec,
    .pwm_green = &pwm_led_green_spec,
    .pwm_blue = &pwm_led_blue_spec,
};

static struct gpio_dt_spec relay_1_spec = GPIO_DT_SPEC_GET(DT_ALIAS(relay_1), gpios);
static struct gpio_dt_spec relay_2_spec = GPIO_DT_SPEC_GET(DT_ALIAS(relay_2), gpios);

static struct gpio_dt_spec button_spec = GPIO_DT_SPEC_GET(DT_ALIAS(button), gpios);

static bool light_on = false;

RGB_DECLARE(rgb, &rgb_spec);

RELAY_DECLARE(relay_1, &relay_1_spec);
RELAY_DECLARE(relay_2, &relay_2_spec);

void button_pressed()
{
  LOG_INF("button pressed, toggling light state");

  light_on = !light_on;

  relay_1_set(light_on);
  relay_2_set(light_on);
}

void button_pressed_for_10_seconds()
{
  LOG_INF("button pressed for 10 seconds, enabling bluetooth");

  bluetooth_enable();
}

static button_callbacks_t button_callbacks = {
    .pressed = button_pressed,
    .pressed_for_10_seconds = button_pressed_for_10_seconds,
};

BUTTON_DECLARE(button, &button_spec, &button_callbacks);
#endif

void zboss_signal_handler(zb_bufid_t param)
{
}

int main(void)
{
  bluetooth_configure();

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
  RGB_CONFIGURE(rgb, &rgb_spec);

  RELAY_CONFIGURE(relay_1, &relay_1_spec);
  RELAY_CONFIGURE(relay_2, &relay_2_spec);

  BUTTON_CONFIGURE(button, &button_spec);
#endif

  // rgb_set_color(255, 255, 255);
  // rgb_start_fading();

  return 0;
}
