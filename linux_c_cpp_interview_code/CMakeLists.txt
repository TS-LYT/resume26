cmake_minimum_required(VERSION 3.10)
project(linux_c_cpp_interview_code C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(stack_heap_demo c_language_stack_heap/stack_heap_demo.c)
add_executable(pointer_demo pointers_arrays_callbacks/pointer_demo.c)
add_executable(macro_bitfield_demo macro_bitfield/macro_bitfield_demo.c)
add_executable(env_demo environment_programming/env_demo.c)
add_executable(memory_leak_demo memory_leak_debug/memory_leak_demo.c)

add_executable(tcp_server socket_networking/tcp_server.c)
add_executable(tcp_client socket_networking/tcp_client.c)

add_executable(fork_demo process_threading/fork_demo.c)
add_executable(pthread_demo process_threading/pthread_demo.c)
target_link_libraries(pthread_demo PRIVATE Threads::Threads)

add_executable(pipe_demo ipc_examples/pipe_demo.c)
add_executable(shared_memory_demo ipc_examples/shared_memory_demo.c)
