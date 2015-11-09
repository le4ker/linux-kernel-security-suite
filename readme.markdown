# Linux Kernel Security Suite

[![Join the chat at https://gitter.im/PanosSakkos/linux-kernel-security-suite](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/PanosSakkos/linux-kernel-security-suite?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

A collection of three tools that are designed to protect linux from rootkits.

## The Drip Dry Carbonite

Protects the system call table, by monitoring it.
In case of an attempt of modifying the system call table, it will get a snapshot of the processes running in the system and freeze the machine.

## Dresden

Dresden blocks all the attempts to insert modules in the kernel. In case of attempting to isnert a module, apart from blocking it, it will dump its instruction memory and log a warning message.

## Netlog

Logs all network communication by probing the inet stack of the kernel.

# Supporting the repo

In case you want to support the development, feel free to send a few bits here 17U479M6uMfsqh7vP2ZMKr62pNVjvCNxvu
