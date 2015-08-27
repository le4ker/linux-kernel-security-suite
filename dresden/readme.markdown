# Dresden

Dresden is a kernel module that will log emergency messages for every attempt of inserting or removing a kernel module.
Also, it will block the insertion of every kernel module, by replacing the init functio of the (wannabe) incoming module
with a function crafted for failure. After deploying dresden, you will not be able to remove it. It will delete itself from
the list of the loaded kernel modules.

# Kernels tested against

2.6.18 up to 3.4.9

# How to use

In the src directory run:

	make

Then run:

	insmod ./dresden.ko

# Format of log messages

````
Jul  9 13:36:05 kmod-testing kernel: dresden: Kernel module insertion blocker and action notifier by CERN Security Team
Jul  9 13:36:05 kmod-testing kernel: dresden:   [+] Future loading of kernel modules will be prevented
Jul  9 13:36:05 kmod-testing kernel: dresden:	[+] Emergency messages will be logged in case of trying to load or unload a module
Jul  9 13:36:05 kmod-testing kernel: dresden:	[+] You are not able to remove this module

Trying to insert a module:

[root@kmod-testing netlog]# insmod ./netlog.ko

Message from syslogd@kmod-testing at Jul  9 13:36:52 ...
 kernel:dresden: event: MODULE_STATE_COMING name: netlog init: 0xffffffffa0038000 size of init (text + data)  											0x137e core: 0xffffffffa07c7000 size of core (text + data) 0x20037a8

Message from syslogd@kmod-testing at Jul  9 13:36:52 ...
 kernel:dresden: New module is th name netlog. Its functionality will be disabled

Message from syslogd@kmod-testing at Jul  9 13:36:52 ...
 kernel:dresden: event: MODULE_STATE_GOING name: netlog core: 0xffffffffa07c7000 size of core (text + data) 0x20037a8
insmod: error inserting './netlog.ko': -1 Operation not permitted
[root@kmod-testing netlog]#

Jul  9 13:36:52 kmod-testing kernel: dresden: event: MODULE_STATE_COMING name: netlog init: 0xffffffffa0038000 size of init (text + data)  							0x137e core: 0xffffffffa07c7000 size of core (text + data) 0x20037a8
Jul  9 13:36:52 kmod-testing kernel: dresden: New module is th name netlog. Its functionality will be disabled
Jul  9 13:36:52 kmod-testing kernel: Module insertion blocked by CERN's dresden
Jul  9 13:36:52 kmod-testing kernel: dresden: event: MODULE_STATE_GOING name: netlog core: 0xffffffffa07c7000 size of core (text + data) 0x20037a8
````

*You cannot remove dresden after insertion.*

# License

Copyright 2012 CERN. This software is distributed under the terms of the GNU General Public
Licence version 3 (GPL Version 3), copied verbatim in the file COPYING. In applying this licence,
CERN does not waive the privileges and immunities granted to it by virtue of its status as an
Intergovernmental Organization or submit itself to any jurisdiction.
