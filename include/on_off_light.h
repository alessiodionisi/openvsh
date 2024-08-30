#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#ifndef ON_OFF_LIGHT_H
#define ON_OFF_LIGHT_H 1

#include "button.h"
#include "rgb.h"
#include "relay.h"
#include "zigbee.h"
#include "bluetooth.h"
#include "zb_ha_on_off_light.h"

#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

OVSH_BUTTON_DECLARE_HEADER(button);
OVSH_RGB_DECLARE_HEADER(rgb);
OVSH_RELAY_DECLARE_HEADER(relay_1);
OVSH_RELAY_DECLARE_HEADER(relay_2);

void on_off_light_configure_and_start();

#endif /* ON_OFF_LIGHT_H */

#endif /* CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT */
