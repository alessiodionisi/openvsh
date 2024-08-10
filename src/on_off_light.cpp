#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#include "on_off_light.h"

LOG_MODULE_REGISTER(openvsh_on_off_light, CONFIG_OPENVSH_LOG_LEVEL);

namespace openvsh
{
  // public
  void OnOffLight::configure()
  {
    button_.configure(&button_spec_);
    rgb_.configure(&pwm_led_red_spec_, &pwm_led_green_spec_, &pwm_led_blue_spec_);

    // rgb_.set_color(RGBColor::White);
    // rgb_.start_fading();
  }

  void OnOffLight::run()
  {
    openvsh::Zigbee::get_instance()->enable();
  }
}

#endif
