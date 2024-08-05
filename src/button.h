#ifndef BUTTON_H
#define BUTTON_H 1

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef void (*button_pressed_cb_t)(int btn);
typedef void (*button_released_cb_t)(int btn);
typedef void (*button_pressed_for_10_seconds_cb_t)(int btn);

typedef struct
{
  button_pressed_cb_t pressed;
  button_released_cb_t released;
  button_pressed_for_10_seconds_cb_t pressed_for_10_seconds;
} button_callbacks_t;

bool button_configure();
void button_register_callbacks(button_callbacks_t *callbacks);

#endif /* BUTTON_H */
