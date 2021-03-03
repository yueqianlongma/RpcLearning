cmake_minimum_required(VERSION 3.0.0)
project(JsonServer VERSION 0.1.0)

include(CTest)
enable_testing()

# 设置编译环境， 这里使用c++11标准， 并编译时添加了thread
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11 -pthread -g -march=native")
include_directories(${PROJECT_SOURCE_DIR})


add_subdirectory(./test)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
