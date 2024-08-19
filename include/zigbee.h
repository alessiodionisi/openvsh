#ifndef ZIGBEE_H
#define ZIGBEE_H 1

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zboss_api.h>
#include <zboss_api_addons.h>

typedef void (*ovsh_zigbee_connection_handler_t)(bool connected);
typedef void (*ovsh_zigbee_status_handler_t)(bool enabled);
typedef void (*ovsh_zigbee_identify_handler_t)(bool identifing);
typedef void (*ovsh_zigbee_zcl_device_callback_handler_t)(zb_uint8_t param);

typedef struct ovsh_zigbee_handlers_s
{
  ovsh_zigbee_status_handler_t status;
  ovsh_zigbee_connection_handler_t connection;
  ovsh_zigbee_identify_handler_t identify;
  ovsh_zigbee_zcl_device_callback_handler_t zcl_device_callback;
} ovsh_zigbee_handlers_t;

int ovsh_zigbee_enable();
void ovsh_zigbee_register_handlers(ovsh_zigbee_handlers_t *handlers);
void ovsh_zigbee_register_device_context(zb_af_device_ctx_t *context);
void ovsh_zigbee_register_identify_handler(uint8_t endpoint);
// void ovsh_zigbee_register_cluster_handler(uint8_t cluster);

#endif /* ZIGBEE_H */
