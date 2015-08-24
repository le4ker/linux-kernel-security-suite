/*
 *	Security extensions for the linux kernel, as described in "Special Focus Issue: Security" article,
 *	by Rik Farrow at the MAGAZINE OF USENIX & SAGE. The current project merges the 2 suggestions in
 *	a kernel module that is compatible with the lastest versions of the linux kernel and incorporates
 *	some modern security mechanisms. 
 *	Project for the master course "Advanced Operating Systems", instructed by Mema Rousopoulos <http://cgi.di.uoa.gr/~mema/>
 *
 *	Copyright (C) 2013  Panos Sakkos
 *	
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#include <linux/module.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>
#include "my-kallsyms.h"

#define SYMBOL_NOT_FOUND -1
#define MODULE_NAME "sys-call-table-hijacker"

void **sys_call_table;

asmlinkage int (*original_call) (const char*, int, int) = NULL;

/* Enable/Disable page protection. We need it in order to write the sys_call_table*/

static void disable_page_protection(void) 
{
	unsigned long cr0 = read_cr0();

	cr0 &= ~ (1 << 16);
	write_cr0(cr0);
}

static void enable_page_protection(void) 
{
	unsigned long cr0 = read_cr0();

	cr0 |= (1 << 16);
	write_cr0(cr0);
}

static asmlinkage int hijacker_sys_open(const char* file, int flags, int mode)
{
	printk(MODULE_NAME ": PWNED\n");

	return original_call(file, flags, mode);
}

int __init hijack(void)
{
	sys_call_table = (void *) my_kallsyms_lookup_name("sys_call_table");

	if(sys_call_table == NULL)
	{
		printk(MODULE_NAME ":\t[-] sys_call_table symbol not found\n");
		return SYMBOL_NOT_FOUND;
	}    

	original_call = sys_call_table[__NR_open];
	
	disable_page_protection();
	sys_call_table[__NR_open] = hijacker_sys_open;
	enable_page_protection();

	printk(MODULE_NAME ":\t[+] Injected sys_open\n");

	return 0;
}

void __exit cleanup(void)
{
	printk(MODULE_NAME ":\t[+] Cleaning up\n");

	disable_page_protection();
	sys_call_table[__NR_open] = original_call;
	enable_page_protection();
	
	return;
}

module_init(hijack);
module_exit(cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Panos Sakkos <panos.sakkos@gmail.com>");
MODULE_DESCRIPTION("Hijacks the open system call. Use for testing white software only\n");
