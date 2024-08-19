#ifndef RGB_H
#define RGB_H 1

#include "macro.h"

#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef struct ovsh_rgb_spec_s
{
  struct pwm_dt_spec *pwm_red;
  struct pwm_dt_spec *pwm_green;
  struct pwm_dt_spec *pwm_blue;
} ovsh_rgb_spec_t;

typedef struct ovsh_rgb_color_s
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} ovsh_rgb_color_t;

// static const ovsh_rgb_color_t ovsh_rgb_color_rose = {
//     .red = 255,
//     .green = 0,
//     .blue = 128,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_magenta = {
//     .red = 255,
//     .green = 0,
//     .blue = 255,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_violet = {
//     .red = 128,
//     .green = 0,
//     .blue = 255,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_blue = {
//     .red = 0,
//     .green = 0,
//     .blue = 255,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_azure = {
//     .red = 0,
//     .green = 128,
//     .blue = 255,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_cyan = {
//     .red = 0,
//     .green = 255,
//     .blue = 255,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_springgreen = {
//     .red = 0,
//     .green = 255,
//     .blue = 128,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_green = {
//     .red = 0,
//     .green = 255,
//     .blue = 0,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_chartreuse = {
//     .red = 128,
//     .green = 255,
//     .blue = 0,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_yellow = {
//     .red = 255,
//     .green = 255,
//     .blue = 0,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_orange = {
//     .red = 255,
//     .green = 128,
//     .blue = 0,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_red = {
//     .red = 255,
//     .green = 0,
//     .blue = 0,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_white = {
//     .red = 255,
//     .green = 255,
//     .blue = 255,
// };
// static const ovsh_rgb_color_t ovsh_rgb_color_none = {
//     .red = 0,
//     .green = 0,
//     .blue = 0,
// };

static const ovsh_rgb_color_t ovsh_rgb_colors[] = {
    {.red = 255, .green = 0, .blue = 128},   // rose
    {.red = 255, .green = 0, .blue = 255},   // magenta
    {.red = 128, .green = 0, .blue = 255},   // violet
    {.red = 0, .green = 0, .blue = 255},     // blue
    {.red = 0, .green = 128, .blue = 255},   // azure
    {.red = 0, .green = 255, .blue = 255},   // cyan
    {.red = 0, .green = 255, .blue = 128},   // springgreen
    {.red = 0, .green = 255, .blue = 0},     // green
    {.red = 128, .green = 255, .blue = 0},   // chartreuse
    {.red = 255, .green = 255, .blue = 0},   // yellow
    {.red = 255, .green = 128, .blue = 0},   // orange
    {.red = 255, .green = 0, .blue = 0},     // red
    {.red = 255, .green = 255, .blue = 255}, // white
    {.red = 0, .green = 0, .blue = 0},       // none
};

enum ovsh_rgb_color_e
{
  OVSH_RGB_COLOR_ROSE = 0,
  OVSH_RGB_COLOR_MAGENTA,
  OVSH_RGB_COLOR_VIOLET,
  OVSH_RGB_COLOR_BLUE,
  OVSH_RGB_COLOR_AZURE,
  OVSH_RGB_COLOR_CYAN,
  OVSH_RGB_COLOR_SPRINGGREEN,
  OVSH_RGB_COLOR_GREEN,
  OVSH_RGB_COLOR_CHARTREUSE,
  OVSH_RGB_COLOR_YELLOW,
  OVSH_RGB_COLOR_ORANGE,
  OVSH_RGB_COLOR_RED,
  OVSH_RGB_COLOR_WHITE,
  OVSH_RGB_COLOR_NONE,
};

enum ovsh_rgb_brightness_e
{
  OVSH_RGB_BRIGHTNESS_OFF = 0,
  OVSH_RGB_BRIGHTNESS_DIM,
  OVSH_RGB_BRIGHTNESS_MEDIUM,
  OVSH_RGB_BRIGHTNESS_BRIGHT,
};

#define OVSH_RGB_DECLARE_HEADER(NAME)                 \
  void NAME##_start_fading();                         \
  void NAME##_stop_fading();                          \
  int NAME##_configure();                             \
  void NAME##_set_brightness(float brightness);       \
  void NAME##_set_rgb_color(uint8_t color);           \
  void NAME##_set_rgb_brightness(uint8_t brightness); \
  void NAME##_set_color(uint8_t red, uint8_t green, uint8_t blue);

#define OVSH_RGB_DECLARE(NAME, SPEC)                                                  \
  static struct k_timer NAME##_fading_timer;                                          \
  static bool NAME##_fading_increasing = false;                                       \
  static bool NAME##_fading = false;                                                  \
  static uint8_t NAME##_red = 0;                                                      \
  static uint8_t NAME##_green = 0;                                                    \
  static uint8_t NAME##_blue = 0;                                                     \
  static float NAME##_brightness = 0.0f;                                              \
                                                                                      \
  static void NAME##_update()                                                         \
  {                                                                                   \
    int err;                                                                          \
                                                                                      \
    float scaled_red = NAME##_red * NAME##_brightness;                                \
    float scaled_green = NAME##_green * NAME##_brightness;                            \
    float scaled_blue = NAME##_blue * NAME##_brightness;                              \
                                                                                      \
    uint32_t pulse_red = PTR_TO_VAL(SPEC).pwm_red->period * (scaled_red / 255);       \
    uint32_t pulse_green = PTR_TO_VAL(SPEC).pwm_green->period * (scaled_green / 255); \
    uint32_t pulse_blue = PTR_TO_VAL(SPEC).pwm_blue->period * (scaled_blue / 255);    \
                                                                                      \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_red, pulse_red);                      \
    if (err != 0)                                                                     \
    {                                                                                 \
      LOG_ERR("RGB " #NAME " PWM set failed (err: %d)", err);                         \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_green, pulse_green);                  \
    if (err != 0)                                                                     \
    {                                                                                 \
      LOG_ERR("RGB " #NAME " PWM set failed (err: %d)", err);                         \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_blue, pulse_blue);                    \
    if (err != 0)                                                                     \
    {                                                                                 \
      LOG_ERR("RGB " #NAME " PWM set failed (err: %d)", err);                         \
      return;                                                                         \
    }                                                                                 \
  }                                                                                   \
                                                                                      \
  static void NAME##_fading_timer_expired()                                           \
  {                                                                                   \
    float step = 0.01f;                                                               \
                                                                                      \
    if (NAME##_fading_increasing)                                                     \
    {                                                                                 \
      NAME##_brightness += step;                                                      \
                                                                                      \
      if (NAME##_brightness > 1.0f)                                                   \
      {                                                                               \
        NAME##_brightness = 1.0f;                                                     \
        NAME##_fading_increasing = false;                                             \
      }                                                                               \
                                                                                      \
      NAME##_update();                                                                \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
      NAME##_brightness -= step;                                                      \
                                                                                      \
      if (NAME##_brightness < 0.0f)                                                   \
      {                                                                               \
        NAME##_brightness = 0.0f;                                                     \
        NAME##_fading_increasing = true;                                              \
      }                                                                               \
                                                                                      \
      NAME##_update();                                                                \
    }                                                                                 \
  }                                                                                   \
                                                                                      \
  void NAME##_set_color(uint8_t red, uint8_t green, uint8_t blue)                     \
  {                                                                                   \
    NAME##_red = red;                                                                 \
    NAME##_green = green;                                                             \
    NAME##_blue = blue;                                                               \
    NAME##_update();                                                                  \
  }                                                                                   \
                                                                                      \
  void NAME##_set_rgb_color(uint8_t color)                                            \
  {                                                                                   \
                                                                                      \
    NAME##_set_color(                                                                 \
        ovsh_rgb_colors[color].red,                                                   \
        ovsh_rgb_colors[color].green,                                                 \
        ovsh_rgb_colors[color].blue);                                                 \
  }                                                                                   \
                                                                                      \
  void NAME##_set_brightness(float brightness)                                        \
  {                                                                                   \
    if (NAME##_fading)                                                                \
    {                                                                                 \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    NAME##_brightness = brightness;                                                   \
    NAME##_update();                                                                  \
  }                                                                                   \
                                                                                      \
  void NAME##_set_rgb_brightness(uint8_t brightness)                                  \
  {                                                                                   \
    switch (brightness)                                                               \
    {                                                                                 \
    case OVSH_RGB_BRIGHTNESS_OFF:                                                     \
      NAME##_set_brightness(0.0f);                                                    \
      break;                                                                          \
                                                                                      \
    case OVSH_RGB_BRIGHTNESS_DIM:                                                     \
      NAME##_set_brightness(0.33f);                                                   \
      break;                                                                          \
                                                                                      \
    case OVSH_RGB_BRIGHTNESS_MEDIUM:                                                  \
      NAME##_set_brightness(0.66f);                                                   \
      break;                                                                          \
                                                                                      \
    case OVSH_RGB_BRIGHTNESS_BRIGHT:                                                  \
      NAME##_set_brightness(1.0f);                                                    \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
                                                                                      \
  void NAME##_start_fading()                                                          \
  {                                                                                   \
    if (NAME##_fading)                                                                \
    {                                                                                 \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    NAME##_fading = true;                                                             \
    NAME##_fading_increasing = true;                                                  \
    NAME##_brightness = 0.0f;                                                         \
    NAME##_update();                                                                  \
                                                                                      \
    k_timer_start(&NAME##_fading_timer, K_MSEC(10), K_MSEC(10));                      \
  }                                                                                   \
                                                                                      \
  void NAME##_stop_fading()                                                           \
  {                                                                                   \
    if (!NAME##_fading)                                                               \
    {                                                                                 \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    NAME##_fading = false;                                                            \
    k_timer_stop(&NAME##_fading_timer);                                               \
                                                                                      \
    NAME##_update();                                                                  \
  }                                                                                   \
                                                                                      \
  int NAME##_configure()                                                              \
  {                                                                                   \
                                                                                      \
    LOG_INF("Configuring RGB " #NAME);                                                \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_red))                                   \
    {                                                                                 \
      LOG_ERR("RGB " #NAME " PWM is not ready");                                      \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_green))                                 \
    {                                                                                 \
      LOG_ERR("RGB " #NAME " PWM is not ready");                                      \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_blue))                                  \
    {                                                                                 \
      LOG_ERR("RGB " #NAME " PWM is not ready");                                      \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_red))                                   \
    {                                                                                 \
      LOG_ERR("RGB " #NAME " PWM is not ready");                                      \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    k_timer_init(&NAME##_fading_timer, NAME##_fading_timer_expired, NULL);            \
                                                                                      \
    LOG_DBG("RGB " #NAME " configured");                                              \
                                                                                      \
    return 0;                                                                         \
  }

#endif /* RGB_H */
