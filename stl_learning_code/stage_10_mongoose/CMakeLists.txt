cmake_minimum_required(VERSION 3.14)
project(stage_10_mongoose)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Mongoose 官方推荐的集成方式之一是把 mongoose.c 和 mongoose.h 放进工程。
# 这里用 CMake 在首次编译时自动下载这两个单文件源码。
FetchContent_Declare(
    mongoose
    URL https://github.com/cesanta/mongoose/archive/refs/heads/master.zip
)

FetchContent_MakeAvailable(mongoose)

add_executable(demo main.cpp ${mongoose_SOURCE_DIR}/mongoose.c)
target_include_directories(demo PRIVATE ${mongoose_SOURCE_DIR})
