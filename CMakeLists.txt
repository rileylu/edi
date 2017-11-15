cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
INCLUDE_DIRECTORIES(/usr/local/include/c++/v1)
LINK_DIRECTORIES(/usr/local/lib)
project(edi)

set(SRC_LIST
    "ftpcontext.cpp"
    "ftpsession.cpp"
    "sessionmgr.cpp"
    "state.cpp"
    "main.cpp")

add_executable(${PROJECT_NAME} ${SRC_LIST})
