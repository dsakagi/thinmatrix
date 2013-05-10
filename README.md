thinmatrix
==========

Yet another (header-only) C++ implementation of 2D matrices.  I like this one because I also have a python module for IO, so it's really easy to go from numpy to C++

To use the C++ header, just include that header.  It's self contained (except for some standard headers).  It works with C++03 spec (although I would love to use some C++11 tricks in the future).

You can use the save and load methods to persist to disk.  If you use the python module, you can use the read_tm and write_tm for IO as well - they will write and read numpy matrices.
