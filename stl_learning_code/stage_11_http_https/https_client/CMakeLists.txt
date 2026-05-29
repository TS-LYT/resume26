cmake_minimum_required(VERSION 3.10)
project(https_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE OpenSSL::SSL OpenSSL::Crypto)
