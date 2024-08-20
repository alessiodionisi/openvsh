#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#ifndef ZB_ZCL_ON_OFF_LIGHT_SETTINGS_H
#define ZB_ZCL_ON_OFF_LIGHT_SETTINGS_H 1

#include <zboss_api.h>

enum zb_zcl_on_off_light_settings_attr_e
{
  ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_COLOR_ID = 0x0000,
  ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_ON_ID = 0x0001,
  ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_OFF_ID = 0x0002,
};

typedef struct zb_zcl_on_off_light_settings_attrs_s
{
  zb_uint8_t led_color;
  zb_uint8_t led_brightness_when_on;
  zb_uint8_t led_brightness_when_off;
} zb_zcl_on_off_light_settings_attrs_t;

#define ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS 0xFC00U

#define ZB_ZCL_ON_OFF_LIGHT_SETTINGS_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001U)

#define ZB_ZCL_DECLARE_ON_OFF_LIGHT_SETTINGS_ATTRIB_LIST(attr_list, led_color, led_brightness_when_on, led_brightness_when_off)                                                                                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ON_OFF_LIGHT_SETTINGS)                                                                                                                        \
  ZB_ZCL_SET_MANUF_SPEC_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_COLOR_ID, ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_WRITE, ZB_DEFAULT_MANUFACTURER_CODE, (led_color))                             \
  ZB_ZCL_SET_MANUF_SPEC_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_ON_ID, ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_WRITE, ZB_DEFAULT_MANUFACTURER_CODE, (led_brightness_when_on))   \
  ZB_ZCL_SET_MANUF_SPEC_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_OFF_ID, ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_WRITE, ZB_DEFAULT_MANUFACTURER_CODE, (led_brightness_when_off)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

void zb_zcl_on_off_light_settings_init_server(void);
void zb_zcl_on_off_light_settings_init_client(void);
#define ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS_SERVER_ROLE_INIT zb_zcl_on_off_light_settings_init_server
#define ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS_CLIENT_ROLE_INIT zb_zcl_on_off_light_settings_init_client

#endif /* ZB_ZCL_ON_OFF_LIGHT_SETTINGS_H */

#endif /* CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT */
