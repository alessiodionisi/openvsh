#include "zigbee.h"

LOG_MODULE_REGISTER(openvsh_zigbee, CONFIG_OPENVSH_LOG_LEVEL);

K_THREAD_STACK_DEFINE(zboss_thread_stack, 2048);

extern "C"
{
  void zboss_signal_handler(zb_uint8_t param)
  {
    openvsh::Zigbee::get_instance()->zboss_signal_handler(param);
  }
}

namespace openvsh
{
  // public

  Zigbee::Zigbee() {}

  // void Zigbee::configure()
  // {
  //   LOG_INF("Configuring Zigbee");

  //   LOG_DBG("Zigbee configured");
  // }

  void Zigbee::enable()
  {
    if (zboss_thread_id_ != NULL)
    {
      return;
    }

    LOG_INF("Enabling Zigbee");

    zboss_thread_id_ = k_thread_create(&zboss_thread_,
                                       zboss_thread_stack, K_THREAD_STACK_SIZEOF(zboss_thread_stack),
                                       (k_thread_entry_t)zboss_thread, NULL, NULL, NULL,
                                       0, 0, K_NO_WAIT);
    k_thread_name_set(zboss_thread_id_, "zboss");

    LOG_DBG("Zigbee enabled");
  }

  void Zigbee::set_connected_handler(ConnectedHandler handler)
  {
    connected_handler_ = handler;
  }

  void Zigbee::set_disconnected_handler(DisconnectedHandler handler)
  {
    disconnected_handler_ = handler;
  }

  void Zigbee::set_identify_handler(uint8_t endpoint, IdentifyHandler handler)
  {
    identify_handler_ = handler;
    ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(endpoint, zboss_identify_handler);
  }

  void Zigbee::zboss_signal_handler(zb_uint8_t param)
  {
    zb_ret_t err;

    zb_zdo_app_signal_hdr_t *signal_header = NULL;
    zb_zdo_app_signal_type_t signal_type = zb_get_app_signal(param, &signal_header);
    zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(param);

    switch (signal_type)
    {
    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      LOG_DBG("ZB_BDB_SIGNAL_DEVICE_FIRST_START (status: %d)", status);

      if (status != 0)
      {
        if (disconnected_handler_)
        {
          disconnected_handler_();
        }

        LOG_ERR("Zigbee initialization for the first time failed (status: %d)", status);
        break;
      }

      if (connected_handler_)
      {
        connected_handler_();
      }

      LOG_INF("Zigbee started for the first time");
      break;

    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
      LOG_DBG("ZB_BDB_SIGNAL_DEVICE_REBOOT (status: %d)", status);

      if (status != 0)
      {
        if (disconnected_handler_)
        {
          disconnected_handler_();
        }

        LOG_ERR("Zigbee initialization using NVRAM data failed (status: %d)", status);
        break;
      }

      if (connected_handler_)
      {
        connected_handler_();
      }

      LOG_INF("Zigbee started using NVRAM data");
      break;

    case ZB_BDB_SIGNAL_STEERING:
      LOG_DBG("ZB_BDB_SIGNAL_STEERING (status: %d)", status);

      if (status != 0)
      {
        if (disconnected_handler_)
        {
          disconnected_handler_();
        }

        LOG_ERR("BDB network steering failed (status: %d)", status);
        break;
      }

      if (connected_handler_)
      {
        connected_handler_();
      }
      break;

    case ZB_ZDO_SIGNAL_LEAVE:
    {
      zb_zdo_signal_leave_params_t *leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(signal_header, zb_zdo_signal_leave_params_t);
      LOG_DBG("ZB_ZDO_SIGNAL_LEAVE (status: %d, leave type: %d)", status, leave_params->leave_type);

      if (disconnected_handler_)
      {
        disconnected_handler_();
      }

      if (leave_params->leave_type == ZB_NWK_LEAVE_TYPE_RESET)
      {
        bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
      }
      break;
    }

    case ZB_COMMON_SIGNAL_CAN_SLEEP:
    {
      zb_sleep_now();
      break;
    }

    case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
      LOG_DBG("ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY (status: %d)", status);

      if (disconnected_handler_)
      {
        disconnected_handler_();
      }

      if (status != 0)
      {
        LOG_INF("Zigbee configuration is not present or invalid (status: %d)", status);
        break;
      }

      LOG_INF("Zigbee configuration successfully loaded");
      break;

    default:
      LOG_DBG("Unknown signal %u (status %d)", signal_type, status);
      break;
    }

    if (param)
    {
      zb_buf_free(param);
    }
  }

  // private

  void Zigbee::zboss_thread()
  {
    zb_ret_t err;

    err = zboss_start();
    if (err != 0)
    {
      LOG_ERR("Zigbee start failed (err: %d)", err);
      return;
    }

    zboss_main_loop();
  }

  void Zigbee::zboss_identify_handler(zb_uint8_t param)
  {
    LOG_DBG("Identify command received (identifing: %u)", param);

    if (openvsh::Zigbee::get_instance()->identify_handler_)
    {
      openvsh::Zigbee::get_instance()->identify_handler_(param);
    }
  }

  Zigbee *Zigbee::instance_ = nullptr;

  Zigbee *Zigbee::get_instance()
  {
    if (instance_ == nullptr)
    {
      instance_ = new Zigbee();
      // LOG_DBG("Zigbee instance initialized");
    }

    return instance_;
  }
}
