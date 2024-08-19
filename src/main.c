#include "bluetooth.h"
#include "on_off_light.h"
#include "zigbee.h"
#include "bluetooth.h"
#include "zb_ha_on_off_light.h"

#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

LOG_MODULE_REGISTER(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
typedef struct on_off_light_attrs_s
{
  zb_zcl_basic_attrs_ext_t basic_attrs;
  zb_zcl_identify_attrs_t identify_attrs;
  zb_zcl_groups_attrs_t groups_attrs;
  zb_zcl_scenes_attrs_t scenes_attrs;
  zb_zcl_on_off_attrs_t on_off_attrs;
  zb_zcl_on_off_light_settings_attrs_t on_off_light_settings_attrs;
} on_off_light_attrs_t;

on_off_light_attrs_t on_off_light_attrs;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attributes,
                                     ZB_ZCL_VERSION,
                                     &on_off_light_attrs.basic_attrs.app_version,
                                     &on_off_light_attrs.basic_attrs.stack_version,
                                     &on_off_light_attrs.basic_attrs.hw_version,
                                     &on_off_light_attrs.basic_attrs.mf_name,
                                     &on_off_light_attrs.basic_attrs.model_id,
                                     &on_off_light_attrs.basic_attrs.date_code,
                                     &on_off_light_attrs.basic_attrs.power_source,
                                     &on_off_light_attrs.basic_attrs.location_id,
                                     &on_off_light_attrs.basic_attrs.ph_env,
                                     &on_off_light_attrs.basic_attrs.sw_ver);
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attributes,
                                    &on_off_light_attrs.identify_attrs.identify_time);
ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attributes,
                                  &on_off_light_attrs.groups_attrs.name_support);
ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attributes,
                                  &on_off_light_attrs.scenes_attrs.scene_count,
                                  &on_off_light_attrs.scenes_attrs.current_scene,
                                  &on_off_light_attrs.scenes_attrs.current_group,
                                  &on_off_light_attrs.scenes_attrs.scene_valid,
                                  &on_off_light_attrs.scenes_attrs.name_support);
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attributes, &on_off_light_attrs.on_off_attrs.on_off);

ZB_ZCL_DECLARE_ON_OFF_LIGHT_SETTINGS_ATTRIB_LIST(on_off_light_settings_attributes,
                                                 &on_off_light_attrs.on_off_light_settings_attrs.led_color,
                                                 &on_off_light_attrs.on_off_light_settings_attrs.led_brightness_when_on,
                                                 &on_off_light_attrs.on_off_light_settings_attrs.led_brightness_when_off);

ZB_HA_DECLARE_ON_OFF_LIGHT_CLUSTER_LIST(on_off_light_cluster_list,
                                        basic_attributes,
                                        identify_attributes,
                                        groups_attributes,
                                        scenes_attributes,
                                        on_off_attributes,
                                        on_off_light_settings_attributes);
ZB_HA_DECLARE_ON_OFF_LIGHT_EP(on_off_light_endpoint, 1, on_off_light_cluster_list);

ZBOSS_DECLARE_DEVICE_CTX_1_EP(on_off_light_ctx, on_off_light_endpoint);

static bool turned_on = false;
static bool zigbee_identifing = false;
static bool zigbee_connected = false;
static bool bluetooth_enabled = false;
static bool bluetooth_connected = false;
static uint8_t settings_led_color = OVSH_RGB_COLOR_WHITE;
static uint8_t settings_led_brightness_when_on = OVSH_RGB_BRIGHTNESS_OFF;
static uint8_t settings_led_brightness_when_off = OVSH_RGB_BRIGHTNESS_BRIGHT;

static void save_settings()
{
  int err;

  err = settings_save_one("openvsh/on_off_light/settings_led_color", &settings_led_color, sizeof(settings_led_color));
  if (err != 0)
  {
    LOG_ERR("Writing settings_led_color value to settings failed (err: %d)", err);
  }

  err = settings_save_one("openvsh/on_off_light/settings_led_brightness_when_on", &settings_led_brightness_when_on, sizeof(settings_led_brightness_when_on));
  if (err != 0)
  {
    LOG_ERR("Writing settings_led_brightness_when_on value to settings failed (err: %d)", err);
  }

  err = settings_save_one("openvsh/on_off_light/settings_led_brightness_when_off", &settings_led_brightness_when_off, sizeof(settings_led_brightness_when_off));
  if (err != 0)
  {
    LOG_ERR("Writing settings_led_brightness_when_off value to settings failed (err: %d)", err);
  }
}

static void update()
{
  relay_1_set(turned_on);
  relay_2_set(turned_on);

  int err;

  err = settings_save_one("openvsh/on_off_light/turned_on", &turned_on, sizeof(turned_on));
  if (err != 0)
  {
    LOG_ERR("Writing turned_on value to settings failed (err: %d)", err);
  }

  if (turned_on)
  {
    on_off_light_attrs.on_off_attrs.on_off = ZB_ZCL_ON_OFF_IS_ON;
  }
  else
  {
    on_off_light_attrs.on_off_attrs.on_off = ZB_ZCL_ON_OFF_IS_OFF;
  }

  ZB_ZCL_SET_ATTRIBUTE(
      1,
      ZB_ZCL_CLUSTER_ID_ON_OFF,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
      (zb_uint8_t *)&on_off_light_attrs.on_off_attrs.on_off,
      ZB_FALSE);

  if (bluetooth_connected)
  {
    rgb_set_rgb_color(OVSH_RGB_COLOR_BLUE);
    rgb_stop_fading();
    rgb_set_brightness(1.0f);

    return;
  }

  if (bluetooth_enabled)
  {
    rgb_set_rgb_color(OVSH_RGB_COLOR_BLUE);
    rgb_start_fading();

    return;
  }

  if (!zigbee_connected)
  {
    rgb_set_rgb_color(OVSH_RGB_COLOR_RED);
    rgb_start_fading();

    return;
  }

  if (zigbee_identifing)
  {
    rgb_set_rgb_color(OVSH_RGB_COLOR_WHITE);
    rgb_start_fading();

    return;
  }

  rgb_stop_fading();

  if (turned_on)
  {
    rgb_set_rgb_color(settings_led_color);
    rgb_set_rgb_brightness(settings_led_brightness_when_on);
  }
  else
  {
    rgb_set_rgb_color(settings_led_color);
    rgb_set_rgb_brightness(settings_led_brightness_when_off);
  }
}

static void button_pressed_handler()
{
  turned_on = !turned_on;
  update();
}

static void button_pressed_for_ten_seconds_handler()
{
  ovsh_bluetooth_enable();
}

static void button_pressed_for_fifteen_seconds_handler()
{
  ovsh_bluetooth_disable();
  ZB_SCHEDULE_APP_CALLBACK(zb_bdb_reset_via_local_action, 0);
}

static ovsh_button_handlers_t button_handlers = {
    .pressed = button_pressed_handler,
    .pressed_for_ten_seconds = button_pressed_for_ten_seconds_handler,
    .pressed_for_fifteen_seconds = button_pressed_for_fifteen_seconds_handler,
};

static void zigbee_connection_handler(bool connected)
{
  zigbee_connected = connected;
  update();
}

static void zigbee_identify_handler(bool identifing)
{
  zigbee_identifing = identifing;
  update();
}

static void zigbee_zcl_device_callback_handler(zb_uint8_t param)
{
  zb_zcl_device_callback_param_t *device_callback_param = ZB_BUF_GET_PARAM(param, zb_zcl_device_callback_param_t);

  zb_uint16_t cluster_id;
  zb_uint16_t attribute_id;

  device_callback_param->status = RET_OK;

  switch (device_callback_param->device_cb_id)
  {
  case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
    cluster_id = device_callback_param->cb_param.set_attr_value_param.cluster_id;
    attribute_id = device_callback_param->cb_param.set_attr_value_param.attr_id;

    switch (cluster_id)
    {
    case ZB_ZCL_CLUSTER_ID_ON_OFF_LIGHT_SETTINGS:
      switch (attribute_id)
      {
      case ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_COLOR_ID:
      {
        uint8_t value = device_callback_param->cb_param.set_attr_value_param.values.data8;
        settings_led_color = value;
        save_settings();
        update();
        break;
      }

      case ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_ON_ID:
      {
        uint8_t value = device_callback_param->cb_param.set_attr_value_param.values.data8;
        settings_led_brightness_when_on = value;
        save_settings();
        update();
        break;
      }

      case ZB_ZCL_ATTR_ON_OFF_LIGHT_SETTINGS_LED_BRIGHTNESS_WHEN_OFF_ID:
      {
        uint8_t value = device_callback_param->cb_param.set_attr_value_param.values.data8;
        settings_led_brightness_when_off = value;
        save_settings();
        update();
        break;
      }

      default:
        LOG_DBG("Received Zigbee ZCL set attribute value for cluster ON_OFF_LIGHT_SETTINGS with unknown attribute (attribute: %d)", attribute_id);
        device_callback_param->status = RET_NOT_IMPLEMENTED;
        break;
      }
      break;

    case ZB_ZCL_CLUSTER_ID_ON_OFF:
      switch (attribute_id)
      {
      case ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID:
      {
        uint8_t value = device_callback_param->cb_param.set_attr_value_param.values.data8;
        turned_on = value;
        update();
        break;
      }

      default:
        LOG_DBG("Received Zigbee ZCL set attribute value for cluster ON_OFF with unknown attribute (attribute: %d)", attribute_id);
        device_callback_param->status = RET_NOT_IMPLEMENTED;
        break;
      }
      break;

    default:
      LOG_DBG("Received Zigbee ZCL set attribute value for unknown cluster (cluster: %d)", cluster_id);
      device_callback_param->status = RET_NOT_IMPLEMENTED;
      break;
    }
    break;

  default:
    LOG_DBG("Received unknown Zigbee ZCL device callback (device_cb_id: %d)", device_callback_param->device_cb_id);
    device_callback_param->status = RET_NOT_IMPLEMENTED;
    break;
  }
}

static ovsh_zigbee_handlers_t zigbee_handlers = {
    .connection = zigbee_connection_handler,
    .identify = zigbee_identify_handler,
    .zcl_device_callback = zigbee_zcl_device_callback_handler,
};

static void bluetooth_enabled_handler()
{
  bluetooth_enabled = true;
  update();
}

static void bluetooth_disabled_handler()
{
  bluetooth_enabled = false;
  update();
}

static void bluetooth_connected_handler()
{
  bluetooth_connected = true;
  update();
}

static void bluetooth_disconnected_handler()
{
  bluetooth_connected = false;
  ovsh_bluetooth_disable();
}

static ovsh_bluetooth_handlers_t bluetooth_handlers = {
    .enabled = bluetooth_enabled_handler,
    .disabled = bluetooth_disabled_handler,
    .connected = bluetooth_connected_handler,
    .disconnected = bluetooth_disconnected_handler,
};

static int openvsh_settings_set(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg)
{
  const char *next;
  int rc;

  if (settings_name_steq(key, "turned_on", &next) && !next)
  {
    if (len != sizeof(turned_on))
    {
      return -EINVAL;
    }

    rc = read_cb(cb_arg, &turned_on, sizeof(turned_on));
    if (rc >= 0)
    {
      return 0;
    }

    return rc;
  }

  if (settings_name_steq(key, "settings_led_color", &next) && !next)
  {
    if (len != sizeof(settings_led_color))
    {
      return -EINVAL;
    }

    rc = read_cb(cb_arg, &settings_led_color, sizeof(settings_led_color));
    if (rc >= 0)
    {
      return 0;
    }

    return rc;
  }

  if (settings_name_steq(key, "settings_led_brightness_when_on", &next) && !next)
  {
    if (len != sizeof(settings_led_brightness_when_on))
    {
      return -EINVAL;
    }

    rc = read_cb(cb_arg, &settings_led_brightness_when_on, sizeof(settings_led_brightness_when_on));
    if (rc >= 0)
    {
      return 0;
    }

    return rc;
  }

  if (settings_name_steq(key, "settings_led_brightness_when_off", &next) && !next)
  {
    if (len != sizeof(settings_led_brightness_when_off))
    {
      return -EINVAL;
    }

    rc = read_cb(cb_arg, &settings_led_brightness_when_off, sizeof(settings_led_brightness_when_off));
    if (rc >= 0)
    {
      return 0;
    }

    return rc;
  }

  return -ENOENT;
}

static struct settings_handler openvsh_settings_handler = {
    .name = "openvsh/on_off_light",
    .h_set = openvsh_settings_set,
};
#endif

int main(void)
{
  LOG_INF("Starting OpenVSH");

#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT
  LOG_INF("On/Off Light device");

  int err;

  err = settings_subsys_init();
  if (err != 0)
  {
    LOG_ERR("Initialization of settings failed (err: %d)", err);
    return 0;
  }

  err = settings_register(&openvsh_settings_handler);
  if (err != 0)
  {
    LOG_ERR("Settings register handler failed (err: %d)", err);
    return 0;
  }

  err = settings_load();
  if (err != 0)
  {
    LOG_ERR("Load of settings failed (err: %d)", err);
    return 0;
  }

  err = button_configure();
  if (err != 0)
  {
    LOG_ERR("Button configuration failed (err: %d)", err);
    return 0;
  }

  button_register_handlers(&button_handlers);

  err = rgb_configure();
  if (err != 0)
  {
    LOG_ERR("RGB configuration failed (err: %d)", err);
    return 0;
  }

  err = relay_1_configure();
  if (err != 0)
  {
    LOG_ERR("Relay 1 configuration failed (err: %d)", err);
    return 0;
  }

  err = relay_2_configure();
  if (err != 0)
  {
    LOG_ERR("Relay 1 configuration failed (err: %d)", err);
    return 0;
  }

  ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.mf_name, "OpenVSH", ZB_ZCL_STRING_CONST_SIZE("OpenVSH"));
  ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.model_id, "On/Off Light", ZB_ZCL_STRING_CONST_SIZE("On/Off Light"));
  on_off_light_attrs.basic_attrs.power_source = ZB_ZCL_BASIC_POWER_SOURCE_MAINS_SINGLE_PHASE;
  on_off_light_attrs.on_off_attrs.on_off = turned_on;
  on_off_light_attrs.on_off_light_settings_attrs.led_color = settings_led_color;
  on_off_light_attrs.on_off_light_settings_attrs.led_brightness_when_on = settings_led_brightness_when_on;
  on_off_light_attrs.on_off_light_settings_attrs.led_brightness_when_off = settings_led_brightness_when_off;

  ovsh_zigbee_register_device_context(&on_off_light_ctx);
  ovsh_zigbee_register_handlers(&zigbee_handlers);
  ovsh_zigbee_register_identify_handler(1);

  err = ovsh_zigbee_enable();
  if (err != 0)
  {
    LOG_ERR("Zigbee enabling failed (err: %d)", err);
    return 0;
  }

  ovsh_bluetooth_register_handlers(&bluetooth_handlers);

  update();
#endif

  return 0;
}
