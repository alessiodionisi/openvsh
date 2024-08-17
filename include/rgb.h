#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

namespace openvsh
{
  struct RGBColor
  {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    RGBColor(uint8_t red, uint8_t green, uint8_t blue);

    static const RGBColor Rose;
    static const RGBColor Magenta;
    static const RGBColor Violet;
    static const RGBColor Blue;
    static const RGBColor Azure;
    static const RGBColor Cyan;
    static const RGBColor SpringGreen;
    static const RGBColor Green;
    static const RGBColor Chartreuse;
    static const RGBColor Yellow;
    static const RGBColor Orange;
    static const RGBColor Red;
    static const RGBColor White;
    static const RGBColor None;
  };

  class RGB
  {
  public:
    int configure(pwm_dt_spec *red_spec, pwm_dt_spec *green_spec, pwm_dt_spec *blue_spec);
    void set_color(uint8_t red, uint8_t green, uint8_t blue);
    void set_color(RGBColor color);
    void set_brightness(double brightness);
    void start_fading();
    void stop_fading();

  private:
    struct pwm_dt_spec *red_spec_;
    struct pwm_dt_spec *green_spec_;
    struct pwm_dt_spec *blue_spec_;
    double brightness_ = 0.0;
    uint8_t red_ = 0;
    uint8_t green_ = 0;
    uint8_t blue_ = 0;
    struct k_timer fading_timer_;
    bool fading_increase_;

    void update();

    static void fading_timer_expired(struct k_timer *timer);
  };
}
