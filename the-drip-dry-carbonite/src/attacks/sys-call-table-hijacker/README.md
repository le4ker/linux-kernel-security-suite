sys-call-table-hijacker
=======================

Description
-----------

A simple rootkit that hijacks the open system call and replaces it
with one that has implemented itself. This is a "dummy" open that simply
calls the original. Interesting part of this source is the bypassing of the
page protection, by setting the 16-th bit of Control Register 0.

USE THIS WORK ONLY FOR TESTING SOFTWARE WITH WHITE PURPOSES

Compilation and insertion of the module
---------------------------------------

In the src directory run `make` and then `insmod ./hijacker.ko`, as root.

Supported kernel versions
-------------------------
2.6.32 and later

Kernels Tested against
----------------------

2.6.32-279.14.1.el6.x86_64  
3.4.2-4.fc17.x86_64

### License

Security extensions for the linux kernel, as described in "FORENSICS: Loadable Kernel Modules"
article, by Keith J. Jones at the MAGAZINE OF USENIX & SAGE. The current project merges the 2
suggested tools in a kernel module that is compatible with the modern versions of the linux kernel
and will also incorporates some modern security mechanisms. 

Project for the master course "Advanced Operating Systems", instructed by Mema Rousopoulos <http://cgi.di.uoa.gr/~mema/>

Copyright (C) 2013  Panos Sakkos

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
