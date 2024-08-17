#pragma once

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <functional>

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
    using ConnectedHandler = std::function<void()>;
    using DisconnectedHandler = std::function<void()>;
    using IdentifyHandler = std::function<void(bool identifing)>;

    Zigbee();
    Zigbee(const Zigbee &) = delete;
    Zigbee &operator=(const Zigbee &) = delete;

    // void configure();
    void enable();
    void zboss_signal_handler(zb_uint8_t param);
    void set_disconnected_handler();
    void set_connected_handler(ConnectedHandler handler);
    void set_disconnected_handler(DisconnectedHandler handler);
    void set_identify_handler(uint8_t endpoint, IdentifyHandler handler);

    static Zigbee *get_instance();

  private:
    k_thread zboss_thread_;
    k_tid_t zboss_thread_id_ = NULL;
    ConnectedHandler connected_handler_;
    DisconnectedHandler disconnected_handler_;
    IdentifyHandler identify_handler_;

    static Zigbee *instance_;

    static void zboss_thread();
    static void zboss_identify_handler(zb_uint8_t param);
  };
}
