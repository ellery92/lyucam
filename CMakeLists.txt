cmake_minimum_required(VERSION 3.0.2)
project(lyucam)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src/rc_genicam_api)

include_directories(src/rc_genicam_api)
include_directories(src/rc_genicam_api/genicam/library/CPP/include)

find_package(Qt5Widgets CONFIG REQUIRED)
include_directories(Qt5Widgets_INCLUDE_DIRS)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB CAM_SRCS
  "src/qtgui/*.cpp"
  "src/qtgui/imageviewer/*.cpp")

find_package (Threads)

add_executable(camdemo ${CAM_SRCS})
target_link_libraries(camdemo Qt5::Widgets Qt5::Gui Qt5::Core)
target_link_libraries(camdemo rc_genicam_api_static)
target_link_libraries (camdemo ${CMAKE_THREAD_LIBS_INIT})
