cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(openvsh)

target_include_directories(app PRIVATE include)

# if(CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT)
#   target_sources(app PRIVATE
#     src/on_off_light.c
#   )
# endif()

target_sources(app PRIVATE
  src/bluetooth.c
  src/main.c
  src/on_off_light.c
  src/zb_zcl_on_off_light_settings.c
  src/zigbee.c
)
