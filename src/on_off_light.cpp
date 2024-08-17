#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#include "on_off_light.h"

LOG_MODULE_REGISTER(openvsh_on_off_light, CONFIG_OPENVSH_LOG_LEVEL);

typedef struct on_off_light_attrs_s
{
  zb_zcl_basic_attrs_ext_t basic_attrs;
  zb_zcl_identify_attrs_t identify_attrs;
  zb_zcl_groups_attrs_t groups_attrs;
  zb_zcl_scenes_attrs_t scenes_attrs;
  zb_zcl_on_off_attrs_t on_off_attrs;
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

ZB_HA_DECLARE_ON_OFF_LIGHT_CLUSTER_LIST(on_off_light_cluster_list,
                                        basic_attributes,
                                        identify_attributes,
                                        groups_attributes,
                                        scenes_attributes,
                                        on_off_attributes);
ZB_HA_DECLARE_ON_OFF_LIGHT_EP(on_off_light_endpoint, 1, on_off_light_cluster_list);
ZBOSS_DECLARE_DEVICE_CTX_1_EP(on_off_light_ctx, on_off_light_endpoint);

namespace openvsh
{
  // public

  void OnOffLight::configure()
  {
    button_.configure(&button_spec_);
    button_.set_pressed_handler(std::bind(&OnOffLight::button_pressed_handler, this));
    button_.set_pressed_for_ten_seconds_handler(std::bind(&OnOffLight::button_pressed_for_ten_seconds_handler, this));
    button_.set_pressed_for_fifteen_seconds_handler(std::bind(&OnOffLight::button_pressed_for_fifteen_seconds_handler, this));

    rgb_.configure(&pwm_led_red_spec_, &pwm_led_green_spec_, &pwm_led_blue_spec_);

    zigbee = openvsh::Zigbee::get_instance();
    // zigbee->configure();

    ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.mf_name, "OpenVSH", ZB_ZCL_STRING_CONST_SIZE("OpenVSH"));
    ZB_ZCL_SET_STRING_VAL(on_off_light_attrs.basic_attrs.model_id, "On/Off Light", ZB_ZCL_STRING_CONST_SIZE("On/Off Light"));
    on_off_light_attrs.basic_attrs.power_source = ZB_ZCL_BASIC_POWER_SOURCE_MAINS_SINGLE_PHASE;

    if (turned_on_)
    {
      on_off_light_attrs.on_off_attrs.on_off = ZB_ZCL_ON_OFF_IS_ON;
    }
    else
    {
      on_off_light_attrs.on_off_attrs.on_off = ZB_ZCL_ON_OFF_IS_OFF;
    }

    ZB_AF_REGISTER_DEVICE_CTX(&on_off_light_ctx);

    zigbee->set_connected_handler(std::bind(&OnOffLight::zigbee_connected_handler, this));
    zigbee->set_disconnected_handler(std::bind(&OnOffLight::zigbee_disconnected_handler, this));
    zigbee->set_identify_handler(1, std::bind(&OnOffLight::zigbee_identify_handler, this, std::placeholders::_1));

    update();
  }

  void OnOffLight::run()
  {
    zigbee->enable();
  }

  // private

  void OnOffLight::update()
  {
    if (turned_on_)
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

    if (bluetooth_enabled_)
    {
      rgb_.set_color(RGBColor::Blue);
      rgb_.start_fading();

      return;
    }

    if (!zigbee_connected_)
    {
      rgb_.set_color(RGBColor::Red);
      rgb_.start_fading();

      return;
    }

    if (zigbee_identifing_)
    {
      rgb_.set_color(RGBColor::White);
      rgb_.start_fading();

      return;
    }

    rgb_.stop_fading();

    if (turned_on_)
    {
      rgb_.set_color(RGBColor::White);
      rgb_.set_brightness(0);
    }
    else
    {
      rgb_.set_color(RGBColor::White);
      rgb_.set_brightness(0.25);
    }
  }

  void OnOffLight::button_pressed_handler()
  {
    turned_on_ = !turned_on_;
    update();
  }

  void OnOffLight::button_pressed_for_ten_seconds_handler()
  {
    bluetooth_enabled_ = true;
    update();
  }

  void OnOffLight::button_pressed_for_fifteen_seconds_handler()
  {
    bluetooth_enabled_ = false;

    ZB_SCHEDULE_APP_CALLBACK(zb_bdb_reset_via_local_action, 0);
  }

  void OnOffLight::zigbee_identify_handler(bool identifing)
  {
    zigbee_identifing_ = identifing;
    update();
  }

  void OnOffLight::zigbee_connected_handler()
  {
    zigbee_connected_ = true;
    update();
  }

  void OnOffLight::zigbee_disconnected_handler()
  {
    zigbee_connected_ = false;
    update();
  }
}

#endif
