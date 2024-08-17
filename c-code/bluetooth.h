#ifndef BLUETOOTH_H
#define BLUETOOTH_H 1

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef void (*ovsh_bluetooth_connected_cb_t)();
typedef void (*ovsh_bluetooth_disconnected_cb_t)();
typedef void (*ovsh_bluetooth_enabled_cb_t)();
typedef void (*ovsh_bluetooth_disabled_cb_t)();

typedef struct ovsh_bluetooth_callbacks_s
{
  ovsh_bluetooth_connected_cb_t connected;
  ovsh_bluetooth_disconnected_cb_t disconnected;
  ovsh_bluetooth_enabled_cb_t enabled;
  ovsh_bluetooth_disabled_cb_t disabled;
} ovsh_bluetooth_callbacks_t;

int ovsh_bluetooth_configure();
int ovsh_bluetooth_disable();
int ovsh_bluetooth_enable();
void ovsh_bluetooth_register_callbacks(ovsh_bluetooth_callbacks_t *callbacks);

#endif /* BLUETOOTH_H */
