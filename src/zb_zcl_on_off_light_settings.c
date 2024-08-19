#include "zb_zcl_on_off_light_settings.h"

LOG_MODULE_DECLARE(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

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

// zb_bool_t zb_zcl_process_openvsh_specific_commands_srv(zb_uint8_t param)
// {
//   LOG_DBG("zb_zcl_process_openvsh_specific_commands_srv %u", param);

//   if (ZB_ZCL_GENERAL_GET_CMD_LISTS_PARAM == param)
//   {
//     // ZCL_CTX().zb_zcl_cluster_cmd_list = &gs_on_off_server_cmd_list;
//     return ZB_TRUE;
//   }

//   return ZB_FALSE;
// }

void zb_zcl_on_off_light_settings_init_server()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS,
                              ZB_ZCL_CLUSTER_SERVER_ROLE,
                              check_value_on_off_light_settings_server,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}

// zb_bool_t zb_zcl_process_openvsh_specific_commands_cli(zb_uint8_t param)
// {
//   LOG_DBG("zb_zcl_process_openvsh_specific_commands_cli %u", param);

//   if (ZB_ZCL_GENERAL_GET_CMD_LISTS_PARAM == param)
//   {
//     // ZCL_CTX().zb_zcl_cluster_cmd_list = &gs_on_off_client_cmd_list;
//     return ZB_TRUE;
//   }

//   return ZB_FALSE;
// }

void zb_zcl_on_off_light_settings_init_client()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS,
                              ZB_ZCL_CLUSTER_CLIENT_ROLE,
                              (zb_zcl_cluster_check_value_t)NULL,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}
