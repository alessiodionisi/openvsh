#include "bluetooth.h"

LOG_MODULE_REGISTER(openvsh_bluetooth, CONFIG_OPENVSH_LOG_LEVEL);

static struct bluetooth_callbacks *registered_callbacks;

static const struct bt_data advertisement_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static const struct bt_data scan_response_data[] = {};

static void connected_handler(struct bt_conn *conn, uint8_t err)
{
  if (err)
  {
    LOG_ERR("Bluetooth connection failed (err %u)", err);
    return;
  }

  LOG_DBG("Bluetooth connected");

  if (registered_callbacks && registered_callbacks->connected)
  {
    registered_callbacks->connected();
  }
}

static void disconnected_handler(struct bt_conn *conn, uint8_t reason)
{
  LOG_DBG("Bluetooth disconnected (reason %u)", reason);

  if (registered_callbacks && registered_callbacks->disconnected)
  {
    registered_callbacks->disconnected();
  }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected_handler,
    .disconnected = disconnected_handler,
};

bool bluetooth_disable()
{
  int err;

  err = bt_le_adv_stop();
  if (err)
  {
    LOG_ERR("Bluetooth advertising failed to stop (err %d)", err);
    return false;
  }

  err = bt_disable();
  if (err)
  {
    LOG_ERR("Bluetooth disable failed (err %d)", err);
    return false;
  }

  LOG_DBG("Bluetooth disabled");

  if (registered_callbacks && registered_callbacks->disabled)
  {
    registered_callbacks->disabled();
  }

  return true;
}

bool bluetooth_enable()
{
  int err;

  err = bt_enable(NULL);
  if (err)
  {
    LOG_ERR("Bluetooth enable failed (err %d)", err);
    return false;
  }

  err = bt_le_adv_start(BT_LE_ADV_CONN, advertisement_data, ARRAY_SIZE(advertisement_data), scan_response_data, ARRAY_SIZE(scan_response_data));
  if (err)
  {
    LOG_ERR("Bluetooth advertising failed to start (err %d)", err);
    return false;
  }

  LOG_DBG("Bluetooth enabled");

  if (registered_callbacks && registered_callbacks->enabled)
  {
    registered_callbacks->enabled();
  }

  return true;
}

void bluetooth_register_callbacks(struct bluetooth_callbacks *callbacks)
{
  registered_callbacks = callbacks;
}

bool bluetooth_configure()
{
  return true;
}
