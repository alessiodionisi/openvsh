#ifndef BLUETOOTH_H
#define BLUETOOTH_H 1

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef void (*ovsh_bluetooth_connected_handler_t)();
typedef void (*ovsh_bluetooth_disconnected_handler_t)();
typedef void (*ovsh_bluetooth_enabled_handler_t)();
typedef void (*ovsh_bluetooth_disabled_handler_t)();

typedef struct ovsh_bluetooth_handlers_s
{
  ovsh_bluetooth_connected_handler_t connected;
  ovsh_bluetooth_disconnected_handler_t disconnected;
  ovsh_bluetooth_enabled_handler_t enabled;
  ovsh_bluetooth_disabled_handler_t disabled;
} ovsh_bluetooth_handlers_t;

int ovsh_bluetooth_disable();
int ovsh_bluetooth_enable();
void ovsh_bluetooth_register_handlers(ovsh_bluetooth_handlers_t *handlers);

#endif /* BLUETOOTH_H */
