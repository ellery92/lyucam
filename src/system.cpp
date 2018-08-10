#include "genicam.h"
#include <system.h>
#include <memory>

#ifndef max
#define max(a, b) (a) > (b) ? (a) : (b)
#endif
#ifndef min
#define min(a, b) (a) < (b) ? (a) : (b)
#endif
using namespace rcg;
using namespace std;

struct gsystem {
  std::shared_ptr<System> ps;
};

static std::vector<struct gsystem> gsystems;
#define real_sys(s) (s->ps)

void string2cstr(char *dst, unsigned int n, std::string &src)
{
  n = min(n-2, src.size());
  for (size_t i = 0; i < n; i++)
    dst[i] = src[i];
  dst[n-1] = '\0';
}

int
getsystems(struct gsystem *sys[], unsigned int n)
{
  std::vector<std::shared_ptr<System> s = System::getSystems();

  n = min(n, s.size());
  struct gsystem gs;
  for (size_t i = 0; i < n; i++) {
    gs.ps = s[i];
    gsystems.push_back(gs);
    sys[i] = &gsystems[i];
  }
  return n;
}

void
clearsystems()
{
  gsystems.clear();
  System::clearSystems();
}

void
system_getfilename(struct gsystem *sys, char *filename, int n)
{
  std::string name = real_sys(sys)->getFilename();
  string2cstr(filename, n, name);
}

int
system_open(struct gsystem *sys)
{
  try {
    real_sys(sys)->open();
  } catch (...) {
    return -1;
  }
  return 0;
}

void
system_close(struct gsystem *sys)
{
  real_sys(sys)->close();
}

void
system_getinterfaces(struct gsystem *sys, struct ginterface *itf[], int n)
{

}

void
system_getid(struct gsystem *sys, char *id, int n)
{
  std::string ID = real_sys(sys)->getID();
  string2cstr(id, n, ID);
}

void
system_getvendor(struct gsystem *sys, char *vendor, int n)
{
  std::string str = real_sys(sys)->getVendor();
  string2cstr(vendor, n, str);
}

void
system_getmodel(struct gsystem *sys, char *model, int n)
{
  std::string str = real_sys(sys)->getModel();
  string2cstr(model, n, str);
}

void
system_getversion(struct gsystem *sys, char *version, int n)
{
  std::string str = real_sys(sys)->getVersion();
  string2cstr(version, n, str);
}

void
system_gettltype(struct gsystem *sys, char *tltype, int n)
{
  std::string str = real_sys(sys)->getTLType();
  string2cstr(tltype, n, str);
}

void
system_getpathname(struct gsystem *sys, char *pathname, int n)
{
  std::string str = real_sys(sys)->getPathname();
  string2cstr(pathname, n, str);
}

void
system_getencoding(struct gsystem *sys, char *encoding, int n)
{
  std::string str = "UTF-8";
  if (real_sys(sys)->isCharEncodingASCII())
    str = "ASCII";
  string2cstr(encoding, n, str);
}

int
system_getmajorversion(struct gsystem *sys)
{
  int v = real_sys(sys)->getMajorVersion();
  return v;
}

int
system_getminorversion(struct gsystem *sys)
{
  int v = real_sys(sys)->getMinorVersion();
  return v;
}

struct gnodemap*
system_getnodemap(struct gsystem *sys)
{
  // int v = real_sys(sys)->getNodeMap();
  return 0;
}

void *
system_gethandle(struct gsystem *sys)
{
  void *v = real_sys(sys)->getHandle();
  return v;
}
