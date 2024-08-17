#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#pragma once

#include "button.h"
#include "rgb.h"
#include "zigbee.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

namespace openvsh
{
  class OnOffLight
  {
  public:
    void configure();
    void run();

  private:
    Button button_;
    RGB rgb_;

    struct pwm_dt_spec pwm_led_red_spec_ = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_red));
    struct pwm_dt_spec pwm_led_green_spec_ = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_green));
    struct pwm_dt_spec pwm_led_blue_spec_ = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_blue));
    struct gpio_dt_spec relay_1_spec_ = GPIO_DT_SPEC_GET(DT_ALIAS(relay_1), gpios);
    struct gpio_dt_spec relay_2_spec_ = GPIO_DT_SPEC_GET(DT_ALIAS(relay_2), gpios);
    struct gpio_dt_spec button_spec_ = GPIO_DT_SPEC_GET(DT_ALIAS(button), gpios);
  };
}

#endif
