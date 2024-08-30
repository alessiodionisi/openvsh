#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
#include "on_off_light.h"
#endif

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

int main(void)
{
  LOG_INF("Starting OpenVSH");

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
  on_off_light_configure_and_start();
#endif

  return 0;
}
