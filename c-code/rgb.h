#ifndef RGB_H
#define RGB_H 1

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

ovsh_rgb_color_t ovsh_rgb_color_rose = {
    .red = 255,
    .green = 0,
    .blue = 128,
};

ovsh_rgb_color_t ovsh_rgb_color_magenta = {
    .red = 255,
    .green = 0,
    .blue = 255,
};

// typedef enum
// {
//   RGB_STATE_NONE,
//   RGB_STATE_RED_PULSE,
//   RGB_STATE_RED_1,
//   RGB_STATE_RED_2,
//   RGB_STATE_RED_3,
//   RGB_STATE_GREEN_PULSE,
//   RGB_STATE_GREEN_1,
//   RGB_STATE_GREEN_2,
//   RGB_STATE_GREEN_3,
//   RGB_STATE_BLUE_PULSE,
//   RGB_STATE_BLUE_1,
//   RGB_STATE_BLUE_2,
//   RGB_STATE_BLUE_3,
//   RGB_STATE_WHITE_PULSE,
//   RGB_STATE_WHITE_1,
//   RGB_STATE_WHITE_2,
//   RGB_STATE_WHITE_3,
//   RGB_STATE_YELLOW_1,
//   RGB_STATE_YELLOW_2,
//   RGB_STATE_YELLOW_3,
//   RGB_STATE_ORANGE_1,
//   RGB_STATE_ORANGE_2,
//   RGB_STATE_ORANGE_3,
//   RGB_STATE_MAGENTA_1,
//   RGB_STATE_MAGENTA_2,
//   RGB_STATE_MAGENTA_3,
//   RGB_STATE_CYAN_1,
//   RGB_STATE_CYAN_2,
//   RGB_STATE_CYAN_3,
// } ovsh_rgb_state_t;

#define RGB_DECLARE(NAME, SPEC)                                                       \
  static struct k_timer NAME##_fading_timer;                                          \
  static bool NAME##_fading_increasing = false;                                       \
  static bool NAME##_fading = false;                                                  \
  static uint8_t NAME##_red = 0;                                                      \
  static uint8_t NAME##_green = 0;                                                    \
  static uint8_t NAME##_blue = 0;                                                     \
  static float NAME##_brightness = 0.0;                                               \
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
      LOG_ERR(#NAME " PWM set failed (err: %d)", err);                                \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_green, pulse_green);                  \
    if (err != 0)                                                                     \
    {                                                                                 \
      LOG_ERR(#NAME " PWM set failed (err: %d)", err);                                \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_blue, pulse_blue);                    \
    if (err != 0)                                                                     \
    {                                                                                 \
      LOG_ERR(#NAME " PWM set failed (err: %d)", err);                                \
      return;                                                                         \
    }                                                                                 \
  }                                                                                   \
                                                                                      \
  static void NAME##_set_color(uint8_t red, uint8_t green, uint8_t blue)              \
  {                                                                                   \
    NAME##_red = red;                                                                 \
    NAME##_green = green;                                                             \
    NAME##_blue = blue;                                                               \
    NAME##_update();                                                                  \
  }                                                                                   \
                                                                                      \
  static void NAME##_set_color(ovsh_rgb_color_t color)                                \
  {                                                                                   \
    NAME##_red = color.red;                                                           \
    NAME##_green = color.green;                                                       \
    NAME##_blue = color.blue;                                                         \
    NAME##_update();                                                                  \
  }                                                                                   \
                                                                                      \
  static void NAME##_set_brightness(float brightness)                                 \
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
  static void NAME##_fading_timer_expired()                                           \
  {                                                                                   \
    float step = 0.01;                                                                \
                                                                                      \
    if (NAME##_fading_increasing)                                                     \
    {                                                                                 \
      NAME##_brightness += step;                                                      \
                                                                                      \
      if (NAME##_brightness > 1.0)                                                    \
      {                                                                               \
        NAME##_brightness = 1.0;                                                      \
        NAME##_fading_increasing = false;                                             \
      }                                                                               \
                                                                                      \
      NAME##_update();                                                                \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
      NAME##_brightness -= step;                                                      \
                                                                                      \
      if (NAME##_brightness < 0.0)                                                    \
      {                                                                               \
        NAME##_brightness = 0.0;                                                      \
        NAME##_fading_increasing = true;                                              \
      }                                                                               \
                                                                                      \
      NAME##_update();                                                                \
    }                                                                                 \
  }                                                                                   \
                                                                                      \
  static void NAME##_start_fading()                                                   \
  {                                                                                   \
    if (NAME##_fading)                                                                \
    {                                                                                 \
      return;                                                                         \
    }                                                                                 \
                                                                                      \
    NAME##_fading = true;                                                             \
    NAME##_fading_increasing = true;                                                  \
    NAME##_brightness = 0.0;                                                          \
    NAME##_update();                                                                  \
                                                                                      \
    k_timer_start(&NAME##_fading_timer, K_MSEC(10), K_MSEC(10));                      \
  }                                                                                   \
                                                                                      \
  static void NAME##_stop_fading()                                                    \
  {                                                                                   \
    if (NAME##_fading)                                                                \
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
    LOG_INF("Configuring " #NAME);                                                    \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_red))                                   \
    {                                                                                 \
      LOG_ERR(#NAME " PWM is not ready");                                             \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_green))                                 \
    {                                                                                 \
      LOG_ERR(#NAME " PWM is not ready");                                             \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_blue))                                  \
    {                                                                                 \
      LOG_ERR(#NAME " PWM is not ready");                                             \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_red))                                   \
    {                                                                                 \
      LOG_ERR(#NAME " PWM is not ready");                                             \
      return 1;                                                                       \
    }                                                                                 \
                                                                                      \
    k_timer_init(&NAME##_fading_timer, NAME##_fading_timer_expired, NULL);            \
                                                                                      \
    LOG_DBG(#NAME " configured");                                                     \
  }

#endif /* RGB_H */
