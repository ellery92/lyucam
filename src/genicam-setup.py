#!/usr/bin/python

from distutils.core import setup, Extension

genicam_module = Extension('_genicam',
                           sources=['genicam_wrap.cxx'],
                           library_dirs=['../rc_genicam_api/rc_genicam_api/'],
                           include_dirs=['../rc_genicam_api/rc_genicam_api/',
                                         "../rc_genicam_api/genicam/library/CPP/include"],
                           libraries=['rc_genicam_api'],
                           extra_compile_args=['-std=c++11'])

setup (name = 'genicam',
       version = '0.1',
       author      = "Ellery Wang",
       description = """GENTL Camera Interface""",
       ext_modules = [genicam_module],
       py_modules = ["genicam"],
       )
