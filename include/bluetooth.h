#ifndef BLUETOOTH_H
#define BLUETOOTH_H 1

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

typedef void (*ovsh_bluetooth_connection_handler_t)(bool connected);
typedef void (*ovsh_bluetooth_status_handler_t)(bool enabled);

typedef struct ovsh_bluetooth_handlers_s
{
  ovsh_bluetooth_connection_handler_t connection;
  ovsh_bluetooth_status_handler_t status;
} ovsh_bluetooth_handlers_t;

int ovsh_bluetooth_disable();
int ovsh_bluetooth_enable();
void ovsh_bluetooth_register_handlers(ovsh_bluetooth_handlers_t *handlers);

#endif /* BLUETOOTH_H */
