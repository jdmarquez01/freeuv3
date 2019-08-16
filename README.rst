README
======

Current build Status:
^^^^^^^^^^^^^^^^^^^^^

|Travis Status| |Appveyor status| |ReadTheDocs Status| |Codecov| |Codacy| |lgtm: C/C++| |License|

freeUV3 is a tiny library for decoding UV3 files which provides in a
simple way (just only 2 calls), a mechanism for extract information
inside of the .UVData and .UVSetup format files using a portable
non-intrusive API.

This project can also serves as template for new projects, it gives a
good CMake base with a few dependencies you most likely want in your
project. It set ups some basic CI builds.

Features
--------

  *  Decode .UV3Data and .UV3Setup files in seamlessly way.
  *  c++ standard only dependency.

Getting Started
---------------
The following small example should contain most of the syntax you need
to use the library.

.. code:: cpp

   # include "uv3decoder.h"

   int main(){
       UV3Decoder decoder("test.UVData");
       if (decoder.good()) {
           auto v = decoder.get();
           for (const auto &n : v) {
               std::ofstream ofs(n->filename_, std::ios::binary);
               ofs << n->buffer_.rdbuf();
               ofs.close();
           }
           decoder.close();
       }
   }

Requirements
------------

The library can be used with C++03. However, it requires C++11 to build,
including compiler and standard library support. The following minimum
versions are required to build the library:
  -  GCC 4.8
  -  Clang 3.4
  -  Visual Studio 2013
  -  Intel 2015 Update 1

.. |Travis Status| image:: https://travis-ci.org/jdmarquez01/freeuv3.svg?branch=master
   :target: https://travis-ci.org/jdmarquez01
.. |Appveyor status| image:: https://ci.appveyor.com/api/projects/status/w6ay1dxx7px29kjr?svg=true
   :target: https://ci.appveyor.com/project/jdmarquez01/freeuv3
.. |ReadTheDocs Status| image:: https://readthedocs.org/projects/freeuv3/badge/?version=latest
   :target: https://freeuv3.readthedocs.io/en/latest/?badge=latest
.. |Codecov| image:: https://codecov.io/gh/jdmarquez01/freeuv3/branch/master/graph/badge.svg
   :target: https://codecov.io/gh/jdmarquez01/freeuv3
.. |Codacy| image:: https://api.codacy.com/project/badge/Grade/5d0b6bce52504bacb94d88857ad03b36
   :target: https://www.codacy.com/app/jdmarquez01/freeuv3?utm_source=github.com&utm_medium=referral&utm_content=jdmarquez01/freeuv3&utm_campaign=Badge_Grade
.. |lgtm: C/C++| image:: https://img.shields.io/lgtm/grade/cpp/g/jdmarquez01/freeuv3.svg?logo=lgtm&logoWidth=18
   :target: https://lgtm.com/projects/g/jdmarquez01/freeuv3/context:cpp
.. |License| image:: https://img.shields.io/badge/license-GPL-blue
   :target: LICENSE
