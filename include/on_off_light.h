#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#pragma once

#include "button.h"
#include "rgb.h"
#include "zigbee.h"
#include "zb_ha_on_off_light.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <functional>

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
    openvsh::Zigbee *zigbee;
    bool turned_on_ = false;
    bool zigbee_identifing_ = false;
    bool zigbee_connected_ = false;
    bool bluetooth_enabled_ = false;

    struct pwm_dt_spec pwm_led_red_spec_ = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_red));
    struct pwm_dt_spec pwm_led_green_spec_ = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_green));
    struct pwm_dt_spec pwm_led_blue_spec_ = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_blue));
    struct gpio_dt_spec relay_1_spec_ = GPIO_DT_SPEC_GET(DT_ALIAS(relay_1), gpios);
    struct gpio_dt_spec relay_2_spec_ = GPIO_DT_SPEC_GET(DT_ALIAS(relay_2), gpios);
    struct gpio_dt_spec button_spec_ = GPIO_DT_SPEC_GET(DT_ALIAS(button), gpios);

    void button_pressed_handler();
    void button_pressed_for_ten_seconds_handler();
    void button_pressed_for_fifteen_seconds_handler();
    void zigbee_connected_handler();
    void zigbee_disconnected_handler();
    void zigbee_identify_handler(bool identifing);
    void update();
  };
}

#endif
