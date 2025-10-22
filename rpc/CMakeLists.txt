cmake_minimum_required(VERSION 3.5)
project(rpc)

include_directories(include)

#系统的线程库。
find_package(Threads REQUIRED)

include(cmake/build.cmake)
include(cmake/develop.cmake)