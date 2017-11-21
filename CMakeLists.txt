cmake_minimum_required(VERSION 3.8)
project(edi)

set(CMAKE_CXX_STANDARD 14)
INCLUDE_DIRECTORIES(/Users/lmz/Downloads/st-1.9/DARWIN_17.2.0_DBG
        /usr/local/include)
LINK_DIRECTORIES(/Users/lmz/Downloads/st-1.9/DARWIN_17.2.0_DBG
        /usr/local/lib)

set(SOURCE_FILES main.cpp stsocket.cpp stsocketbuf.cpp utilities.hpp session.cpp session.h ftpclient.cpp ftpclient.h sessionstream.cpp sessionstream.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} st jemalloc)