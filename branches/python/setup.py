from distutils.core import setup, Extension

import os;

cconv = Extension('cconv',
          sources = ['python_cconv.c'],
          include_dirs = ['usr/include', 'usr/local/include', '/usr/local/cconv/include'],
          library_dirs = ['/usr/local/lib', '/usr/local/cconv/lib', '/usr/local/lib64', '/usr/local/cconv/lib64'],
          libraries    = ['cconv']
)

setup (name = 'cconv',
       version     = '0.5.0',
       description = 'A iconv based simplified-traditional chinese conversion tool',
       ext_modules = [cconv])

