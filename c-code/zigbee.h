#ifndef ZIGBEE_H
#define ZIGBEE_H 1

#include <zephyr/logging/log.h>
#include <zboss_api.h>

typedef struct ovsh_zigbee_callbacks_s
{
} ovsh_zigbee_callbacks_t;

int ovsh_zigbee_enable();
int ovsh_zigbee_disable();
int ovsh_zigbee_configure();
void ovsh_zigbee_register_callbacks(ovsh_zigbee_callbacks_t *callbacks);

#endif /* ZIGBEE_H */
