cmake_minimum_required(VERSION 3.0.2)
project(lyucam)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(rc_genicam_api)

include_directories(rc_genicam_api/rc_genicam_api)
