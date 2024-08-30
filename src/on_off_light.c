#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#include "on_off_light.h"

LOG_MODULE_DECLARE(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

static struct gpio_dt_spec button_spec = GPIO_DT_SPEC_GET(DT_ALIAS(button), gpios);
OVSH_BUTTON_DECLARE(button, &button_spec);

static struct pwm_dt_spec pwm_led_red_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_red));
static struct pwm_dt_spec pwm_led_green_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_green));
static struct pwm_dt_spec pwm_led_blue_spec = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_blue));
static ovsh_rgb_spec_t rgb_led_spec = {
    .pwm_red = &pwm_led_red_spec,
    .pwm_green = &pwm_led_green_spec,
    .pwm_blue = &pwm_led_blue_spec,
};
OVSH_RGB_DECLARE(rgb_led, &rgb_led_spec);

static struct gpio_dt_spec relay_1_spec = GPIO_DT_SPEC_GET(DT_ALIAS(relay_1), gpios);
static struct gpio_dt_spec relay_2_spec = GPIO_DT_SPEC_GET(DT_ALIAS(relay_2), gpios);
OVSH_RELAY_DECLARE(relay_1, &relay_1_spec);
OVSH_RELAY_DECLARE(relay_2, &relay_2_spec);

typedef struct on_off_light_attrs_s
{
  zb_zcl_basic_attrs_ext_t basic_attrs;
  zb_zcl_identify_attrs_t identify_attrs;
  zb_zcl_groups_attrs_t groups_attrs;
  zb_zcl_scenes_attrs_t scenes_attrs;
  zb_zcl_on_off_attrs_ext_t on_off_attrs;
} on_off_light_attrs_t;

static on_off_light_attrs_t on_off_light_attrs;

#define ZIGBEE_ON_OFF_LIGHT_ENDPOINT_ID 1

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attributes,
                                     &on_off_light_attrs.basic_attrs.zcl_version,
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
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attributes,
                                      &on_off_light_attrs.on_off_attrs.on_off,
                                      &on_off_light_attrs.on_off_attrs.global_scene_ctrl,
                                      &on_off_light_attrs.on_off_attrs.on_off,
                                      &on_off_light_attrs.on_off_attrs.off_wait_time);

ZB_HA_DECLARE_ON_OFF_LIGHT_CLUSTER_LIST(on_off_light_cluster_list,
                                        basic_attributes,
                                        identify_attributes,
                                        groups_attributes,
                                        scenes_attributes,
                                        on_off_attributes);
ZB_HA_DECLARE_ON_OFF_LIGHT_EP(on_off_light_endpoint, ZIGBEE_ON_OFF_LIGHT_ENDPOINT_ID, on_off_light_cluster_list);

ZBOSS_DECLARE_DEVICE_CTX_1_EP(on_off_light_ctx, on_off_light_endpoint);

static bool status_zigbee_identifing = false;
static bool status_zigbee_connected = false;
static bool status_bluetooth_enabled = false;
static bool status_bluetooth_connected = false;
static bool status_turned_on = false;
static uint8_t parameter_led_color = OVSH_RGB_COLOR_WHITE;
static uint8_t parameter_led_brightness_when_on = OVSH_RGB_BRIGHTNESS_OFF;
static uint8_t parameter_led_brightness_when_off = OVSH_RGB_BRIGHTNESS_BRIGHT;

// On/Off Light private functions

static void settings_status_save()
{
  int err;

  err = settings_save_one("openvsh/on_off_light/status_turned_on", &status_turned_on, sizeof(status_turned_on));
  if (err != 0)
  {
    LOG_ERR("Writing status_turned_on value to settings failed (err: %d)", err);
  }
}

static void settings_parameters_save()
{
  int err;

  err = settings_save_one("openvsh/on_off_light/parameter_led_color", &parameter_led_color, sizeof(parameter_led_color));
  if (err != 0)
  {
    LOG_ERR("Writing parameter_led_color value to settings failed (err: %d)", err);
  }

  err = settings_save_one("openvsh/on_off_light/parameter_led_brightness_when_on", &parameter_led_brightness_when_on, sizeof(parameter_led_brightness_when_on));
  if (err != 0)
  {
    LOG_ERR("Writing parameter_led_brightness_when_on value to settings failed (err: %d)", err);
  }

  err = settings_save_one("openvsh/on_off_light/parameter_led_brightness_when_off", &parameter_led_brightness_when_off, sizeof(parameter_led_brightness_when_off));
  if (err != 0)
  {
    LOG_ERR("Writing parameter_led_brightness_when_off value to settings failed (err: %d)", err);
  }
}

static void relays_update_based_on_status()
{
  relay_1_set(status_turned_on);
  relay_2_set(status_turned_on);
}

static void rgb_led_update_based_on_status()
{
  if (status_bluetooth_connected)
  {
    rgb_led_set_rgb_color(OVSH_RGB_COLOR_BLUE);
    rgb_led_stop_fading();
    rgb_led_set_brightness(1.0f);

    return;
  }

  if (status_bluetooth_enabled)
  {
    rgb_led_set_rgb_color(OVSH_RGB_COLOR_BLUE);
    rgb_led_start_fading();

    return;
  }

  if (!status_zigbee_connected)
  {
    rgb_led_set_rgb_color(OVSH_RGB_COLOR_RED);
    rgb_led_start_fading();

    return;
  }

  if (status_zigbee_identifing)
  {
    rgb_led_set_rgb_color(OVSH_RGB_COLOR_WHITE);
    rgb_led_start_fading();

    return;
  }

  rgb_led_stop_fading();

  if (status_turned_on)
  {
    rgb_led_set_rgb_color(parameter_led_color);
    rgb_led_set_rgb_brightness(parameter_led_brightness_when_on);
  }
  else
  {
    rgb_led_set_rgb_color(parameter_led_color);
    rgb_led_set_rgb_brightness(parameter_led_brightness_when_off);
  }
}

static void bluetooth_enabled_set(bool value)
{
  status_bluetooth_enabled = value;
  rgb_led_update_based_on_status();
}

static void bluetooth_connected_set(bool value)
{
  status_bluetooth_connected = value;
  rgb_led_update_based_on_status();
}

static void zigbee_connected_set(bool value)
{
  status_zigbee_connected = value;
  rgb_led_update_based_on_status();
}

static void zigbee_identifing_set(bool value)
{
  status_zigbee_identifing = value;
  rgb_led_update_based_on_status();
}

static void status_turned_on_set(bool value)
{
  status_turned_on = value;

  if (status_turned_on)
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

  relays_update_based_on_status();
  rgb_led_update_based_on_status();
  settings_status_save();
}

// Button handlers

static void button_pressed_handler(uint8_t seconds)
{
  switch (seconds)
  {
  case 15:
    ovsh_bluetooth_disable();
    zb_bdb_reset_via_local_action(0);
    // ZB_SCHEDULE_APP_CALLBACK(zb_bdb_reset_via_local_action, 0);
    // sys_reboot(SYS_REBOOT_COLD);
    break;

  case 10:
    ovsh_bluetooth_enable();
    break;

  default:
    status_turned_on_set(!status_turned_on);
    break;
  }
}

static ovsh_button_handlers_t button_handlers = {
    .pressed = button_pressed_handler,
};

// Bluetooth handlers

static void bluetooth_status_handler(bool enabled)
{
  bluetooth_enabled_set(enabled);
}

static void bluetooth_connection_handler(bool connected)
{
  bluetooth_connected_set(connected);

  if (!connected)
  {
    ovsh_bluetooth_disable();
  }
}

static ovsh_bluetooth_handlers_t bluetooth_handlers = {
    .status = bluetooth_status_handler,
    .connection = bluetooth_connection_handler,
};

// Zigbee handlers

static void zigbee_connection_handler(bool connected)
{
  zigbee_connected_set(connected);
}

static void zigbee_identify_handler(bool identifing)
{
  zigbee_identifing_set(identifing);
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
    case ZB_ZCL_CLUSTER_ID_ON_OFF:
      switch (attribute_id)
      {
      case ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID:
      {
        uint8_t value = device_callback_param->cb_param.set_attr_value_param.values.data8;
        status_turned_on_set(value);
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

// Settings handlers

static int settings_status_and_parameters_read_one(void *data, size_t data_len, size_t len, settings_read_cb read_cb, void *cb_arg)
{
  int ret;

  if (len != data_len)
  {
    return -EINVAL;
  }

  ret = read_cb(cb_arg, data, data_len);
  if (ret >= 0)
  {
    return 0;
  }

  return ret;
}

static int settings_status_and_parameters_set_handler(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg)
{
  const char *next;

  if (settings_name_steq(key, "status_turned_on", &next) && !next)
  {
    return settings_status_and_parameters_read_one(&status_turned_on, sizeof(status_turned_on), len, read_cb, cb_arg);
  }

  if (settings_name_steq(key, "parameter_led_color", &next) && !next)
  {
    return settings_status_and_parameters_read_one(&parameter_led_color, sizeof(parameter_led_color), len, read_cb, cb_arg);
  }

  if (settings_name_steq(key, "parameter_led_brightness_when_on", &next) && !next)
  {
    return settings_status_and_parameters_read_one(&parameter_led_brightness_when_on, sizeof(parameter_led_brightness_when_on), len, read_cb, cb_arg);
  }

  if (settings_name_steq(key, "parameter_led_brightness_when_off", &next) && !next)
  {
    return settings_status_and_parameters_read_one(&parameter_led_brightness_when_off, sizeof(parameter_led_brightness_when_off), len, read_cb, cb_arg);
  }

  return -ENOENT;
}

static struct settings_handler settings_status_and_parameters_handler = {
    .name = "openvsh/on_off_light",
    .h_set = settings_status_and_parameters_set_handler,
};

// On/Off Light public functions

void on_off_light_configure_and_start()
{
  LOG_INF("Configuring On/Off Light device");

  int err;

  err = settings_subsys_init();
  if (err != 0)
  {
    LOG_ERR("Initialization of settings failed (err: %d)", err);
    return;
  }

  err = settings_register(&settings_status_and_parameters_handler);
  if (err != 0)
  {
    LOG_ERR("Settings register handler failed (err: %d)", err);
    return;
  }

  err = settings_load_subtree("openvsh/on_off_light");
  if (err != 0)
  {
    LOG_ERR("Load of settings failed (err: %d)", err);
    return;
  }

  err = button_configure();
  if (err != 0)
  {
    LOG_ERR("Button configuration failed (err: %d)", err);
    return;
  }

  button_register_handlers(&button_handlers);

  err = rgb_led_configure();
  if (err != 0)
  {
    LOG_ERR("RGB configuration failed (err: %d)", err);
    return;
  }

  err = relay_1_configure();
  if (err != 0)
  {
    LOG_ERR("Relay 1 configuration failed (err: %d)", err);
    return;
  }

  err = relay_2_configure();
  if (err != 0)
  {
    LOG_ERR("Relay 1 configuration failed (err: %d)", err);
    return;
  }

  on_off_light_attrs.basic_attrs.zcl_version = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
  on_off_light_attrs.basic_attrs.app_version = ZB_ZCL_BASIC_APPLICATION_VERSION_DEFAULT_VALUE;
  on_off_light_attrs.basic_attrs.stack_version = ZB_ZCL_BASIC_STACK_VERSION_DEFAULT_VALUE;
  on_off_light_attrs.basic_attrs.hw_version = ZB_ZCL_BASIC_HW_VERSION_DEFAULT_VALUE;
  ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.mf_name, "OpenVSH", ZB_ZCL_STRING_CONST_SIZE("OpenVSH"));
  ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.model_id, "On/Off Light", ZB_ZCL_STRING_CONST_SIZE("On/Off Light"));
  ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.date_code, "", 0);
  on_off_light_attrs.basic_attrs.power_source = ZB_ZCL_BASIC_POWER_SOURCE_MAINS_SINGLE_PHASE;
  ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.location_id, "", 0);
  on_off_light_attrs.basic_attrs.ph_env = ZB_ZCL_BASIC_PHYSICAL_ENVIRONMENT_DEFAULT_VALUE;
  ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.sw_ver, "", 0);

  on_off_light_attrs.identify_attrs.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

  on_off_light_attrs.groups_attrs.name_support = ZB_ZCL_ATTR_GROUPS_NAME_NOT_SUPPORTED;

  on_off_light_attrs.scenes_attrs.scene_count = ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE;
  on_off_light_attrs.scenes_attrs.current_scene = ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE;
  on_off_light_attrs.scenes_attrs.scene_valid = ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE;
  on_off_light_attrs.scenes_attrs.name_support = ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE;
  on_off_light_attrs.scenes_attrs.current_group = ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE;

  on_off_light_attrs.on_off_attrs.on_off = status_turned_on;
  on_off_light_attrs.on_off_attrs.global_scene_ctrl = ZB_ZCL_ON_OFF_GLOBAL_SCENE_CONTROL_DEFAULT_VALUE;
  on_off_light_attrs.on_off_attrs.on_time = ZB_ZCL_ON_OFF_ON_TIME_DEFAULT_VALUE;
  on_off_light_attrs.on_off_attrs.off_wait_time = ZB_ZCL_ON_OFF_OFF_WAIT_TIME_DEFAULT_VALUE;

  ovsh_zigbee_register_device_context(&on_off_light_ctx);
  ovsh_zigbee_register_handlers(&zigbee_handlers);
  ovsh_zigbee_register_identify_handler(ZIGBEE_ON_OFF_LIGHT_ENDPOINT_ID);

  LOG_INF("On/Off Light device configured");
  LOG_INF("Starting On/Off Light device");

  err = ovsh_zigbee_enable();
  if (err != 0)
  {
    LOG_ERR("Zigbee enabling failed (err: %d)", err);
    return;
  }

  ovsh_bluetooth_register_handlers(&bluetooth_handlers);

  rgb_led_update_based_on_status();
  relays_update_based_on_status();

  LOG_INF("On/Off Light device started");
}

#endif /* CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT */
