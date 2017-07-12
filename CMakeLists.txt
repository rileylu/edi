cmake_minimum_required(VERSION 3.7)
project(edi)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp EventLoop.cpp EventLoop.hpp Noncopyable.hpp Channel.cpp Channel.hpp)
add_executable(edi ${SOURCE_FILES})