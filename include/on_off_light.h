#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#ifndef ON_OFF_LIGHT_H
#define ON_OFF_LIGHT_H 1

#include "button.h"
#include "rgb.h"
#include "relay.h"

#include <zephyr/logging/log.h>

OVSH_BUTTON_DECLARE_HEADER(button);
OVSH_RGB_DECLARE_HEADER(rgb);
OVSH_RELAY_DECLARE_HEADER(relay_1);
OVSH_RELAY_DECLARE_HEADER(relay_2);

#endif /* ON_OFF_LIGHT_H */

#endif /* CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT */
