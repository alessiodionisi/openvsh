#ifndef RGB_H
#define RGB_H 1

#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef struct
{
  struct pwm_dt_spec *pwm_red;
  struct pwm_dt_spec *pwm_green;
  struct pwm_dt_spec *pwm_blue;
} rgb_spec_t;

typedef enum
{
  RGB_STATE_NONE,
  RGB_STATE_RED_PULSE,
  RGB_STATE_RED_1,
  RGB_STATE_RED_2,
  RGB_STATE_RED_3,
  RGB_STATE_GREEN_PULSE,
  RGB_STATE_GREEN_1,
  RGB_STATE_GREEN_2,
  RGB_STATE_GREEN_3,
  RGB_STATE_BLUE_PULSE,
  RGB_STATE_BLUE_1,
  RGB_STATE_BLUE_2,
  RGB_STATE_BLUE_3,
  RGB_STATE_WHITE_PULSE,
  RGB_STATE_WHITE_1,
  RGB_STATE_WHITE_2,
  RGB_STATE_WHITE_3,
  RGB_STATE_YELLOW_1,
  RGB_STATE_YELLOW_2,
  RGB_STATE_YELLOW_3,
  RGB_STATE_ORANGE_1,
  RGB_STATE_ORANGE_2,
  RGB_STATE_ORANGE_3,
  RGB_STATE_MAGENTA_1,
  RGB_STATE_MAGENTA_2,
  RGB_STATE_MAGENTA_3,
  RGB_STATE_CYAN_1,
  RGB_STATE_CYAN_2,
  RGB_STATE_CYAN_3,
} rgb_state_t;

#define RGB_DECLARE(NAME, SPEC)                                                                    \
  K_THREAD_STACK_DEFINE(NAME##_fading_thread_stack, 500);                                          \
  static struct k_thread NAME##_fading_thread;                                                     \
  static k_tid_t NAME##_fading_thread_id;                                                          \
                                                                                                   \
  static uint8_t NAME##_red = 0;                                                                   \
  static uint8_t NAME##_green = 0;                                                                 \
  static uint8_t NAME##_blue = 0;                                                                  \
  static float NAME##_brightness = 0.0;                                                            \
                                                                                                   \
  static void NAME##_update()                                                                      \
  {                                                                                                \
    int err;                                                                                       \
                                                                                                   \
    float scaled_red = NAME##_red * NAME##_brightness;                                             \
    float scaled_green = NAME##_green * NAME##_brightness;                                         \
    float scaled_blue = NAME##_blue * NAME##_brightness;                                           \
                                                                                                   \
    uint32_t pulse_red = PTR_TO_VAL(SPEC).pwm_red->period * (scaled_red / 255);                    \
    uint32_t pulse_green = PTR_TO_VAL(SPEC).pwm_green->period * (scaled_green / 255);              \
    uint32_t pulse_blue = PTR_TO_VAL(SPEC).pwm_blue->period * (scaled_blue / 255);                 \
                                                                                                   \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_red, pulse_red);                                   \
    if (err != 0)                                                                                  \
    {                                                                                              \
      LOG_ERR(#NAME " PWM set failed (err %d)", err);                                              \
      return;                                                                                      \
    }                                                                                              \
                                                                                                   \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_green, pulse_green);                               \
    if (err != 0)                                                                                  \
    {                                                                                              \
      LOG_ERR(#NAME " PWM set failed (err %d)", err);                                              \
      return;                                                                                      \
    }                                                                                              \
                                                                                                   \
    err = pwm_set_pulse_dt(PTR_TO_VAL(SPEC).pwm_blue, pulse_blue);                                 \
    if (err != 0)                                                                                  \
    {                                                                                              \
      LOG_ERR(#NAME " PWM set failed (err %d)", err);                                              \
      return;                                                                                      \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  static void NAME##_set_color(uint8_t red, uint8_t green, uint8_t blue)                           \
  {                                                                                                \
    NAME##_red = red;                                                                              \
    NAME##_green = green;                                                                          \
    NAME##_blue = blue;                                                                            \
    NAME##_update();                                                                               \
  }                                                                                                \
                                                                                                   \
  static void NAME##_set_brightness(float brightness)                                              \
  {                                                                                                \
    NAME##_brightness = brightness;                                                                \
    NAME##_update();                                                                               \
  }                                                                                                \
                                                                                                   \
  static void NAME##_fading_thread_entry()                                                         \
  {                                                                                                \
    float step = 0.01;                                                                             \
    uint8_t delay = 10;                                                                            \
                                                                                                   \
    while (1)                                                                                      \
    {                                                                                              \
      while (NAME##_brightness < 1.0)                                                              \
      {                                                                                            \
        NAME##_brightness += step;                                                                 \
                                                                                                   \
        if (NAME##_brightness > 1.0)                                                               \
          NAME##_brightness = 1.0;                                                                 \
                                                                                                   \
        NAME##_set_brightness(NAME##_brightness);                                                  \
                                                                                                   \
        k_sleep(K_MSEC(delay));                                                                    \
      }                                                                                            \
                                                                                                   \
      while (NAME##_brightness > 0.0)                                                              \
      {                                                                                            \
        NAME##_brightness -= step;                                                                 \
                                                                                                   \
        if (NAME##_brightness < 0.0)                                                               \
          NAME##_brightness = 0.0;                                                                 \
                                                                                                   \
        NAME##_set_brightness(NAME##_brightness);                                                  \
                                                                                                   \
        k_sleep(K_MSEC(delay));                                                                    \
      }                                                                                            \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  static void NAME##_start_fading()                                                                \
  {                                                                                                \
    if (NAME##_fading_thread_id == NULL)                                                           \
    {                                                                                              \
      NAME##_fading_thread_id = k_thread_create(&NAME##_fading_thread, NAME##_fading_thread_stack, \
                                                K_THREAD_STACK_SIZEOF(NAME##_fading_thread_stack), \
                                                (k_thread_entry_t)NAME##_fading_thread_entry,      \
                                                NULL, NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);   \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  static void NAME##_stop_fading()                                                                 \
  {                                                                                                \
    if (NAME##_fading_thread_id != NULL)                                                           \
    {                                                                                              \
      k_thread_abort(NAME##_fading_thread_id);                                                     \
      NAME##_fading_thread_id = NULL;                                                              \
    }                                                                                              \
  }

#define RGB_CONFIGURE(NAME, SPEC)                     \
  {                                                   \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_red))   \
    {                                                 \
      LOG_ERR(#NAME " PWM is not ready");             \
      return 1;                                       \
    }                                                 \
                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_green)) \
    {                                                 \
      LOG_ERR(#NAME " PWM is not ready");             \
      return 1;                                       \
    }                                                 \
                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_blue))  \
    {                                                 \
      LOG_ERR(#NAME " PWM is not ready");             \
      return 1;                                       \
    }                                                 \
                                                      \
    if (!pwm_is_ready_dt(PTR_TO_VAL(SPEC).pwm_red))   \
    {                                                 \
      LOG_ERR(#NAME " PWM is not ready");             \
      return 1;                                       \
    }                                                 \
                                                      \
    LOG_DBG(#NAME " configuration completed");        \
  }

#endif /* RGB_H */
