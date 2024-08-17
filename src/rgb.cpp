#include "rgb.h"

LOG_MODULE_REGISTER(openvsh_rgb, CONFIG_OPENVSH_LOG_LEVEL);

namespace openvsh
{
  RGBColor::RGBColor(uint8_t red, uint8_t green, uint8_t blue)
  {
    this->red = red;
    this->green = green;
    this->blue = blue;
  }

  const RGBColor RGBColor::Rose = RGBColor(255, 0, 128);
  const RGBColor RGBColor::Magenta = RGBColor(255, 0, 255);
  const RGBColor RGBColor::Violet = RGBColor(128, 0, 255);
  const RGBColor RGBColor::Blue = RGBColor(0, 0, 255);
  const RGBColor RGBColor::Azure = RGBColor(0, 128, 255);
  const RGBColor RGBColor::Cyan = RGBColor(0, 255, 255);
  const RGBColor RGBColor::SpringGreen = RGBColor(0, 255, 128);
  const RGBColor RGBColor::Green = RGBColor(0, 255, 0);
  const RGBColor RGBColor::Chartreuse = RGBColor(128, 255, 0);
  const RGBColor RGBColor::Yellow = RGBColor(255, 255, 0);
  const RGBColor RGBColor::Orange = RGBColor(255, 128, 0);
  const RGBColor RGBColor::Red = RGBColor(255, 0, 0);
  const RGBColor RGBColor::White = RGBColor(255, 255, 255);
  const RGBColor RGBColor::None = RGBColor(0, 0, 0);

  // public

  void RGB::set_color(uint8_t red, uint8_t green, uint8_t blue)
  {
    red_ = red;
    green_ = green;
    blue_ = blue;
    update();
  }

  void RGB::set_color(RGBColor color)
  {
    red_ = color.red;
    green_ = color.green;
    blue_ = color.blue;
    update();
  }

  void RGB::set_brightness(double brightness)
  {
    if (fading_)
    {
      return;
    }

    brightness_ = brightness;
    update();
  }

  void RGB::start_fading()
  {
    if (fading_)
    {
      return;
    }

    fading_ = true;
    fading_increase_ = true;
    brightness_ = 0.0;
    update();

    k_timer_start(&fading_timer_, K_MSEC(10), K_MSEC(10));
  }

  void RGB::stop_fading()
  {
    if (!fading_)
    {
      return;
    }

    fading_ = false;
    k_timer_stop(&fading_timer_);
    update();
  }

  int RGB::configure(pwm_dt_spec *red_spec, pwm_dt_spec *green_spec, pwm_dt_spec *blue_spec)
  {
    red_spec_ = red_spec;
    green_spec_ = green_spec;
    blue_spec_ = blue_spec;

    LOG_INF("Configuring RGB");

    if (!pwm_is_ready_dt(red_spec_) || !pwm_is_ready_dt(green_spec_) || !pwm_is_ready_dt(blue_spec_))
    {
      LOG_ERR("PWM is not ready");
      return 1;
    }

    k_timer_init(&fading_timer_, fading_timer_expired, NULL);

    LOG_DBG("RGB configured");

    return 0;
  }

  // private

  void RGB::update()
  {
    int err;

    double scaled_red = red_ * brightness_;
    double scaled_green = green_ * brightness_;
    double scaled_blue = blue_ * brightness_;

    uint32_t pulse_red = red_spec_->period * (scaled_red / 255);
    uint32_t pulse_green = green_spec_->period * (scaled_green / 255);
    uint32_t pulse_blue = blue_spec_->period * (scaled_blue / 255);

    err = pwm_set_pulse_dt(red_spec_, pulse_red);
    if (err != 0)
    {
      LOG_ERR("PWM set failed (err %d)", err);
      return;
    }

    err = pwm_set_pulse_dt(green_spec_, pulse_green);
    if (err != 0)
    {
      LOG_ERR("PWM set failed (err %d)", err);
      return;
    }

    err = pwm_set_pulse_dt(blue_spec_, pulse_blue);
    if (err != 0)
    {
      LOG_ERR("PWM set failed (err %d)", err);
      return;
    }
  }

  void RGB::fading_timer_expired(struct k_timer *timer)
  {
    RGB *rgb = CONTAINER_OF(timer, class RGB, fading_timer_);

    double step = 0.01;

    if (rgb->fading_increase_)
    {
      rgb->brightness_ += step;

      if (rgb->brightness_ > 1.0)
      {
        rgb->brightness_ = 1.0;
        rgb->fading_increase_ = false;
      }

      rgb->update();
    }
    else
    {
      rgb->brightness_ -= step;

      if (rgb->brightness_ < 0.0)
      {
        rgb->brightness_ = 0.0;
        rgb->fading_increase_ = true;
      }

      rgb->update();
    }
  }
};
