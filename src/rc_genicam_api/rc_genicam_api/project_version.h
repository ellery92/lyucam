#ifndef RC_GENICAM_API_VERSION_H_
#define RC_GENICAM_API_VERSION_H_

/// project version as major.minor.patch string
#define RC_GENICAM_API_VERSION "0.0.0"
/// project version as integer: major * 10000 + minor * 100 + patch
#define RC_GENICAM_API_VERSION_INT 0
#define RC_GENICAM_API_VERSION_MAJOR 0
#define RC_GENICAM_API_VERSION_MINOR 0
#define RC_GENICAM_API_VERSION_PATCH 0
/// package version as string, possibly with git commit: v1.2.3+4+g56789abc
#define RC_GENICAM_API_PACKAGE_VERSION "0.0.0+7+g0ba3446"

///runtime versions, where the above values are linked into a lib and therefore reflect the version
///of the library itself (not the version of the header at compile time of the user code)
const char* rc_genicam_api_runtime_project_version();
const char* rc_genicam_api_runtime_package_version();
int rc_genicam_api_runtime_version_int();
int rc_genicam_api_runtime_version_major();
int rc_genicam_api_runtime_version_minor();
int rc_genicam_api_runtime_version_patch();

///Check consistency of runtime vs compile-time version number. I.e. the header used
///for compilation was from the same version as the linked library.
inline bool rc_genicam_api_check_version_consistency(bool major_minor_only)
{
  return rc_genicam_api_runtime_version_major() == RC_GENICAM_API_VERSION_MAJOR &&
         rc_genicam_api_runtime_version_minor() == RC_GENICAM_API_VERSION_MINOR &&
         (major_minor_only ||
          rc_genicam_api_runtime_version_patch() == RC_GENICAM_API_VERSION_PATCH);
}


#endif
