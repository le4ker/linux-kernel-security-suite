# Linux Kernel Security Suite

A collection of three tools that are designed to protect linux from rootkits.

## The Drip Dry Carbonite

Protects the system call table, by monitoring it.
In case of an attempt of modifying the system call table, it will get a snapshot of the processes running in the system and freeze the machine.

## Dresden

Dresden blocks all the attempts to insert modules in the kernel. In case of attempting to isnert a module, apart from blocking it, it will dump its instruction memory and log a warning message.

## Netlog

Logs all network communication by probing the inet stack of the kernel.
