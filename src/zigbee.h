#pragma once

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

extern "C"
{
#include <zboss_api.h>
  // #include <zb_nrf_platform.h>
  // #include <zigbee/zigbee_error_handler.h>
  // #include <zigbee/zigbee_app_utils.h>

  void zboss_signal_handler(zb_uint8_t param);
}

namespace openvsh
{
  class Zigbee
  {
  public:
    Zigbee();
    Zigbee(const Zigbee &) = delete;
    Zigbee &operator=(const Zigbee &) = delete;

    void enable();

    static Zigbee *get_instance();
    static void zboss_signal_handler(zb_uint8_t param);
    static void zboss_thread();

  private:
    k_thread zboss_thread_;
    k_tid_t zboss_thread_id_;

    static Zigbee *instance;
  };
}
