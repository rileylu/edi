cmake_minimum_required(VERSION 2.8)
project(edi)
set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES edi/*.h edi/*.hpp edi/*.cpp)

#INCLUDE_DIRECTORIES(/usr/local/include /Users/lmz/Downloads/st-1.9/DARWIN_17.2.0_DBG)
#LINK_DIRECTORIES(/usr/local/lib /Users/lmz/Downloads/st-1.9/DARWIN_17.2.0_DBG)


add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} jemalloc st)
