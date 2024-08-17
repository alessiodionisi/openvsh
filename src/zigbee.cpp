#include "zigbee.h"

#include "zb_ha_on_off_light.h"

LOG_MODULE_REGISTER(openvsh_zigbee, CONFIG_OPENVSH_LOG_LEVEL);

K_THREAD_STACK_DEFINE(zboss_thread_stack, 2048);

extern "C"
{
  void zboss_signal_handler(zb_uint8_t param)
  {
    openvsh::Zigbee::get_instance()->zboss_signal_handler(param);
  }
}

typedef struct test_attrs_s
{
  zb_zcl_basic_attrs_ext_t basic_attrs;
  zb_zcl_identify_attrs_t identify_attrs;
  zb_zcl_groups_attrs_t groups_attrs;
  zb_zcl_scenes_attrs_t scenes_attrs;
  zb_zcl_on_off_attrs_t on_off_attrs;
} test_attrs_t;

test_attrs_t test_attrs;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attributes,
                                     ZB_ZCL_VERSION,
                                     &test_attrs.basic_attrs.app_version,
                                     &test_attrs.basic_attrs.stack_version,
                                     &test_attrs.basic_attrs.hw_version,
                                     &test_attrs.basic_attrs.mf_name,
                                     &test_attrs.basic_attrs.model_id,
                                     &test_attrs.basic_attrs.date_code,
                                     &test_attrs.basic_attrs.power_source,
                                     &test_attrs.basic_attrs.location_id,
                                     &test_attrs.basic_attrs.ph_env,
                                     &test_attrs.basic_attrs.sw_ver);
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attributes,
                                    &test_attrs.identify_attrs.identify_time);
ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attributes,
                                  &test_attrs.groups_attrs.name_support);
ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attributes,
                                  &test_attrs.scenes_attrs.scene_count,
                                  &test_attrs.scenes_attrs.current_scene,
                                  &test_attrs.scenes_attrs.current_group,
                                  &test_attrs.scenes_attrs.scene_valid,
                                  &test_attrs.scenes_attrs.name_support);
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attributes, &test_attrs.on_off_attrs.on_off);

ZB_HA_DECLARE_ON_OFF_LIGHT_CLUSTER_LIST(on_off_light_cluster_list,
                                        basic_attributes,
                                        identify_attributes,
                                        groups_attributes,
                                        scenes_attributes,
                                        on_off_attributes);
ZB_HA_DECLARE_ON_OFF_LIGHT_EP(on_off_light_endpoint, 1, on_off_light_cluster_list);
ZBOSS_DECLARE_DEVICE_CTX_1_EP(test_ctx, on_off_light_endpoint);

namespace openvsh
{
  Zigbee::Zigbee()
  {
  }

  void Zigbee::configure()
  {
    LOG_INF("Configuring Zigbee");

    ZB_ZCL_SET_STRING_VAL(test_attrs.basic_attrs.mf_name, "OpenVSH", ZB_ZCL_STRING_CONST_SIZE("OpenVSH"));
    ZB_ZCL_SET_STRING_VAL(test_attrs.basic_attrs.model_id, "On/Off Light", ZB_ZCL_STRING_CONST_SIZE("On/Off Light"));
    test_attrs.basic_attrs.power_source = ZB_ZCL_BASIC_POWER_SOURCE_MAINS_SINGLE_PHASE;

    ZB_AF_REGISTER_DEVICE_CTX(&test_ctx);

    LOG_DBG("Zigbee configured");
  }

  void Zigbee::enable()
  {
    if (zboss_thread_id_ != NULL)
    {
      return;
    }

    LOG_INF("Enabling Zigbee");

    // zigbee_enable();

    zboss_thread_id_ = k_thread_create(&zboss_thread_,
                                       zboss_thread_stack, K_THREAD_STACK_SIZEOF(zboss_thread_stack),
                                       (k_thread_entry_t)zboss_thread, NULL, NULL, NULL,
                                       0, 0, K_NO_WAIT);
    k_thread_name_set(zboss_thread_id_, "zboss");

    LOG_DBG("Zigbee enabled");
  }

  void Zigbee::zboss_signal_handler(zb_uint8_t param)
  {
    // LOG_DBG("Handling zboss signal");

    zb_ret_t err;

    zb_zdo_app_signal_hdr_t *signal_header = NULL;
    zb_zdo_app_signal_type_t signal_type = zb_get_app_signal(param, &signal_header);
    zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(param);
    // zb_nwk_device_type_t role = zb_get_network_role();

    switch (signal_type)
    {
      // case ZB_BDB_SIGNAL_DEVICE_REBOOT:
      // case ZB_BDB_SIGNAL_STEERING:
      //   if (status != 0)
      //   {
      //     LOG_DBG("Calling connecting callback");
      //   }
      //   else
      //   {
      //     LOG_DBG("Calling connected callback");
      //   }
      //   break;

      // case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      // case ZB_ZDO_SIGNAL_LEAVE:
      //   LOG_DBG("Calling connecting callback");
      //   break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      LOG_DBG("ZB_BDB_SIGNAL_DEVICE_FIRST_START (status %d)", status);

      if (status != 0)
      {
        LOG_ERR("Failed to initialize Zigbee stack for the first time (status %d)", status);
        break;
      }

      LOG_INF("Zigbee started for the first time");
      // LOG_DBG("Starting network steering");

      break;

    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
      LOG_DBG("ZB_BDB_SIGNAL_DEVICE_REBOOT (status %d)", status);

      if (status != 0)
      {
        LOG_ERR("Failed to initialize Zigbee stack using NVRAM data (status %d)", status);
        break;
      }

      LOG_INF("Zigbee started using NVRAM data");

      break;

    case ZB_BDB_SIGNAL_STEERING:
      LOG_DBG("ZB_BDB_SIGNAL_STEERING (status %d)", status);
      break;

    case ZB_ZDO_SIGNAL_LEAVE:
    {
      zb_zdo_signal_leave_params_t *leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(signal_header, zb_zdo_signal_leave_params_t);

      LOG_DBG("ZB_ZDO_SIGNAL_LEAVE (status %d, leave type %d)", status, leave_params->leave_type);

      if (leave_params->leave_type == ZB_NWK_LEAVE_TYPE_RESET)
      {
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
      }

      break;
    }

    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
    {
      LOG_DBG("ZB_ZDO_SIGNAL_SKIP_STARTUP (status %d)", status);

      // zb_ret_t err;

      // err = zb_zcl_set_backward_comp_mode(ZB_ZCL_AUTO_MODE);
      // if (err != 0)
      // {
      //   LOG_ERR("Zigbee set backward compatibility failed (err %d)", err);
      //   break;
      // }

      // err = zb_zcl_set_backward_compatible_statuses_mode(ZB_ZCL_STATUSES_ZCL8_MODE);
      // if (err != 0)
      // {
      //   LOG_ERR("Zigbee set backward compatible status failed (err %d)", err);
      //   break;
      // }

      // LOG_INF("Zigbee stack initialized");

      // zb_bool_t ret;
      // ret = bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION);
      // if (ret == 0)
      // {
      //   LOG_ERR("Zigbee start device commissioning procedure failed");
      // }

      break;
    }

    case ZB_COMMON_SIGNAL_CAN_SLEEP:
    {
      // LOG_DBG("ZB_COMMON_SIGNAL_CAN_SLEEP (status %d)", status);
      zb_sleep_now();
      break;
    }

    case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
      LOG_DBG("ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY (status %d)", status);

      if (status != 0)
      {
        LOG_INF("Zigbee configuration is not present or invalid (status %d)", status);
        break;
      }

      LOG_INF("Zigbee configuration successfully loaded");
      break;

    default:
      LOG_DBG("Unknown signal %u (status %d)", signal_type, status);
      break;
    }

    // err = zigbee_default_signal_handler(param);
    // if (err != 0)
    // {
    //   LOG_ERR("Passing signals to the default Zigbee stack event handler failed (err %d)", err);
    // }

    if (param)
    {
      zb_buf_free(param);
    }
  }

  void Zigbee::zboss_thread()
  {
    zb_ret_t err;

    // ZB_INIT();

    // err = zb_zcl_set_backward_comp_mode(ZB_ZCL_AUTO_MODE);
    // if (err != 0)
    // {
    //   LOG_ERR("Zigbee set backward compatibility failed (err %d)", err);
    //   return;
    // }

    // err = zb_zcl_set_backward_compatible_statuses_mode(ZB_ZCL_STATUSES_ZCL8_MODE);
    // if (err != 0)
    // {
    //   LOG_ERR("Zigbee set backward compatible status failed (err %d)", err);
    //   return;
    // }

    err = zboss_start();
    if (err != 0)
    {
      LOG_ERR("Zigbee start failed (err %d)", err);
      return;
    }

    zboss_main_loop();
  }

  Zigbee *Zigbee::instance_ = nullptr;

  Zigbee *Zigbee::get_instance()
  {
    if (instance_ == nullptr)
    {
      instance_ = new Zigbee();
      LOG_DBG("Zigbee instance initialized");
    }

    return instance_;
  }
}
