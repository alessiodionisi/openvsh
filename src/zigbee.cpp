#include "zigbee.h"

LOG_MODULE_REGISTER(openvsh_zigbee, CONFIG_OPENVSH_LOG_LEVEL);

K_THREAD_STACK_DEFINE(zboss_thread_stack, CONFIG_ZBOSS_DEFAULT_THREAD_STACK_SIZE);

extern "C"
{
  void zboss_signal_handler(zb_uint8_t param)
  {
    openvsh::Zigbee::zboss_signal_handler(param);

    // zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    // zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &p_sg_p);
    // zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(param);
  }
}

namespace openvsh
{
  Zigbee::Zigbee()
  {
  }

  void Zigbee::enable()
  {
    // zigbee_enable();
    zboss_thread_id_ = k_thread_create(&zboss_thread_,
                                       zboss_thread_stack,
                                       K_THREAD_STACK_SIZEOF(zboss_thread_stack),
                                       (k_thread_entry_t)zboss_thread,
                                       NULL, NULL, NULL,
                                       CONFIG_ZBOSS_DEFAULT_THREAD_PRIORITY,
                                       0, K_NO_WAIT);
    k_thread_name_set(&zboss_thread_, "zboss");
  }

  void Zigbee::zboss_thread()
  {
  }

  void Zigbee::zboss_signal_handler(zb_uint8_t param)
  {
    zb_zdo_app_signal_hdr_t *signal_header = NULL;
    zb_zdo_app_signal_type_t signal_type = zb_get_app_signal(param, &signal_header);
    zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(param);

    switch (signal_type)
    {
    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
      LOG_DBG("ZB_BDB_SIGNAL_DEVICE_FIRST_START (status %d)", status);
      break;

    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
      LOG_DBG("ZB_BDB_SIGNAL_DEVICE_REBOOT (status %d)", status);
      break;

    case ZB_BDB_SIGNAL_STEERING:
      LOG_DBG("ZB_BDB_SIGNAL_STEERING (status %d)", status);
      break;

    case ZB_ZDO_SIGNAL_LEAVE:
      LOG_DBG("ZB_ZDO_SIGNAL_LEAVE (status %d)", status);
      break;

    default:
      break;
    }

    // ZB_ERROR_CHECK(zigbee_default_signal_handler(param));

    if (param)
    {
      zb_buf_free(param);
    }
  }

  Zigbee *Zigbee::instance = nullptr;

  Zigbee *Zigbee::get_instance()
  {
    if (instance == nullptr)
    {
      instance = new Zigbee();
    }

    return instance;
  }
}
