#include "on_off_light.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
static openvsh::OnOffLight on_off_light;
#endif

int main(void)
{
  LOG_INF("Starting openvsh");

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
  LOG_INF("On/off light device");

  on_off_light.configure();
  on_off_light.run();
#endif

  return 0;
}
