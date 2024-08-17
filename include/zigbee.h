#pragma once

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

extern "C"
{
#include <zboss_api.h>
#include <zboss_api_addons.h>

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

    void configure();
    void enable();
    void zboss_signal_handler(zb_uint8_t param);

    static Zigbee *get_instance();

  private:
    k_thread zboss_thread_;
    k_tid_t zboss_thread_id_ = NULL;

    static Zigbee *instance_;

    static void zboss_thread();
  };
}
