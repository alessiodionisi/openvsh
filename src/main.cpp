#include "button.h"
#include "on_off_light.h"
#include "rgb.h"
#include "zigbee.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
static openvsh::OnOffLight on_off_light;
#endif

int main(void)
{
  LOG_INF("Starting OpenVSH");

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
  LOG_INF("Device is On/Off Light");

  on_off_light.configure();
  on_off_light.run();
#endif

  return 0;
}
