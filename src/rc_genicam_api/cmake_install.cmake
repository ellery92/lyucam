# Install script for directory: /home/ellery/workspace/project/lyucam/src/rc_genicam_api

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/rc_genicam_api" TYPE FILE FILES
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/RC_GENICAM_APIConfig.cmake"
    "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/RC_GENICAM_APIConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/rc_genicam_api/RC_GENICAM_APITargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/rc_genicam_api/RC_GENICAM_APITargets.cmake"
         "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/CMakeFiles/Export/lib/rc_genicam_api/RC_GENICAM_APITargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/rc_genicam_api/RC_GENICAM_APITargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/rc_genicam_api/RC_GENICAM_APITargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/rc_genicam_api" TYPE FILE FILES "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/CMakeFiles/Export/lib/rc_genicam_api/RC_GENICAM_APITargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/rc_genicam_api" TYPE FILE FILES "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/CMakeFiles/Export/lib/rc_genicam_api/RC_GENICAM_APITargets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rc_genicam_api" TYPE FILE FILES "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/package.xml")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rc_genicam_api" TYPE FILE PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ FILES "/home/ellery/workspace/project/lyucam/src/rc_genicam_api/net_perf_check.sh")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/ellery/workspace/project/lyucam/src/rc_genicam_api/baumer/cmake_install.cmake")
  include("/home/ellery/workspace/project/lyucam/src/rc_genicam_api/genicam/cmake_install.cmake")
  include("/home/ellery/workspace/project/lyucam/src/rc_genicam_api/rc_genicam_api/cmake_install.cmake")
  include("/home/ellery/workspace/project/lyucam/src/rc_genicam_api/tools/cmake_install.cmake")
  include("/home/ellery/workspace/project/lyucam/src/rc_genicam_api/doc/cmake_install.cmake")

endif()

