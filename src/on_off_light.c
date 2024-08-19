#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#include "on_off_light.h"

LOG_MODULE_DECLARE(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

static struct gpio_dt_spec button_spec = GPIO_DT_SPEC_GET(DT_ALIAS(button), gpios);
OVSH_BUTTON_DECLARE(button, &button_spec);

static struct pwm_dt_spec pwm_led_red_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_red));
static struct pwm_dt_spec pwm_led_green_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_green));
static struct pwm_dt_spec pwm_led_blue_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_blue));
static ovsh_rgb_spec_t rgb_spec = {
    .pwm_red = &pwm_led_red_spec,
    .pwm_green = &pwm_led_green_spec,
    .pwm_blue = &pwm_led_blue_spec,
};
OVSH_RGB_DECLARE(rgb, &rgb_spec);

static struct gpio_dt_spec relay_1_spec = GPIO_DT_SPEC_GET(DT_ALIAS(relay_1), gpios);
static struct gpio_dt_spec relay_2_spec = GPIO_DT_SPEC_GET(DT_ALIAS(relay_2), gpios);
OVSH_RELAY_DECLARE(relay_1, &relay_1_spec);
OVSH_RELAY_DECLARE(relay_2, &relay_2_spec);

#endif /* CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT */
