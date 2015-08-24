/*
* Reveal hidden kernel modules by brute forcing the Virtual Memory
* Copyright (C) 2013  Panos Sakkos
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/jiffies.h>
#include <linux/stddef.h>
#include "vm-utils.h"
#include "module-utils.h"

#define MODULE_NAME "shedder"

/* Every YIELD_THRESHOLD seconds, shedder will leave the CPU in order to
 * avoid monopolizing it.
 */

#define YIELD_THRESHOLD 0.01

void do_shed(void)
{	
	struct module *module;
	unsigned long time_in_CPU;

	printk(KERN_INFO MODULE_NAME ":\t[+] Brute forcing Virtual Memory of the kernel: pgd(swapper_pg_dir)\n");
	
	time_in_CPU = jiffies;

	for(module = (struct module *) VMALLOC_START; (unsigned long) module < VMALLOC_END; 
				module = (struct module *) ((unsigned long) module + PAGE_SIZE))
	{
		if(looks_like_module(module) && exists_in_the_modules_list(module) == 0)
		{
			printk(KERN_CRIT MODULE_NAME ":\t[+] Found hidden module: %s\n", module->name);

			//TODO Dump the instance for forensics
		}
		
		/* Yield the CPU */
		
		if((jiffies - time_in_CPU) / HZ >= YIELD_THRESHOLD)
		{
			
			schedule();

			time_in_CPU = jiffies;
		}
	}	

	printk(KERN_INFO MODULE_NAME ":\t[+] Finished brute forcing\n");	
}
EXPORT_SYMBOL(do_shed);

int __init shed(void)
{
	printk(KERN_INFO MODULE_NAME ": Hidden module exposure by Panos Sakkos <panos.sakkos@gmail.com>\n");

	if(init_vm_utils() == 0)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Failed to initialize vm_utils\n");

		return -1;
	}
	else
	{
		printk(KERN_INFO MODULE_NAME ":\t[+] Initialized vm_utils\n");
	}	
	
	do_shed();

	return 0;	
}

void __exit shedder_exit(void)
{
	printk(KERN_INFO MODULE_NAME ":\t[+] Exiting\n");
}

module_init(shed);
module_exit(shedder_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Panos Sakkos <panos.sakkos@gmail.com>");
MODULE_DESCRIPTION("reveal hidden kernel modules in the linux kernel. Hidden kernel modules are modules\n"
		"\t\tthat have deleted themselves from the linked list that the kernel keeps the modules");
