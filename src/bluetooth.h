#ifndef BLUETOOTH_H
#define BLUETOOTH_H 1

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef void (*bluetooth_connected_cb_t)();
typedef void (*bluetooth_disconnected_cb_t)();
typedef void (*bluetooth_enabled_cb_t)();
typedef void (*bluetooth_disabled_cb_t)();

struct bluetooth_callbacks
{
  bluetooth_connected_cb_t connected;
  bluetooth_disconnected_cb_t disconnected;
  bluetooth_enabled_cb_t enabled;
  bluetooth_disabled_cb_t disabled;
};

bool bluetooth_configure();
bool bluetooth_disable();
bool bluetooth_enable();
void bluetooth_register_callbacks(struct bluetooth_callbacks *callbacks);

#endif /* BLUETOOTH_H */
