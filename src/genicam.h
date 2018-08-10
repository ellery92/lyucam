#pragma once

struct gsystem;
struct gnodemap;
struct ginterface;

extern int
getsystems(struct gsystem *sys[], unsigned int n);

extern void
clearsystems();

extern void
system_getfilename(struct gsystem *sys, char *filename, int n);

extern int
system_open(struct gsystem *sys);

extern void
system_close(struct gsystem *sys);

extern void
system_getinterfaces(struct gsystem *sys, struct ginterface *itf[], int n);

extern void
system_getid(struct gsystem *sys, char *id, int n);

extern void
system_getvendor(struct gsystem *sys, char *vendor, int n);

extern void
system_getmodel(struct gsystem *sys, char *model, int n);

extern void
system_getversion(struct gsystem *sys, char *version, int n);

extern void
system_gettltype(struct gsystem *sys, char *tltype, int n);

extern void
system_getpathname(struct gsystem *sys, char *pathname, int n);

extern void
system_getencoding(struct gsystem *sys, char *encoding, int n);

extern int
system_getmajorversion(struct gsystem *sys);

extern int
system_getminorversion(struct gsystem *sys);

extern struct gnodemap*
system_getnodemap(struct gsystem *sys);

extern void *
system_gethandle(struct gsystem *sys);
