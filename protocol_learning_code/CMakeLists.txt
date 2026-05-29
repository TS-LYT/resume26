cmake_minimum_required(VERSION 3.10)
project(protocol_learning_code C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(rtsp_client_demo rtsp/rtsp_client_demo.c)
add_executable(onvif_probe_demo onvif/onvif_probe_demo.c)
add_executable(mqtt_client_demo mqtt/mqtt_client_demo.c)
add_executable(can_socketcan_demo can_socketcan/can_socketcan_demo.c)
