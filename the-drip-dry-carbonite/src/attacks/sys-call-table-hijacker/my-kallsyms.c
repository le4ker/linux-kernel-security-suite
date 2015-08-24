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

#include "my-kallsyms.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/version.h>

unsigned long target_address;

int find(void *data, const char *name, struct module *module, unsigned long address)
{
	char *target_name = (char *) data;
	
	if(!strncmp(target_name, name, KSYM_NAME_LEN))
	{
		target_address = address;
		return 1;
	}
	
	return 0;
}

int return_next_symbol;

int find_next(void *data, const char *name, struct module *module, unsigned long address)
{
	unsigned long target = * (unsigned long *) data;
	
	if(target == address)
	{
		return_next_symbol = 1;
		return 0;
	}
	
	if(return_next_symbol)
	{
		target_address = address;
		return 1;		
	}
	
	return 0;
}

unsigned long find_next_symbol(unsigned long target_symbol)
{
	target_address = 0;
	return_next_symbol = 0;

	kallsyms_on_each_symbol(&find_next, (void *) &target_symbol);

	return target_address;
}

unsigned long my_kallsyms_lookup_name(const char *name)
{
	#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 32)
		target_address = 0;
		
		kallsyms_on_each_symbol(&find, (void *) name);
	
		return target_address;
	#else
		return kallsyms_lookup_name(name);
	#endif	
}
