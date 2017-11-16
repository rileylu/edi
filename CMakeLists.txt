cmake_minimum_required(VERSION 3.8)
project(edi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
INCLUDE_DIRECTORIES(/usr/local/include /usr/local/include/c++/v1)
LINK_DIRECTORIES(/usr/local/lib)

set(SRC_LIST
    "ftpcontext.cpp"
    "ftpsession.cpp"
    "sessionmgr.cpp"
    "state.cpp"
    "main.cpp")

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} pthread boost_system)
