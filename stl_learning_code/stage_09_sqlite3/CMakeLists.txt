cmake_minimum_required(VERSION 3.10)
project(stage_09_sqlite3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SQLite3 是 C 语言库，C++ 工程可以直接调用它的 C API。
# Ubuntu/Debian 通常需要先安装：
#   sudo apt install libsqlite3-dev
find_package(SQLite3 REQUIRED)

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE SQLite::SQLite3)
