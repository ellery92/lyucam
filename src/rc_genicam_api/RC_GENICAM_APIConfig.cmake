# Config file for rc_genicam_api
#
# It defines the following variables:
#
# RC_GENICAM_API_INCLUDE_DIRS     - include directory
# RC_GENICAM_API_LIBRARIES        - all dynamic libraries
# RC_GENICAM_API_STATIC_LIBRARIES - all static libraries

get_filename_component(PROJECT_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(RC_GENICAM_API_INCLUDE_DIRS "${PROJECT_CMAKE_DIR}/../../include")

include("${PROJECT_CMAKE_DIR}/RC_GENICAM_APITargets.cmake")

set(RC_GENICAM_API_STATIC_LIBRARIES )
set(RC_GENICAM_API_LIBRARIES rc_genicam_api)

list(APPEND RC_GENICAM_API_INCLUDE_DIRS "${PROJECT_CMAKE_DIR}/../../include/rc_genicam_api/genicam")
