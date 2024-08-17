cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(openvsh)

target_include_directories(app PRIVATE include)

target_sources(app PRIVATE
  src/button.cpp
  src/main.cpp
  src/on_off_light.cpp
  src/rgb.cpp
  src/zigbee.cpp
)
