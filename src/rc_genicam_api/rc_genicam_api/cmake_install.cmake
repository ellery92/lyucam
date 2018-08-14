# Install script for directory: /home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xbinx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librc_genicam_api.so.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librc_genicam_api.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/librc_genicam_api.so.0.0"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/librc_genicam_api.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librc_genicam_api.so.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/librc_genicam_api.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/genicam/bin/Linux64_x64:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rc_genicam_api" TYPE FILE FILES
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/exception.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/system.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/interface.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/device.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/stream.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/buffer.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/config.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/image.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/imagelist.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/pixel_formats.h"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/project_version.h"
    )
endif()

