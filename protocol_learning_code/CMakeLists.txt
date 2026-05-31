cmake_minimum_required(VERSION 3.10)
project(protocol_learning_code C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    mongoose
    URL https://github.com/cesanta/mongoose/archive/refs/heads/master.zip
)

FetchContent_MakeAvailable(mongoose)

add_executable(rtsp_client_demo rtsp/rtsp_client_demo.c)
add_executable(rtsp_push_demo rtsp/rtsp_push_demo.c)
add_executable(onvif_probe_demo onvif/onvif_probe_demo.c)
add_executable(mqtt_client_demo mqtt/mqtt_client_demo.c)
add_executable(mqtt_mongoose_client mqtt/mqtt_mongoose_client.c ${mongoose_SOURCE_DIR}/mongoose.c)
add_executable(mqtt_mongoose_broker mqtt/mqtt_mongoose_broker.c ${mongoose_SOURCE_DIR}/mongoose.c)
add_executable(can_socketcan_demo can_socketcan/can_socketcan_demo.c)

target_include_directories(mqtt_mongoose_client PRIVATE ${mongoose_SOURCE_DIR})
target_include_directories(mqtt_mongoose_broker PRIVATE ${mongoose_SOURCE_DIR})

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(FFMPEG QUIET IMPORTED_TARGET libavformat libavcodec libavutil)
endif()

if(FFMPEG_FOUND)
    add_executable(rtsp_ipwebcam_recorder rtsp/rtsp_ipwebcam_recorder.cpp)
    target_link_libraries(rtsp_ipwebcam_recorder PRIVATE PkgConfig::FFMPEG)
else()
    message(STATUS "FFmpeg development libraries not found; rtsp_ipwebcam_recorder will not be built.")
    message(STATUS "Install on Ubuntu/Debian: sudo apt install pkg-config libavformat-dev libavcodec-dev libavutil-dev")
endif()
