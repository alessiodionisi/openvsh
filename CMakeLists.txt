cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(openvsh)

target_sources(app PRIVATE
  src/bluetooth.c
  src/main.c
  src/rgb.c
  src/button.c
)
