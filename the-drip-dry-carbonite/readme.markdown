The Drip Dry Carbonite
======================

Description
-----------

Security extensions for the linux kernel, as described in "FORENSICS: Loadable Kernel Modules"
article, by Keith J. Jones at the MAGAZINE OF USENIX & SAGE. The current project merges the 2
suggested tools in a kernel module that is compatible with the modern versions of the linux kernel
and will also incorporates some modern security mechanisms.

Project for the master course "Advanced Operating Systems", instructed by Mema Rousopoulos <http://cgi.di.uoa.gr/~mema/>

### Monitoring integrity of the system call table

the-drip-dry-carbonite registers timer interrupts in order to verify the integrity of the
system call table and gets notified after a module event in order to monitor the system call table
after an insertion of a kernel module. The interval between the interrupts can be specified as a
parameter of the module and the default value is every second. If a change in the hash of the system
call table is detected, a critical log will be logged and carbonite functionality will be triggered.

#### Tuning the monitoring

You can set the interval between the monitoring of the hash of the system call table, by setting the
sys_call_table_check_interval parameter while inserting the module. Default value is every 1 second.

You can also disable the monitoring of the system call table, by the setting sys_call_table_monitoring
parameter of the module to 0. Default value is to monitor the system call table.

### Carbonite

By executing trigger-carbonite you will command the kernel module to freeze the system and take
a snapshot of each process. Then you will be able to see the snapshot by reading the
/proc/the-drip-dry-carbonite-dump file. Carbonite functionality should be used by a system administrator,
in order to dump the state of the system and perform forensics on this data.
The carbonite symbol is exported in order to be used by other kernel modules, i.e. by security modules that
detected an incident and they want to get a dump of the system's processes.

#### Remote logging

You can enable remote logging in order to instruct the-drip-dry-carbonite to send a backup of its carbonite logs to
a syslog server. In order to enable the remote logging, you need to specify the IP address of the syslog server as a
parameter (remote_log_ip) to the module while inserting it.

#### Stealth mode

By default the-drip-dry-carbonite is hidden from the kernel and it cannot be removed. In order to disable this behaviour
load the module with parameter "stealth" set to 0.

#### Dumped data

The dumped data of the processes are the pid, uid, gid, state, name and start time (in epoch) of each process.

##### Sample dump

![carbonite dump] (https://dl.dropbox.com/u/8522559/carb.png "sample carbonite dump")

#### Demo

http://www.youtube.com/watch?v=EmklHQn20ZM

Installation
------------

Execute the install script, found under the utils directory. The script will compile the  
user-space tool (trigger-carbonite) and the kernel module. Then it will insert the kernel module  
in the kernel and will place the trigger-carbonite executable under /bin and /usr/bin directories.  
In order to uninstall carbonite, run uninstall script (under utils directory) as root.

Supported kernel versions
-------------------------
2.6.32 and later

Kernels tested against
----------------------

2.6.32-279.14.1.el6.x86_64  
3.4.2-4.fc17.x86_64

Version log
-----------

#### 1.4.2
Added notifier handler after a module event

#### 1.3.2
Added Stealth mode

##### 1.2.2
Added remote logging and fixed several bugs

##### 1.1.1
Added uninstall script

##### 1.0.1
Fixed bug where the incident handler was dispatched during the first check of the system call table

##### 1.0
First version completed

Future Work
-----------

##### Dump more process information
Enviroment variables, arguments, open files, executable

##### Reveal (explicitly) hidden processes

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
