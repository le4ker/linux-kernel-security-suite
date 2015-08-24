shedder
=======

Description
-----------

shedder aims to reveal hidden kernel modules in the linux kernel. Hidden kernel modules are modules
that have deleted themselves from the linked list that the kernel keeps the modules.
These hidden modules are not visible from the lsmod. Of course a module gets hidden if it doesn't want to
be found or removed and it's a common technique used by malware.

How to use
------------

In the src directory run `make` to compile the module.
Then run `insmod ./shedder.ko` to insert it.
When you wish to remove it, run `rmmod shedder`.

Supported kernel versions
-------------------------
2.6.32 and later

Kernels tested against
----------------------

3.4.2-4.fc17.x86_64

References
----------

##### Finding hidden kernel modules (the extrem way)
http://www.phrack.org/issues.html?id=3&issue=61

##### Page Table Management
http://kernel.org/doc/gorman/html/understand/understand006.html

##### Four-level page tables
http://lwn.net/Articles/106177/

License
-------

Reveal hidden kernel modules by brute forcing the Virtual Memory
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

