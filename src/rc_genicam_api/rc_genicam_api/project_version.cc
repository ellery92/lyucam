#include "project_version.h"
/// project version as major.minor.patch string
const char* rc_genicam_api_runtime_project_version(){ return "0.0.0"; }
/// package version as string, possibly with git commit: v1.2.3+4+g56789abc
const char* rc_genicam_api_runtime_package_version(){ return "0.0.0+7+g0ba3446"; }
/// project version as integer: major * 10000 + minor * 100 + patch
int rc_genicam_api_runtime_version_int()  { return 0; }
/// project version as integer: major
int rc_genicam_api_runtime_version_major(){ return 0; }
/// project version as integer: minor
int rc_genicam_api_runtime_version_minor(){ return 0; }
/// project version as integer: patch
int rc_genicam_api_runtime_version_patch(){ return 0; }

