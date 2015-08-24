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

#define MODULE_NAME "dummy_attacker"

static int __init hide(void)
{
	/* Hide ourselves */
	
	list_del(&THIS_MODULE->list);	

	printk(KERN_INFO MODULE_NAME ":\t[+] You can't see me!\n");

	return 0;
}

module_init(hide);

MODULE_AUTHOR("Panos Sakkos <panos.sakkos@gmail.com>");
MODULE_DESCRIPTION("kernel module that hides itself from the kernel by deleting itself form the list of the modules");
MODULE_LICENSE("GPL");
