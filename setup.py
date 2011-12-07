# -*- coding:utf-8 -*-
from distutils.core import setup, Extension
import subprocess
import os

pkg_conf = subprocess.check_output("pkg-config --cflags --libs dconf".split())
pkg_conf = pkg_conf.split()
includes = []
libs = []
for i in pkg_conf:
    if i.startswith('-I'):
        includes.append(i[2:])
    elif i.startswith('-l'):
        libs.append(i[2:])
    else:
        libs.append(i[1:])

dconf = Extension('tinydconf',
                 include_dirs = includes,
                 #['/usr/include/dconf',
                 # '/usr/include/glib-2.0',
                 # '/usr/lib/glib-2.0/include'],
                 libraries = libs,
                 #['dconf', 'gio-2.0', 'gobject-2.0',
                 # 'gmodule-2.0', 'gthread-2.0',
                 # 'glib-2.0']
                 sources = [os.path.join('src',
                                         'tinydconfmodule.c')
                           ])

setup(name = 'tinydconf',
      license='MIT',
      version = '0.1',
      description = 'A wrapper around dconf client api.',
      author=u'André Ericson',
      author_email='de.ericson@gmail.com',
      url='https://github.com/aericson/tinydconf',
      long_description = open('README').read(),
      classifiers=[
          'Environment :: Console',
          'Intended Audience :: Gtk Users',
          'License :: OSI Approved :: MIT License',
          'Programming Language :: C/Python',
          'Topic :: Utilities', 
      ],
      ext_modules = [dconf])
