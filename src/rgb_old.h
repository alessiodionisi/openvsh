#ifndef RGB_H
#define RGB_H 1

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef enum
{
  RGB_STATE_NONE,
  RGB_STATE_RED_PULSE,
  RGB_STATE_GREEN_PULSE,
  RGB_STATE_BLUE_PULSE,
  RGB_STATE_BLUE_1,
  RGB_STATE_BLUE_2,
  RGB_STATE_BLUE_3,
  RGB_STATE_WHITE_PULSE,
  RGB_STATE_WHITE_1,
  RGB_STATE_WHITE_2,
  RGB_STATE_WHITE_3,
} rgb_state_t;

bool rgb_configure();
void rgb_set_state(rgb_state_t state);

#endif /* RGB_H */
