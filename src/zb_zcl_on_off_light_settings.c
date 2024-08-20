#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#include "zb_zcl_on_off_light_settings.h"

zb_ret_t check_value_on_off_light_settings_server(zb_uint16_t attr_id, zb_uint8_t endpoint, zb_uint8_t *value)
{
  ZVUNUSED(endpoint);
  ZVUNUSED(attr_id);

  switch (attr_id)
  {
  case ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_COLOR_ID:
    return RET_OK;

  case ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_ON_ID:
    return RET_OK;

  case ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_OFF_ID:
    return RET_OK;

  default:
    return RET_ERROR;
  }
}

void zb_zcl_on_off_light_settings_init_server()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS,
                              ZB_ZCL_CLUSTER_SERVER_ROLE,
                              check_value_on_off_light_settings_server,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}

void zb_zcl_on_off_light_settings_init_client()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS,
                              ZB_ZCL_CLUSTER_CLIENT_ROLE,
                              (zb_zcl_cluster_check_value_t)NULL,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}

#endif /* CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT */
