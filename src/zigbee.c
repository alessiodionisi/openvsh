#include "zigbee.h"

LOG_MODULE_DECLARE(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

static ovsh_zigbee_handlers_t *registered_handlers;
static struct k_thread zigbee_thread;
static k_tid_t zigbee_thread_id = NULL;
static K_THREAD_STACK_DEFINE(zigbee_thread_stack, 2048);

static void zigbee_thread_entry()
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

static void zigbee_identify_handler(zb_uint8_t param)
{
  if (registered_handlers && registered_handlers->identify)
  {
    registered_handlers->identify(param);
  }
}

static void zigbee_zcl_device_callback(zb_uint8_t param)
{
  if (registered_handlers && registered_handlers->zcl_device_callback)
  {
    registered_handlers->zcl_device_callback(param);
  }
}

void ovsh_zigbee_register_handlers(ovsh_zigbee_handlers_t *handlers)
{
  registered_handlers = handlers;
}

int ovsh_zigbee_enable()
{
  if (zigbee_thread_id != NULL)
  {
    return 0;
  }

  LOG_INF("Enabling Zigbee");

  zigbee_thread_id = k_thread_create(&zigbee_thread,
                                     zigbee_thread_stack, K_THREAD_STACK_SIZEOF(zigbee_thread_stack),
                                     (k_thread_entry_t)zigbee_thread_entry, NULL, NULL, NULL,
                                     1, 0, K_NO_WAIT);
  k_thread_name_set(zigbee_thread_id, "zboss");

  if (registered_handlers && registered_handlers->status)
  {
    registered_handlers->status(true);
  }

  LOG_DBG("Zigbee enabled");
  return 0;
}

// int ovsh_zigbee_disable()
// {
//   if (zigbee_thread_id == NULL)
//   {
//     return 0;
//   }

//   LOG_INF("Disabling Zigbee");

//   zigbee_thread_id = NULL;
//   k_thread_abort(zigbee_thread_id);

//   if (registered_handlers && registered_handlers->disabled)
//   {
//     registered_handlers->disabled();
//   }

//   LOG_DBG("Zigbee disabled");
//   return 0;
// }

void ovsh_zigbee_register_device_context(zb_af_device_ctx_t *context)
{
  ZB_AF_REGISTER_DEVICE_CTX(context);
  ZB_ZCL_REGISTER_DEVICE_CB(zigbee_zcl_device_callback);
}

void ovsh_zigbee_register_identify_handler(uint8_t endpoint)
{
  ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(endpoint, zigbee_identify_handler);
}

void ovsh_zigbee_register_cluster_handler(uint8_t cluster)
{
}

void zboss_signal_handler(zb_uint8_t param)
{
  zb_zdo_app_signal_hdr_t *signal_header = NULL;
  zb_zdo_app_signal_type_t signal_type = zb_get_app_signal(param, &signal_header);
  zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(param);

  switch (signal_type)
  {
  case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    LOG_DBG("Received Zigbee signal ZB_BDB_SIGNAL_DEVICE_FIRST_START (status: %d)", status);

    if (status != 0)
    {
      if (registered_handlers && registered_handlers->connection)
      {
        registered_handlers->connection(false);
      }

      LOG_ERR("Zigbee initialization for the first time failed (status: %d)", status);
      break;
    }

    if (registered_handlers && registered_handlers->connection)
    {
      registered_handlers->connection(true);
    }

    LOG_INF("Zigbee started for the first time");
    break;

  case ZB_BDB_SIGNAL_DEVICE_REBOOT:
    LOG_DBG("Received Zigbee signal ZB_BDB_SIGNAL_DEVICE_REBOOT (status: %d)", status);

    if (status != 0)
    {
      if (registered_handlers && registered_handlers->connection)
      {
        registered_handlers->connection(false);
      }

      LOG_ERR("Zigbee initialization using NVRAM data failed (status: %d)", status);
      break;
    }

    if (registered_handlers && registered_handlers->connection)
    {
      registered_handlers->connection(true);
    }

    LOG_INF("Zigbee started using NVRAM data");
    break;

  case ZB_BDB_SIGNAL_STEERING:
    LOG_DBG("Received Zigbee signal ZB_BDB_SIGNAL_STEERING (status: %d)", status);

    if (status != 0)
    {
      if (registered_handlers && registered_handlers->connection)
      {
        registered_handlers->connection(false);
      }

      LOG_ERR("Zigbee BDB network steering failed (status: %d)", status);
      break;
    }

    if (registered_handlers && registered_handlers->connection)
    {
      registered_handlers->connection(true);
    }
    break;

  case ZB_ZDO_SIGNAL_LEAVE:
  {
    zb_zdo_signal_leave_params_t *leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(signal_header, zb_zdo_signal_leave_params_t);
    LOG_DBG("Received Zigbee signal ZB_ZDO_SIGNAL_LEAVE (status: %d, leave type: %d)", status, leave_params->leave_type);

    if (registered_handlers && registered_handlers->connection)
    {
      registered_handlers->connection(false);
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
    LOG_DBG("Received Zigbee signal ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY (status: %d)", status);

    if (registered_handlers && registered_handlers->connection)
    {
      registered_handlers->connection(false);
    }

    if (status != 0)
    {
      LOG_INF("Zigbee configuration is not present or invalid (status: %d)", status);
      break;
    }

    LOG_INF("Zigbee configuration successfully loaded");
    break;

  default:
    LOG_DBG("Received unknown Zigbee signal %u (status: %d)", signal_type, status);
    break;
  }

  if (param)
  {
    zb_buf_free(param);
  }
}
