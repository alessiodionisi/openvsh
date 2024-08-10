#ifndef ZIGBEE_H
#define ZIGBEE_H 1

#include <zephyr/logging/log.h>
#include <zboss_api.h>

struct zigbee_callbacks
{
};

bool zigbee_enable();
bool zigbee_configure();
void zigbee_register_callbacks(struct zigbee_callbacks *callbacks);

#endif /* ZIGBEE_H */
