#include "rgb.h"

LOG_MODULE_REGISTER(openvsh_rgb, CONFIG_OPENVSH_LOG_LEVEL);

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(DT_ALIAS(led_red), gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(DT_ALIAS(led_green), gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_ALIAS(led_blue), gpios);

static const struct pwm_dt_spec pwm_led_green = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_green));
static const struct pwm_dt_spec pwm_led_red = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_red));
static const struct pwm_dt_spec pwm_led_blue = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_blue));

K_THREAD_STACK_DEFINE(pulse_thread_stack, 500);
static struct k_thread pulse_thread;
static k_tid_t pulse_thread_id;

static rgb_state_t current_state = RGB_STATE_NONE;
static bool green_pulse = false;
static bool red_pulse = false;
static bool blue_pulse = false;

static void pulse_thread_func()
{
  LOG_DBG("Pulse thread started");

  bool pulse_in = true;
  uint32_t pulse = 0U;
  uint32_t period = pwm_led_red.period;
  uint32_t step = period / 50U;

  while (1)
  {
    if (red_pulse)
    {
      pwm_set_pulse_dt(&pwm_led_red, pulse);
    }
    else
    {
      pwm_set_pulse_dt(&pwm_led_red, 0U);
    }

    if (green_pulse)
    {
      pwm_set_pulse_dt(&pwm_led_green, pulse);
    }
    else
    {
      pwm_set_pulse_dt(&pwm_led_green, 0U);
    }

    if (blue_pulse)
    {
      pwm_set_pulse_dt(&pwm_led_blue, pulse);
    }
    else
    {
      pwm_set_pulse_dt(&pwm_led_blue, 0U);
    }

    if (pulse_in)
    {
      pulse += step;
      if (pulse >= period)
      {
        pulse = period - step;
        pulse_in = false;
      }
    }
    else
    {
      if (pulse >= step)
      {
        pulse -= step;
      }
      else
      {
        pulse = step;
        pulse_in = true;
      }
    }

    k_sleep(K_MSEC(25U));
  }
}

static void update_pulse_thread()
{
  if (red_pulse || green_pulse || blue_pulse)
  {
    if (pulse_thread_id == NULL)
    {
      LOG_DBG("Starting pulse thread");
      pulse_thread_id = k_thread_create(&pulse_thread, pulse_thread_stack,
                                        K_THREAD_STACK_SIZEOF(pulse_thread_stack),
                                        (k_thread_entry_t)pulse_thread_func,
                                        NULL, NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);
    }
  }
  else
  {
    if (pulse_thread_id != NULL)
    {
      LOG_DBG("Stopping pulse thread");
      k_thread_abort(pulse_thread_id);
      pulse_thread_id = NULL;
    }
  }
}

static bool configure_gpio(const struct gpio_dt_spec *spec, char *alias)
{
  int err;

  if (!gpio_is_ready_dt(spec))
  {
    LOG_ERR("GPIO port for %s is not ready", alias);
    return false;
  }

  err = gpio_pin_configure_dt(spec, GPIO_OUTPUT_INACTIVE);
  if (err < 0)
  {
    LOG_ERR("GPIO pin configure failed for %s (err %d)", alias, err);
    return false;
  }

  LOG_DBG("GPIO %s configured", alias);

  return true;
}

static bool configure_pwm(const struct pwm_dt_spec *spec, char *alias)
{
  if (!pwm_is_ready_dt(spec))
  {
    LOG_ERR("PWM %s is not ready", alias);
    return false;
  }

  LOG_DBG("PWM %s configured", alias);

  return true;
}

static void set_level(const struct pwm_dt_spec *spec, int level)
{
  uint32_t level1 = pwm_led_red.period / 6U;
  uint32_t level2 = level1 * 3U;
  uint32_t level3 = level1 * 6U;

  uint32_t levels[4] = {0U, level1, level2, level3};

  pwm_set_pulse_dt(&pwm_led_red, levels[level]);
}

void rgb_set_state(rgb_state_t state)
{
  if (state == current_state)
  {
    return;
  }

  LOG_DBG("Setting RGB state to %d", state);
  current_state = state;

  // Reset pulse
  red_pulse = false;
  green_pulse = false;
  blue_pulse = false;

  // Reset PWM to zero
  pwm_set_pulse_dt(&pwm_led_red, 0U);
  pwm_set_pulse_dt(&pwm_led_green, 0U);
  pwm_set_pulse_dt(&pwm_led_blue, 0U);

  switch (state)
  {
  case RGB_STATE_NONE:
    update_pulse_thread();
    break;

  case RGB_STATE_RED_PULSE:
    red_pulse = true;
    update_pulse_thread();
    break;

  case RGB_STATE_GREEN_PULSE:
    green_pulse = true;
    update_pulse_thread();
    break;

  case RGB_STATE_BLUE_PULSE:
    blue_pulse = true;
    update_pulse_thread();
    break;

  case RGB_STATE_BLUE_1:
    update_pulse_thread();
    set_level(&pwm_led_blue, 1);
    break;

  case RGB_STATE_BLUE_2:
    update_pulse_thread();
    set_level(&pwm_led_blue, 2);
    break;

  case RGB_STATE_BLUE_3:
    update_pulse_thread();
    set_level(&pwm_led_blue, 3);
    break;

  case RGB_STATE_WHITE_PULSE:
    red_pulse = true;
    green_pulse = true;
    blue_pulse = true;
    update_pulse_thread();
    break;

  case RGB_STATE_WHITE_1:
    update_pulse_thread();
    set_level(&pwm_led_red, 1);
    set_level(&pwm_led_green, 1);
    set_level(&pwm_led_blue, 1);
    break;

  case RGB_STATE_WHITE_2:
    update_pulse_thread();
    set_level(&pwm_led_red, 2);
    set_level(&pwm_led_green, 2);
    set_level(&pwm_led_blue, 2);
    break;

  case RGB_STATE_WHITE_3:
    update_pulse_thread();
    set_level(&pwm_led_red, 3);
    set_level(&pwm_led_green, 3);
    set_level(&pwm_led_blue, 3);
    break;

  default:
    break;
  }
}

bool rgb_configure()
{
  if (!configure_gpio(&led_red, "led_red"))
  {
    return false;
  }

  if (!configure_pwm(&pwm_led_red, "pwm_led_red"))
  {
    return false;
  }

  if (!configure_gpio(&led_green, "led_green"))
  {
    return false;
  }

  if (!configure_pwm(&pwm_led_green, "pwm_led_green"))
  {
    return false;
  }

  if (!configure_gpio(&led_blue, "led_blue"))
  {
    return false;
  }

  if (!configure_pwm(&pwm_led_blue, "pwm_led_blue"))
  {
    return false;
  }

  return true;
}
