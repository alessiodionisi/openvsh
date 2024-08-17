#include "bluetooth.h"

LOG_MODULE_DECLARE(openvsh, CONFIG_OPENVSH_LOG_LEVEL);

static ovsh_bluetooth_callbacks_t *registered_callbacks;

static const struct bt_data advertisement_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static const struct bt_data scan_response_data[] = {};

static void connected_handler(struct bt_conn *conn, uint8_t err)
{
  if (err != 0)
  {
    LOG_ERR("Bluetooth connection failed (err: %u)", err);
    return;
  }

  LOG_INF("Bluetooth connected");

  if (registered_callbacks && registered_callbacks->connected)
  {
    registered_callbacks->connected();
  }
}

static void disconnected_handler(struct bt_conn *conn, uint8_t reason)
{
  LOG_INF("Bluetooth disconnected (reason: %u)", reason);

  if (registered_callbacks && registered_callbacks->disconnected)
  {
    registered_callbacks->disconnected();
  }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected_handler,
    .disconnected = disconnected_handler,
};

int ovsh_bluetooth_disable()
{
  int err;

  err = bt_le_adv_stop();
  if (err != 0)
  {
    LOG_ERR("Bluetooth advertising failed to stop (err: %d)", err);
    return err;
  }

  err = bt_disable();
  if (err != 0)
  {
    LOG_ERR("Bluetooth disable failed (err: %d)", err);
    return err;
  }

  LOG_INF("Bluetooth disabled");

  if (registered_callbacks && registered_callbacks->disabled)
  {
    registered_callbacks->disabled();
  }
}

int ovsh_bluetooth_enable()
{
  int err;

  err = bt_enable(NULL);
  if (err != 0)
  {
    LOG_ERR("Bluetooth enable failed (err: %d)", err);
    return err;
  }

  err = bt_le_adv_start(BT_LE_ADV_CONN,
                        advertisement_data, ARRAY_SIZE(advertisement_data),
                        scan_response_data, ARRAY_SIZE(scan_response_data));
  if (err != 0)
  {
    LOG_ERR("Bluetooth advertising failed to start (err: %d)", err);
    return err;
  }

  LOG_INF("Bluetooth enabled");

  if (registered_callbacks && registered_callbacks->enabled)
  {
    registered_callbacks->enabled();
  }
}

void ovsh_bluetooth_register_callbacks(ovsh_bluetooth_callbacks_t *callbacks)
{
  registered_callbacks = callbacks;
}

int ovsh_bluetooth_configure()
{
  LOG_INF("Configuring Bluetooth");

  LOG_DBG("Bluetooth configured");
}
